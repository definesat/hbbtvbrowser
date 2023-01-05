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

#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslSocket>

BrowserOptions browserOptions;

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

#include <fcntl.h>

void exit_main(void)
{

	int sockfd;
	struct sockaddr_un servaddr;
	int data[1024];
	int ret;

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
	//printf("process exit\n");
	memset(data,0,16);
	data[0] = 5; /*CONTROL_EXIT*/
	ret = write(sockfd,data,16);
	/*printf ("write len %d\n", ret);*/
	ret = read(sockfd,data,1024);
	
	::close(sockfd);


}

int main(int argc, char **argv)
{
	{
	    ///////////////////////////////////////////////////////////////////
	    //Default options
	    browserOptions.userAgentForUrl = "Mozilla/5.0 (Windows NT 6.1; rv:26.0) Gecko/20100101 Firefox/26.0";
        //browserOptions.defaultURL = "http://www.google.com/";
        browserOptions.defaultURL = "http://itv.mit-xperts.com/hbbtvtest/";

	    BrowserApp app(argc, argv);

	    QString url = app.url();

	    if (url.isEmpty())
	        url = browserOptions.defaultURL;

	    QList<QSslCertificate> cacertsDef = QSslSocket::defaultCaCertificates();
	    QList<QSslCertificate> cacertsSys = QSslSocket::systemCaCertificates();

	    qDebug("Default CA certs: %d", cacertsDef.count());
	    qDebug("System  CA certs: %d", cacertsSys.count());
	    if (!(cacertsDef.count() + cacertsSys.count()))
	        qWarning("Warning! CA certificates are not installed correctly! SSL will not work!");

	    BrowserWindow* window = new BrowserWindow(&browserOptions);

	    window->setStyleSheet("background:transparent");
	    window->setAttribute(Qt::WA_TranslucentBackground);

	    window->load(url);
	    window->show();

	    return app.exec();
	}
#if 0
	else
	{
		exit_main();
		return 0;
	}
#endif
}


