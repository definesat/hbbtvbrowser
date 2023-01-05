/*
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C) 2006 George Staikos <staikos@kde.org>
 * Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Simon Hausmann <hausmann@kde.org>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "webpage.h"
#include "browserwindow.h"

#include <QApplication>
#include <QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkProxy>

extern BrowserOptions browserOptions;

WebPage::WebPage(QObject* parent)
    : QWebPage(parent)
    , m_userAgent()
    , m_interruptingJavaScriptEnabled(false)
{
    applyProxy();

    connect(networkAccessManager(), SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            this, SLOT(authenticationRequired(QNetworkReply*, QAuthenticator*)));
    connect(this, SIGNAL(featurePermissionRequested(QWebFrame*, QWebPage::Feature)), this, SLOT(requestPermission(QWebFrame*, QWebPage::Feature)));
    connect(this, SIGNAL(featurePermissionRequestCanceled(QWebFrame*, QWebPage::Feature)), this, SLOT(featurePermissionRequestCanceled(QWebFrame*, QWebPage::Feature)));
}

void WebPage::applyProxy()
{
    QUrl proxyUrl(qgetenv("http_proxy"));

    if (proxyUrl.isValid() && !proxyUrl.host().isEmpty()) {
        int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
        networkAccessManager()->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort));
    }
}

bool WebPage::supportsExtension(QWebPage::Extension extension) const
{
    if (extension == QWebPage::ErrorPageExtension)
        return true;
    return false;
}

bool WebPage::extension(Extension extension, const ExtensionOption* option, ExtensionReturn* output)
{
    Q_UNUSED(extension);
    const QWebPage::ErrorPageExtensionOption* info = static_cast<const QWebPage::ErrorPageExtensionOption*>(option);
    QWebPage::ErrorPageExtensionReturn* errorPage = static_cast<QWebPage::ErrorPageExtensionReturn*>(output);

    errorPage->content = QString("<html><head><title>Failed loading page</title></head><body>%1</body></html>")
        .arg(info->errorString).toUtf8();

    return true;
}

bool WebPage::shouldInterruptJavaScript()
{
    if (!m_interruptingJavaScriptEnabled)
        return false;
    return QWebPage::shouldInterruptJavaScript();
}

void WebPage::authenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
    Q_UNUSED(reply);
    Q_UNUSED(authenticator);
	qDebug("WebPage::authenticationRequired");
}

void WebPage::requestPermission(QWebFrame* frame, QWebPage::Feature feature)
{
    setFeaturePermission(frame, feature, PermissionGrantedByUser);
}

void WebPage::featurePermissionRequestCanceled(QWebFrame*, QWebPage::Feature)
{
}

QString WebPage::userAgentForUrl(const QUrl& url) const
{
    if (!m_userAgent.isEmpty())
        return m_userAgent;
    return QWebPage::userAgentForUrl(url);
}

void WebPage::javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
{
    if (browserOptions.enableJavaScriptConsoleMessage) {
        QString logEntry = message +" on line:"+ QString::number(lineNumber) +" Source:"+ sourceID;
        qDebug()<<logEntry;
    }
}
