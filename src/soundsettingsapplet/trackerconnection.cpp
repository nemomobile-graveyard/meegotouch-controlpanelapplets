/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "trackerconnection.h"

#include <QTimer>

//#define DEBUG
#define WARNING
#include "../debug.h"

TrackerConnection *TrackerConnection::s_Instance = 0;

static const int delayBetweenRequests = 100;
static const int delayFirstRequests = 100;

#ifdef DEBUG
static int nRequests = 0;
static int nCacheHits = 0;
#endif

/*
 * This macro decides which are those files that we try to discover with
 * tracker. Currently tracker is processing only the user's home, whatever the
 * username is.
 */
#define TRACKER_HAS_A_CHANCE(string) (string).startsWith("/home")

TrackerConnection *
TrackerConnection::instance ()
{
    if (!TrackerConnection::s_Instance)
        TrackerConnection::s_Instance = 
            new TrackerConnection;

    return TrackerConnection::s_Instance;
}

TrackerConnection::TrackerConnection ()
{
}

TrackerConnection::~TrackerConnection ()
{
    s_Instance = 0;
}

QString
TrackerConnection::niceNameFromFileName (
        const QString &fileName)
{
    QString                           niceName;
   
    /**************************************************************************
     * First we try to find the filename in the cache. If we find the nice name
     * in the cache we are returning it and will not initiate a tracker request.
     */
    niceName = m_NiceNameCache[fileName];
    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    SYS_DEBUG ("*** niceName = %s", SYS_STR(niceName));
    
    #ifdef DEBUG
    ++nRequests;
    #endif

    SYS_DEBUG ("*********************************");
    SYS_DEBUG ("Cache hits/requests %d/%d (%.2g%%)", 
            nCacheHits, nRequests, 
            100.0 * (((qreal)nCacheHits / nRequests)));
    SYS_DEBUG ("*********************************");
    if (!niceName.isEmpty()) {
        #ifdef DEBUG
        nCacheHits++;
        #endif
        return niceName;
    }

    /**************************************************************************
     * Tracker is processing the user's home, it will not give information about
     * the default sound files. We are not bothering tracker about these files,
     * this will speed up the process considerably. 
     * NOTE: Please comment this code block out to test what happens when
     * tracker does not provide information about a particular file. In that
     * case we send a signal in the processRequest() method. It is not
     * necessary, but this way we can show a placeholder string while waiting
     * for the tracker answer.
     */
    if (!TRACKER_HAS_A_CHANCE(fileName)) {
        niceName = poorNiceName (fileName);
        m_NiceNameCache[fileName] = niceName;
        return niceName;
    }

    /**************************************************************************
     * So we have no information in the cache, we have a chance to get the
     * information from tracker. We add a pandingrequest info about this and we
     * return a temporary title. If necessary we fire up the request processing.
     *
     * NOTE: This value is a temporary value, we will send a signal even if the
     * tracker is not answering. We are of course not adding this value to the 
     * cache.
     */
    if (m_PendingRequests.indexOf(fileName) < 0) {
        m_PendingRequests << fileName;
        if (m_PendingRequests.size() == 1)
            QTimer::singleShot (
                    delayFirstRequests, this, SLOT(processPendingRequests()));
    }

    niceName = poorNiceName (fileName);

    SYS_DEBUG ("*** returning %s", SYS_STR(niceName));
	return niceName;
}

QString
TrackerConnection::trackerIdFromFileName (
        const QString &fileName)
{
    QString trackerId;

    trackerId = m_TrackerIdCache[fileName];
    if (!trackerId.isEmpty())
        return trackerId;

    /*
     * This call will initiate a tracker request and s signal about the
     * trackerId later.
     */
    niceNameFromFileName (fileName);
    return QString ("");
}

void
TrackerConnection::processPendingRequests()
{
    QString request;
    SYS_DEBUG ("We have %d requests.", m_PendingRequests.size());

    processRequest (m_PendingRequests.takeFirst());
    if (m_PendingRequests.size() > 0)
        QTimer::singleShot (
                delayBetweenRequests, this, SLOT(processPendingRequests()));
}

/*!
 * Low level function to process one request, sends a tracker query, checks the
 * return value and sends a signal about the result. Please note that this
 * method will emit a signal even if the tracker fails to return the requested
 * data. The returned data also added to the cache, so we call tracker at most
 * once for every file.
 */
void 
TrackerConnection::processRequest (
        const QString &fileName)
{
    const QString encodedFilename = QUrl::fromLocalFile(fileName).toEncoded();
    const QString query = 
        QString("select"
                " nie:title(?u) ?u"
                " where { ?u a nmm:MusicPiece . ?u nie:url \"") +
        encodedFilename + 
        "\"}";
    QVector<QStringList> result;
    QString              title;
    QString              trackerId;

    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    SYS_DEBUG ("*** encodedFilename = %s", SYS_STR(encodedFilename));
    SYS_DEBUG ("*** query    = %s", SYS_STR(query));

    #ifdef LOTDEBUG
    /*
     * XXX: Causes build error: deprecated
     *
     * ::tracker()->setConsoleVerbosity (5);
     */
    #endif
    result = ::tracker()->rawSparqlQuery (query);
    if (result.size() == 0) {
        SYS_WARNING ("Failed to get tracker info for %s", 
                SYS_STR(encodedFilename));
        goto fallback;
    }

    if (result[0].size() > 0) {
        title = result[0][0];
        m_NiceNameCache[fileName] = title;
    }

    if (result[0].size() > 1) {
        trackerId = result[0][1];
        m_TrackerIdCache[fileName] = trackerId;
    }

fallback:
    /*
     * This part is handling the case when the tracker information is not
     * available. In this case we are creating a nice name from the filename. We
     * also store this name in the cache.
     */
    if (title.isEmpty()) {
        title = poorNiceName (fileName);
        m_NiceNameCache[fileName] = title;
    }
    
    SYS_DEBUG ("*** title     = %s", SYS_STR(title));
    SYS_DEBUG ("*** trackerId = %s", SYS_STR(trackerId));
    SYS_DEBUG ("Emitting dataReady()");
    emit dataReady (fileName, title, trackerId);
}

/*!
 * Our own nice name generator. We return the basename of the file without the
 * extension. 
 */
QString
TrackerConnection::poorNiceName (
        const QString &fileName)
{
    QString niceName;

    niceName = fileName.split('/').last();
	niceName = niceName.left(niceName.lastIndexOf('.'));
    niceName.replace ("_", " ");
    
    return niceName;
}
