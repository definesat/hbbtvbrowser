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

//#include "config.h"
#include <QApplication>
//#include <linux/input.h>
//#include <qpa/qwindowsysteminterface.h>
#include "browserwindow.h"
#include "cookiejar.h"


#if !defined(QT_NO_NETWORKDISKCACHE) && !defined(QT_NO_DESKTOPSERVICES)
#include <QStandardPaths>
#include <QtNetwork/QNetworkDiskCache>
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <getopt.h>

extern BrowserOptions browserOptions;

static TestBrowserCookieJar* testBrowserCookieJarInstance()
{
    static TestBrowserCookieJar* cookieJar = new TestBrowserCookieJar(qApp);
    return cookieJar;
}


BrowserWindow::BrowserWindow(BrowserOptions* data)
    : QWidget()
    , m_view(0)
    , m_page(0)
{

    if (data)
        m_windowOptions = *data;

#if 0
    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
       printf("socket_error\n");

    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, "/tmp/.browser.support");

    if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
      printf("connect_error\n");
      exit(1);
      return;
    }
#endif

    init();

}

BrowserWindow::~BrowserWindow()
{
}

void BrowserWindow::init()
{
    resize(1280, 720);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    if (browserOptions.startMaximized)
        toggleFullScreenMode(true);

    initializeView();
}

void BrowserWindow::initializeView()
{

    if (m_view) {
	m_view->removeEventFilter(this); 
        delete m_view;
        m_view = 0;
    }


    //printf("======================== %s\n", __FUNCTION__);

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(0);
    setLayout(mainLayout);

    WebPage* webPage = new WebPage(this);
    setPage(webPage);
    setDiskCache(m_windowOptions.useDiskCache);
    setUseDiskCookies(m_windowOptions.useDiskCookies);

#if 0
    QFile file;
    file.setFileName("/home/root/keyevent.js");
    file.open(QIODevice::ReadOnly);
    jsFiles.push_back(file.readAll());
    file.close();
    printf( "loaded keyevent.js\n");
#else

	//QString script = "var KeyEvent = {VK_RED:116, VK_GREEN:117, VK_BLUE:118, VK_BLUE:119, VK_ENTER:13, VK_LEFT:37, VK_RIGHT:39, VK_UP:38, VK_DOWN:40, VK_PLAY:179, VK_PAUSE:179, VK_PLAY_PUASE:179, VK_STOP:178, VK_FAST_FWD: 123, VK_REWIND: 122};";
	QString script = "var KeyEvent = {VK_RED:403, VK_YELLOW:405, VK_GREEN:404, VK_BLUE:406, VK_ENTER:13, VK_LEFT:37, VK_RIGHT:39, VK_UP:38, VK_DOWN:40, VK_PLAY:0, VK_PAUSE:179, VK_PLAY_PUASE:0, VK_STOP:178, VK_FAST_FWD: 123, VK_REWIND: 122};";

    jsFiles.push_back(script);
    //printf( "loaded keyevent.js\n");
#endif
    // We reuse the same cookieJar on multiple QNAMs, which is OK.
    QObject* cookieJarParent = testBrowserCookieJarInstance()->parent();
    page()->networkAccessManager()->setCookieJar(testBrowserCookieJarInstance());
    testBrowserCookieJarInstance()->setParent(cookieJarParent);

    if (!m_windowOptions.useGraphicsView) {
        WebViewTraditional* view = new WebViewTraditional(this);
        mainLayout->addWidget((QWidget*)view, 0, 0);
        m_view = view;
        view->setPage(page());
    } else {
        WebViewGraphicsBased* view = new WebViewGraphicsBased(this);
        mainLayout->addWidget((QWidget*)view, 0, 0);
        m_view = view;

#ifndef QT_NO_OPENGL
        bool enable = m_windowOptions.useQGLWidgetViewport;
        if (enable && isGraphicsBased()) {
            WebViewGraphicsBased* view = static_cast<WebViewGraphicsBased*>(m_view);
            view->setViewport(enable ? new QGLWidget() : 0);
        }
#endif
        view->setPage(page());
        connect(view, SIGNAL(currentFPSUpdated(int)), this, SLOT(updateFPS(int)));
    }
    m_view->installEventFilter(this);


    connect(page(), SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(page(), SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(page(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinished()));
    connect(this, SIGNAL(enteredFullScreenMode(bool)), this, SLOT(toggleFullScreenMode(bool)));

    /* by Jin-Chul */
    connect(m_view, SIGNAL(titleChanged(QString)),this, SLOT(Title(const QString)));
    connect(m_view, SIGNAL(urlChanged(QUrl)),this, SLOT(URLChanged(const QUrl)));


    applyPrefs();

    progressBar = new QProgressDialog(m_view, Qt::FramelessWindowHint);
    //progressBar->setWindowOpacity(1.0);
    progressBar->setCancelButton(0);
    progressBar->setAutoFillBackground(false);
    progressBar->setAttribute(Qt::WA_TranslucentBackground, true);

    //progressBar->setObjectName(QStringLiteral("progressBar"));
    progressBar->setValue(0);
#if 0 //mutant style
    progressBar->setFixedSize(300, 30);
    progressBar->setStyleSheet("background-color:white;");
    //mainLayout->addWidget( progressBar, 0, 0, Qt::AlignHCenter);
#else //vuplus style
    progressBar->setStyleSheet("QProgressDialog {background-color:transparent;}");
    progress = new QProgressBar(m_view);

    progress->setTextVisible(false);
    progress->setStyleSheet("QProgressBar { background-color: #999999; border: 1px solid grey; border-radius: 5 px; padding: 1px; text-align: center; } QProgressBar::chunk { background: QLinearGradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #0034A7, stop: 1 #0064c7); border-radius: 5 px; margin: 0px; }");
    progressBar->setBar(progress);

    progressBar->setGeometry(240, 630, 800, 8);
#endif

    toggleSpatialNavigation(true);


}

void BrowserWindow::applyPrefs()
{
    QWebSettings* settings = page()->settings();
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, m_windowOptions.useCompositing);
    settings->setAttribute(QWebSettings::TiledBackingStoreEnabled, m_windowOptions.useTiledBackingStore);
    settings->setAttribute(QWebSettings::FrameFlatteningEnabled, m_windowOptions.useFrameFlattening);
    settings->setAttribute(QWebSettings::WebGLEnabled, m_windowOptions.useWebGL);


//    QString string = "iso-8859-1";
 //   settings->setDefaultTextEncoding(string);

    if (!isGraphicsBased())
        return;

    WebViewGraphicsBased* view = static_cast<WebViewGraphicsBased*>(m_view);
    view->setViewportUpdateMode(m_windowOptions.viewportUpdateMode);
    view->setFrameRateMeasurementEnabled(m_windowOptions.showFrameRate);
    view->setItemCacheMode(m_windowOptions.cacheWebView ? QGraphicsItem::DeviceCoordinateCache : QGraphicsItem::NoCache);

    if (m_windowOptions.resizesToContents)
        toggleResizesToContents(m_windowOptions.resizesToContents);
}

void BrowserWindow::setPage(WebPage* page)
{
    //printf("======================== %s\n", __FUNCTION__);
    if (page && m_page)
    {
        if (browserOptions.userAgentForUrl.isEmpty())
            page->setUserAgent(m_page->userAgentForUrl(QUrl()));
        else
            page->setUserAgent(browserOptions.userAgentForUrl);
    }
    page->setUserAgent("Mozilla/5.0 (DirectFB; Linux armv7l) AppleWebkit/535.4+ (KHTML, like Gecko) Version/5.0HbbTV/1.1.1 (+DL+PVR;;;;;) CE-HTML/1.0");
//Crash on Linux
//    if (m_page)
//        delete m_page;
    m_page = page;
}

WebPage* BrowserWindow::page() const
{
    return m_page;
}

void BrowserWindow::load(const QUrl& url)
{
    if (!url.isValid())
        return;

    //printf("======================== %s\n", __FUNCTION__);

//QString code = "qt.jQuery('[src*=gif]').remove()";
//page()->mainFrame()->evaluateJavascript(scriptSource);
//page()->mainFrame()->evaluateJavascript("var KeyEvent = {VK_RED:52, VK_GREEN:53, VK_BLUE:54, VK_BLUE:55};");
//page()->mainFrame()->evaluateJavaScript(it.next());

    page()->mainFrame()->load(url);
}

bool BrowserWindow::isGraphicsBased() const
{
    return bool(qobject_cast<QGraphicsView*>(m_view));
}

bool BrowserWindow::isGLWidgetBased() const
{
    bool ret = false;
    if (isGraphicsBased())  {
        QGraphicsView* view = static_cast<QGraphicsView*>(m_view);
        ret = qobject_cast<QGLWidget*>(view->viewport());
    }
    return ret;
}

void BrowserWindow::loadStarted()
{
    m_view->setFocus(Qt::OtherFocusReason);
	qDebug("BrowserWindow::loadStarted");

    progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setVisible(true);
}

void BrowserWindow::setProgress(int p)
{
    //printf( "%s: %d %%\n", __func__, p);

    progressBar->setValue(p);
}

void BrowserWindow::loadFinished()
{
    int sockfd;
    struct sockaddr_un servaddr;
    int data[1024];
    char *c_data = (char *)data;
    int ret;
    QByteArray byteName = m_windowTitle.toLocal8Bit();
    char *title_name = byteName.data();
    qDebug("BrowserWindow::loadFinished");
    //printf("***********************************>Title %s\n", title_name);
    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
    	//printf("socket_error\n");
        return;
    }
    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, "/tmp/.browser.support");

    if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
    	//printf("connect_error\n");
        ::close(sockfd);
        return;
    }
    memset(data,0,1024);
    data[0] = 6; /*CONTROL_TITLE*/
    data[2] = byteName.length();
    memcpy(&c_data[12], title_name, byteName.length());
    ret = write(sockfd,data,1024);
    ret = read(sockfd,data,1024);
    ::close(sockfd);
#if 0
    mainLayout->removeWidget(progressBar);
    mainLayout->removeItem(progressBottom);
#endif
	progressBar->setVisible(false);
}

void BrowserWindow::dumpHtml()
{
    qDebug("HTML: %s", qPrintable(m_page->mainFrame()->toHtml()));
}

void BrowserWindow::setDiskCache(bool enable)
{
#if !defined(QT_NO_NETWORKDISKCACHE) && !defined(QT_NO_DESKTOPSERVICES)
    m_windowOptions.useDiskCache = enable;
    QNetworkDiskCache* cache = 0;
    if (enable) {
        cache = new QNetworkDiskCache();
        QString cacheLocation = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        cache->setCacheDirectory(cacheLocation);
    }
    page()->networkAccessManager()->setCache(cache);
#endif
}

void BrowserWindow::toggleWebView(bool graphicsBased)
{
    m_windowOptions.useGraphicsView = graphicsBased;
    initializeView();
}

void BrowserWindow::toggleAcceleratedCompositing(bool toggle)
{
    m_windowOptions.useCompositing = toggle;
    page()->settings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, toggle);
}

void BrowserWindow::toggleTiledBackingStore(bool toggle)
{
    page()->settings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, toggle);
}

void BrowserWindow::toggleResizesToContents(bool toggle)
{
    m_windowOptions.resizesToContents = toggle;
    static_cast<WebViewGraphicsBased*>(m_view)->setResizesToContents(toggle);
}

void BrowserWindow::toggleWebGL(bool toggle)
{
    m_windowOptions.useWebGL = toggle;
    page()->settings()->setAttribute(QWebSettings::WebGLEnabled, toggle);
}

void BrowserWindow::toggleSpatialNavigation(bool b)
{
    page()->settings()->setAttribute(QWebSettings::SpatialNavigationEnabled, b);
}

void BrowserWindow::toggleFullScreenMode(bool enable)
{
    bool alreadyEnabled = windowState() & Qt::WindowFullScreen;
    if (enable ^ alreadyEnabled)
        setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void BrowserWindow::toggleFrameFlattening(bool toggle)
{
    m_windowOptions.useFrameFlattening = toggle;
    page()->settings()->setAttribute(QWebSettings::FrameFlatteningEnabled, toggle);
}

void BrowserWindow::toggleInterruptingJavaScriptEnabled(bool enable)
{
    page()->setInterruptingJavaScriptEnabled(enable);
}

void BrowserWindow::toggleJavascriptCanOpenWindows(bool enable)
{
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, enable);
}

void BrowserWindow::setUseDiskCookies(bool enable)
{
    testBrowserCookieJarInstance()->setDiskStorageEnabled(enable);
}

void BrowserWindow::clearCookies()
{
    testBrowserCookieJarInstance()->reset();
}

void BrowserWindow::toggleAutoLoadImages(bool enable)
{
    page()->settings()->setAttribute(QWebSettings::AutoLoadImages, !enable);
}

void BrowserWindow::togglePlugins(bool enable)
{
    page()->settings()->setAttribute(QWebSettings::PluginsEnabled, !enable);
}

void BrowserWindow::changeViewportUpdateMode(int mode)
{
    m_windowOptions.viewportUpdateMode = QGraphicsView::ViewportUpdateMode(mode);

    if (!isGraphicsBased())
        return;

    WebViewGraphicsBased* view = static_cast<WebViewGraphicsBased*>(m_view);
    view->setViewportUpdateMode(m_windowOptions.viewportUpdateMode);
}

void BrowserWindow::showFPS(bool enable)
{
    if (!isGraphicsBased())
        return;

    m_windowOptions.showFrameRate = enable;
    WebViewGraphicsBased* view = static_cast<WebViewGraphicsBased*>(m_view);
    view->setFrameRateMeasurementEnabled(enable);
}

void BrowserWindow::toggleLocalStorage(bool toggle)
{
    m_windowOptions.useLocalStorage = toggle;
    page()->settings()->setAttribute(QWebSettings::LocalStorageEnabled, toggle);
}

void BrowserWindow::toggleOfflineStorageDatabase(bool toggle)
{
    m_windowOptions.useOfflineStorageDatabase = toggle;
    page()->settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, toggle);
}

void BrowserWindow::toggleOfflineWebApplicationCache(bool toggle)
{
    m_windowOptions.useOfflineWebApplicationCache = toggle;
    page()->settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, toggle);
}

void BrowserWindow::setOfflineStorageDefaultQuota()
{
    // For command line execution, quota size is taken from command line.   
    if (m_windowOptions.offlineStorageDefaultQuotaSize)
        page()->settings()->setOfflineStorageDefaultQuota(m_windowOptions.offlineStorageDefaultQuotaSize);
}

void BrowserWindow::toggleScrollAnimator(bool toggle)
{
    m_windowOptions.enableScrollAnimator = toggle;
    page()->settings()->setAttribute(QWebSettings::ScrollAnimatorEnabled, toggle);
}

void BrowserWindow::updateFPS(int fps)
{
    QString fpsStatusText = QString("Current FPS: %1").arg(fps);
    qDebug("%s", qPrintable(fpsStatusText));
}

void BrowserWindow::printSettings()
{
    qDebug("-graphicsbased       : WebPage Graphics based - %s", isGraphicsBased() ? "enabled" : "disabled");
    qDebug("-tiled-backing-store : Tiled backing store - %s", page()->settings()->testAttribute(QWebSettings::TiledBackingStoreEnabled) ? "enabled" : "disabled");
}

bool BrowserWindow::event(QEvent *event)
{
    int sockfd;
    struct sockaddr_un servaddr;
    int data[1024];
    int ret;

    if (/*event->type() == QEvent::KeyPress ||*/ event->type() == QEvent::ShortcutOverride) {

        QKeyEvent *ke = static_cast<QKeyEvent *>(event);

	switch (ke->key()){
	        case  Qt::Key_F4 :
                        if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
                        {
                                //printf("socket_error\n");
                                break;
                        }
                        memset(&servaddr,0, sizeof(servaddr));
                        servaddr.sun_family = AF_LOCAL;
                        strcpy(servaddr.sun_path, "/tmp/.browser.support");

                        if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                        {
                                //printf("connect_error\n");
                                ::close(sockfd);
                                break;
                        }
                        //printf("process CONTROL_STOP\n");
                        memset(data,0,16);
                        data[0] = 2; /*CONTROL_EXIT*/
                        ret = write(sockfd,data,16);
                        /*printf ("write len %d\n", ret);*/
                        ret = read(sockfd,data,1024);
                        /*printf ("read len %d\n", ret);
                        printf("rcv %x %x %x %x\n", data[0],data[1],data[2],data[3]);*/
                        ::close(sockfd);

			sleep(1);

			if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
                        {
                                //printf("socket_error\n");
                                break;
                        }
                        memset(&servaddr,0, sizeof(servaddr));
                        servaddr.sun_family = AF_LOCAL;
                        strcpy(servaddr.sun_path, "/tmp/.browser.support");

                        if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                        {
                                //printf("connect_error\n");
                                ::close(sockfd);
                                break;
                        }
                        //printf("process _CB_OOIF_BROADCAST_PLAY\n");
                        memset(data,0,16);
                        data[0] = 24; /*CONTROL_EXIT*/
                        ret = write(sockfd,data,16);
                        /*printf ("write len %d\n", ret);*/
                        ret = read(sockfd,data,1024);
                        /*printf ("read len %d\n", ret);
                        printf("rcv %x %x %x %x\n", data[0],data[1],data[2],data[3]);*/
			::close(sockfd);

			sleep(1);

                        if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
                        {
                                //printf("socket_error\n");
                                break;
                        }
                        memset(&servaddr,0, sizeof(servaddr));
                        servaddr.sun_family = AF_LOCAL;
                        strcpy(servaddr.sun_path, "/tmp/.browser.support");

                        if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                        {
                                //printf("connect_error\n");
                                ::close(sockfd);
                                break;
                        }
                        //printf("process exit\n");
                        memset(data,0,16);
                        data[0] = 5; /*CONTROL_EXIT*/
                        ret = write(sockfd,data,16);
                        /*printf ("write len %d\n", ret);*/
                        ret = read(sockfd,data,1024);

			::close(sockfd);

			break;
        	case  Qt::Key_VolumeUp :
        	case  Qt::Key_VolumeDown :
        	case  Qt::Key_VolumeMute : 
		{
			if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
			{
   				//printf("socket_error\n");
				break;
			}
	                memset(&servaddr,0, sizeof(servaddr));
                	servaddr.sun_family = AF_LOCAL;
                	strcpy(servaddr.sun_path, "/tmp/.browser.support");

                	if (::connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
                	{
                        	//printf("connect_error\n");
				::close(sockfd);
				break;
                	}
#if 0
			if (ke->key()==Qt::Key_F4)
        		{
 		           // special tab handling here
                		printf("process exit\n");
                		memset(data,0,16);
                		data[0] = 5; /*CONTROL_EXIT*/
                		ret = write(sockfd,data,16);
                		/*printf ("write len %d\n", ret);*/
                		ret = read(sockfd,data,1024);
                		/*printf ("read len %d\n", ret);
                		printf("rcv %x %x %x %x\n", data[0],data[1],data[2],data[3]);*/
                		/*exit(0);*/
      			 }
			else 
#endif
			if (ke->key()==Qt::Key_VolumeUp)
        		{
                		//printf("process volume up\n");
                		memset(data,0,16);
                		data[0] = 13; /*CONTROL_VOLUME_UP*/
                		ret = write(sockfd,data,16);
                		/*printf ("write len %d\n", ret);*/
                		ret = read(sockfd,data,1024);
                		/*printf ("read len %d\n", ret);
                		printf("rcv %x %x %x %x\n", data[0],data[1],data[2],data[3]);*/
        		}
			else if (ke->key()==Qt::Key_VolumeDown)
        		{
                		//printf("process volume down\n");
                		memset(data,0,16);
                		data[0] = 14; /*CONTROL_VOLUME_DOWN*/
                		ret = write(sockfd,data,16);
                		/*printf ("write len %d\n", ret);*/
                		ret = read(sockfd,data,1024);
                		/*printf ("read len %d\n", ret);
                		printf("rcv %x %x %x %x\n", data[0],data[1],data[2],data[3]);*/
        		}
			else if (ke->key()==Qt::Key_VolumeMute)
       			{
                		/*data[0] = 5; CONTROL_EXIT*/
        		}
			::close(sockfd);
			break;
		}
        	case  Qt::Key_F5 :
		{
			QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, 403, Qt::NoModifier, 403, 403, 0, "");
		 	page()->setProperty("_q_useNativeVirtualKeyAsDOMKey", true);
			QApplication::postEvent(m_view, key_press);
			return true;	
		}
        	case  Qt::Key_F6 :
		{
			QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, 404, Qt::NoModifier, 404, 404, 0, "");
		 	page()->setProperty("_q_useNativeVirtualKeyAsDOMKey", true);
			QApplication::postEvent(m_view, key_press);
			return true;	
		}
        	case  Qt::Key_F7 :
		{
			QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, 405, Qt::NoModifier, 405, 405, 0, "");
		 	page()->setProperty("_q_useNativeVirtualKeyAsDOMKey", true);
			QApplication::postEvent(m_view, key_press);
			return true;	
		}
        	case  Qt::Key_F8 :
		{
			QKeyEvent *key_press = new QKeyEvent(QKeyEvent::KeyPress, 406, Qt::NoModifier, 406, 406, 0, "");
		 	page()->setProperty("_q_useNativeVirtualKeyAsDOMKey", true);
			QApplication::postEvent(m_view, key_press);
			return true;	
		}
	} /*switch*/
    }

    return QWidget::event(event);
}

void BrowserWindow::Title(const QString &title)
{
    m_windowTitle = title;
}

void BrowserWindow::URLChanged(const QUrl &url)
{
	QString path = url.url();
    //printf( "%s ############## URL:%s\n", __func__, qPrintable(path) );

    //printf( "%s: %llu bytes, %u scripts\n", __func__, page()->totalBytes(), jsFiles.size() );
    QLinkedListIterator<QString> it(jsFiles);
    while (it.hasNext()) {
    	page()->mainFrame()->evaluateJavaScript(it.next());
    }
}



