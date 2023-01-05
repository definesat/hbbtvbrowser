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

#ifndef browserwindow_h
#define browserwindow_h

#include <QtNetwork/QNetworkRequest>

#ifndef QT_NO_OPENGL
#include <QtOpenGL/QGLWidget>
#endif

#include <QDebug>
#include <QLayout>
#include <QtWidgets/QProgressBar>
#include <QProgressDialog>

#include <cstdio>
#include <qevent.h>
#include <qwebframe.h>
#include <qwebinspector.h>
#include <qwebsettings.h>

#include "utils.h"
#include "webinspector.h"
#include "webpage.h"
#include "webview.h"

#include <QEvent>


QT_BEGIN_NAMESPACE
class QPropertyAnimation;
QT_END_NAMESPACE

class BrowserOptions {
public:
    BrowserOptions()
        : useGraphicsView(false)
        , useDiskCache(false)
        , useCompositing(true)
        , useTiledBackingStore(false)
        , useWebGL(false)
        , useFrameFlattening(false)
        , cacheWebView(false)
        , showFrameRate(false)
        , resizesToContents(false)
        , viewportUpdateMode(QGraphicsView::MinimalViewportUpdate)
        , useLocalStorage(false)
        , useOfflineStorageDatabase(false)
        , useOfflineWebApplicationCache(false)
        , useDiskCookies(true)
        , enableScrollAnimator(false)
        , offlineStorageDefaultQuotaSize(0)
#ifndef QT_NO_OPENGL
        , useQGLWidgetViewport(false)
#endif
        , startMaximized(false)
        , enableMouseContextMenu(false)
        , enableJavaScriptConsoleMessage(false)
    {
    }

    bool useGraphicsView;
    bool useDiskCache;
    bool useCompositing;
    bool useTiledBackingStore;
    bool useWebGL;
    bool useFrameFlattening;
    bool cacheWebView;
    bool showFrameRate;
    bool resizesToContents;
    QGraphicsView::ViewportUpdateMode viewportUpdateMode;
    bool useLocalStorage;
    bool useOfflineStorageDatabase;
    bool useOfflineWebApplicationCache;
    bool useDiskCookies;
    bool enableScrollAnimator;
    quint64 offlineStorageDefaultQuotaSize;
#ifndef QT_NO_OPENGL
    bool useQGLWidgetViewport;
#endif
    bool startMaximized;
    bool enableMouseContextMenu;
    bool enableJavaScriptConsoleMessage;
    QString userAgentForUrl;
    QString defaultURL;

};

class BrowserWindow : public QWidget {
    Q_OBJECT

public:
    BrowserWindow(BrowserOptions* data = 0);
    virtual ~BrowserWindow();

    void load(const QUrl& url);
    WebPage* page() const;
    void setPage(WebPage*);

    void printSettings();
    /* by Jin-Chul */
    bool event(QEvent *event);

#if 0
    void javaScriptWindowObjectCleared(void);
#endif
protected Q_SLOTS:
    void loadStarted();
    void setProgress(int p);
    void loadFinished();

    void dumpHtml();

    void setDiskCache(bool enable);
    void toggleWebView(bool graphicsBased);
    void toggleAcceleratedCompositing(bool toggle);
    void toggleTiledBackingStore(bool toggle);
    void toggleResizesToContents(bool toggle);
    void toggleWebGL(bool toggle);
    void toggleSpatialNavigation(bool b);
    void toggleFullScreenMode(bool enable);
    void toggleFrameFlattening(bool toggle);
    void toggleInterruptingJavaScriptEnabled(bool enable);
    void toggleJavascriptCanOpenWindows(bool enable);
    void toggleAutoLoadImages(bool enable);
    void setUseDiskCookies(bool enable);
    void clearCookies();
    void togglePlugins(bool enable);
    void toggleLocalStorage(bool toggle);
    void toggleOfflineStorageDatabase(bool toggle);
    void toggleOfflineWebApplicationCache(bool toggle);
    void toggleScrollAnimator(bool toggle);
    void setOfflineStorageDefaultQuota();

    void changeViewportUpdateMode(int mode);
    void showFPS(bool enable);

public Q_SLOTS:
    void updateFPS(int fps);
    void Title(const QString &title);
    void URLChanged(const QUrl &url);

Q_SIGNALS:
    void enteredFullScreenMode(bool on);

private:
    void init();
    void initializeView();
    void applyPrefs();
    bool isGraphicsBased() const;
    bool isGLWidgetBased() const;

private:
    QWidget* m_view;
    WebPage* m_page;
    BrowserOptions m_windowOptions;
    QString m_windowTitle;
    QLinkedList<QString> jsFiles;
    QProgressBar *progress;
    QProgressDialog * progressBar;
    QGridLayout *mainLayout;
};

#if 0
class MyKeyEvent : public QObject {
    Q_OBJECT

public:
    int VK_RED = Qt::Key_Red;
    int VK_GREEN = Qt::Key_Green;
    int VK_YELLOW = Qt::Key_Yellow;
    int VK_BLUE = Qt::Key_Blue;
};
#endif


#endif
