/******************************************************************************
 *   Copyright 2013 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
 * AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
 * SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 * RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
 * IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
 * A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
 * ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE
 * ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 * ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 * INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
 * RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
 * EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
 * FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *****************************************************************************/

#include "browserapp.h"
#include "browserwindow.h"
#include "utils.h"

#include <QDebug>
#include <QWebSettings>
#include <QGraphicsView>

extern BrowserOptions browserOptions;

static void requiresGraphicsView(const QString& option)
{
    if (browserOptions.useGraphicsView)
        return;
    appQuit(1, QString("%1 only works in combination with the -graphicsbased option").arg(option));
}

void BrowserApp::applyDefaultSettings()
{
    QWebSettings::setMaximumPagesInCache(4);

    QWebSettings::setObjectCacheCapacities((16*1024*1024) / 8, (16*1024*1024) / 8, 16*1024*1024);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, browserOptions.enableMouseContextMenu);
    QWebSettings::enablePersistentStorage();
}

BrowserApp::BrowserApp(int& argc, char** argv)
    : QApplication(argc, argv)
{
    // To allow QWebInspector's configuration persistence
    setOrganizationName("Broadcom");
    setApplicationName("qtsimplebrowser");
    setApplicationVersion("0.1");

    handleUserOptions();

    applyDefaultSettings();
}

void BrowserApp::handleUserOptions()
{
    int modeIndex = 0;
    QString arg_opt;
    QStringList args = arguments();
    QFileInfo program(args.at(0));
    QString programName("qtsimplebrowser");
    if (program.exists())
        programName = program.baseName();

    QList<QString> updateModes(enumToKeys(QGraphicsView::staticMetaObject,
            "ViewportUpdateMode", "ViewportUpdate"));

    if (args.contains("-help")) {
        qDebug() << "Usage:" << programName.toLatin1().data()
             << "[-graphicsbased]"
             << "[-no-compositing]"
#if defined(QT_CONFIGURED_WITH_OPENGL)
             << "[-gl-viewport]"
             << "[-webgl]"
#endif
             << QString("[-viewport-update-mode %1]").arg(formatKeys(updateModes)).toLatin1().data()
#if !defined(QT_NO_NETWORKDISKCACHE) && !defined(QT_NO_DESKTOPSERVICES)
             << "[-disk-cache]"
#endif
             << "[-cache-webview]"
             << "[-maximize]"
             << "[-show-fps]"
             << "[-tiled-backing-store]"
             << "[-resizes-to-contents]"
             << "[-local-storage-enabled]"
             << "[-no-disk-cookies]"
             << "[-offline-storage-database-enabled]"
             << "[-offline-web-application-cache-enabled]"
             << "[-set-offline-storage-default-quota maxSize]"
             << "[-mouse-context-menu]"
             << "[-user-agent]"
             << "URL";
        appQuit(0);
    }

    const bool defaultForAnimations = args.contains("-default-animations");
    if (args.contains("-graphicsbased") || defaultForAnimations)
        browserOptions.useGraphicsView = true;

    if (args.contains("-no-compositing")) {
        requiresGraphicsView("-no-compositing");
        browserOptions.useCompositing = false;
    }

    if (args.contains("-show-fps")) {
        requiresGraphicsView("-show-fps");
        browserOptions.showFrameRate = true;
    }

    if (args.contains("-disk-cache")) {
#if !defined(QT_NO_NETWORKDISKCACHE) && !defined(QT_NO_DESKTOPSERVICES)
        browserOptions.useDiskCache = true;
#else
    appQuit(1, "-disk-cache only works if QNetworkDiskCache and QDesktopServices is enabled in your Qt build.");
#endif
    }

    if (args.contains("-cache-webview") || defaultForAnimations) {
        requiresGraphicsView("-cache-webview");
        browserOptions.cacheWebView = true;
    }

    if (args.contains("-tiled-backing-store")) {
        requiresGraphicsView("-tiled-backing-store");
        browserOptions.useTiledBackingStore = true;
    }

    if (args.contains("-resizes-to-contents")) {
        requiresGraphicsView("-resizes-to-contents");
        browserOptions.resizesToContents = true;
    }
    
    if (args.contains("-local-storage-enabled"))
        browserOptions.useLocalStorage = true;

    if (args.contains("-no-disk-cookies"))
        browserOptions.useDiskCookies = false;

    if (args.contains("-maximize"))
        browserOptions.startMaximized = true;

    if (args.contains("-offline-storage-database-enabled"))
        browserOptions.useOfflineStorageDatabase = true;
        
    if (args.contains("-offline-web-application-cache-enabled"))   
        browserOptions.useOfflineWebApplicationCache = true;
    
    int setOfflineStorageDefaultQuotaIndex = args.indexOf("-set-offline-storage-default-quota");
    if (setOfflineStorageDefaultQuotaIndex != -1) {
        unsigned int maxSize = takeOptionValue(&args, setOfflineStorageDefaultQuotaIndex).toUInt();
        browserOptions.offlineStorageDefaultQuotaSize = maxSize;
    }   
    
    if (defaultForAnimations)
        browserOptions.viewportUpdateMode = QGraphicsView::BoundingRectViewportUpdate;

    if (args.contains("-mouse-context-menu"))
        browserOptions.enableMouseContextMenu = true;

    if (args.contains("-js-console-message"))
        browserOptions.enableJavaScriptConsoleMessage = true;

    arg_opt = "-viewport-update-mode";
    modeIndex = args.indexOf(arg_opt);
    if (modeIndex != -1) {
        requiresGraphicsView(arg_opt);

        QString mode = takeOptionValue(&args, modeIndex);
        if (mode.isEmpty())
            appQuit(1, QString("%1 needs a value of one of [%2]").arg(arg_opt).arg(formatKeys(updateModes)));
        int idx = updateModes.indexOf(mode);
        if (idx == -1)
            appQuit(1, QString("%1 value has to be one of [%2]").arg(arg_opt).arg(formatKeys(updateModes)));

        browserOptions.viewportUpdateMode = static_cast<QGraphicsView::ViewportUpdateMode>(idx);
    }

#ifdef QT_CONFIGURED_WITH_OPENGL
    if (args.contains("-gl-viewport") || defaultForAnimations) {
        requiresGraphicsView("-gl-viewport");
        browserOptions.useQGLWidgetViewport = true;
    }

    if (args.contains("-webgl")) {
        requiresGraphicsView("-webgl");
        browserOptions.useWebGL = true;
    }
#endif

    arg_opt = "-user-agent";
    modeIndex = args.indexOf(arg_opt);
    if (modeIndex != -1) {
        browserOptions.userAgentForUrl = takeOptionValue(&args, modeIndex);
    }

    int lastArg = args.lastIndexOf(QRegExp("^-.*"));
    QStringList list = (lastArg != -1) ? args.mid(++lastArg) : args.mid(1);
    if (!list.isEmpty())
        m_url = list.at(0);
}

