/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et sw=4 ts=4 sts=4: */
#include "ut_wallpaperdescriptor.h"

#include "wallpaperdescriptor.h"
#include "wallpapercurrentdescriptor.h"
#include "wallpaperitrans.h"

#include <QPixmap>
#include <QUrl>
#include <QString>

#include <MApplication>
#include <MGConfItem>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * SignalSink implementation.
 */
SignalSink::SignalSink ()
{
    reset ();
}

void 
SignalSink::reset ()
{
    m_ThumbnailLoaded = false;
}

void
SignalSink::thumbnailLoaded (
        WallpaperDescriptor *desc)
{
    Q_UNUSED (desc);
    SYS_DEBUG ("");

    m_ThumbnailLoaded = true;
}

/******************************************************************************
 * Ut_WallpaperDescriptor implementation. 
 */
void 
Ut_WallpaperDescriptor::init()
{
}

void 
Ut_WallpaperDescriptor::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./Ut_WallpaperDescriptor";

void 
Ut_WallpaperDescriptor::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Desc = 0;
}

void 
Ut_WallpaperDescriptor::cleanupTestCase()
{
    if (m_Desc)
        delete m_Desc;

    m_App->deleteLater ();
}

/*!
 * Checking if the newly created descriptor has the proper default values.
 */
void 
Ut_WallpaperDescriptor::testDefaults ()
{
    createDescriptor ();

    QVERIFY (m_Desc->m_Thumbnailer == NULL);
    QVERIFY (m_Desc->m_Filename.isEmpty());
    QVERIFY (m_Desc->m_Title.isEmpty());
    QVERIFY (m_Desc->m_MimeType.isEmpty());
    QVERIFY (m_Desc->m_HasThumbnail == false);
    QVERIFY (m_Desc->m_Cached == false);
    QVERIFY (m_Desc->isCurrent() == false);
    QVERIFY (m_Desc->version() == 0);

    dropDescriptor ();    
}

/*
 * Tests all but the default constructor. The default constructor is used all
 * over the place and tested also in testDefaults().
 */
void
Ut_WallpaperDescriptor::testConstructors ()
{
    /*
     * Testing the constructor that gets one filename.
     */
    WallpaperDescriptor desc1 ("/nodir/NoSuchFile.png");
    
    QVERIFY (desc1.filename() == "/nodir/NoSuchFile.png");
    QVERIFY (desc1.basename() == "NoSuchFile");
    QVERIFY (desc1.extension() == "png");
    QVERIFY (desc1.m_Url.toString() == "file:///nodir/NoSuchFile.png");

    /*
     * Testing the copy constructor.
     */
    WallpaperDescriptor desc2 (desc1);

    QVERIFY (desc2.filename() == "/nodir/NoSuchFile.png");
    QVERIFY (desc2.basename() == "NoSuchFile");
    QVERIFY (desc2.extension() == "png");
    QVERIFY (desc2.m_Url.toString() == "file:///nodir/NoSuchFile.png");
}

void
Ut_WallpaperDescriptor::testFilenames ()
{
    WallpaperDescriptor desc ("/nodir/NoSuchFile.png");
    QString original  = desc.originalImageFile(M::Landscape);
    QString portrait  = desc.suggestedOutputFilename(M::Portrait);
    QString landscape = desc.suggestedOutputFilename(M::Landscape);

    QVERIFY (original == "/nodir/NoSuchFile.png");
    QVERIFY (!portrait.isEmpty());
    QVERIFY (!landscape.isEmpty());
    QVERIFY (!portrait.contains("/"));
    QVERIFY (!landscape.contains("/"));
    
    // Now this is important!
    QVERIFY (landscape != portrait);
}


/*!
 * When the descriptor has an url and a mime type it should be able to initiate
 * the thumbnailing and when the thumbnailing is successfull the descriptor
 * should report by emitting the thumbnailLoaded() signal and should have the
 * proper thumbnail.
 *
 * In this case the thumbnail is created by the Thumbnailer stub so there is no
 * file activity of any kind.
 */
void
Ut_WallpaperDescriptor::testThumbnailing ()
{
    createDescriptor ();
    m_SignalSink.reset ();

    /*
     * Let's see if the desscriptor does initiate the thumbnailing when a proper
     * url and mimetype is set.
     */
    m_Desc->setUrl ("file:///NoSuchAFile.png");
    m_Desc->setMimeType ("image/png");
    m_Desc->initiateThumbnailer ();

    QVERIFY (m_Desc->m_Thumbnailer != 0);
    QVERIFY (m_Desc->m_Thumbnailer->m_RequestCame);

    /*
     * Ok, now we emulate the case when the thumbnail creation is finished.
     */
    m_Desc->m_Thumbnailer->sendThumbnail ();
    m_Desc->m_Thumbnailer->sendFinished ();

    QVERIFY (m_Desc->isThumbnailLoaded());
    QVERIFY (m_SignalSink.m_ThumbnailLoaded);
    QVERIFY (m_Desc->m_Thumbnailer == 0);

    dropDescriptor ();    
}

/*
 * Tests the thumbnailing using a local file name and the mime type.
 */
void
Ut_WallpaperDescriptor::testThumbnailingByFilename ()
{
    createDescriptor ();
    m_SignalSink.reset ();

    /*
     * Let's see if the desscriptor does initiate the thumbnailing when a proper
     * url and mimetype is set.
     */
    m_Desc->setFilename ("/nodirectory/NoSuchAFile.png");
    m_Desc->setMimeType ("image/png");
    m_Desc->initiateThumbnailer ();

    QVERIFY (m_Desc->m_Thumbnailer != 0);
    QVERIFY (m_Desc->m_Thumbnailer->m_RequestCame);

    /*
     * Ok, now we emulate the case when the thumbnail creation is finished.
     */
    m_Desc->m_Thumbnailer->sendThumbnail ();
    m_Desc->m_Thumbnailer->sendFinished ();

    QVERIFY (m_Desc->isThumbnailLoaded());
    QVERIFY (m_SignalSink.m_ThumbnailLoaded);
    QVERIFY (m_Desc->m_Thumbnailer == 0);

    dropDescriptor ();    
}

/*!
 * When the descriptor has no filename/url and/or mimetype the thumbnailing
 * should not be initiated.
 */
void
Ut_WallpaperDescriptor::testThumbnailingWithoutData ()
{
    createDescriptor ();
    m_SignalSink.reset ();

    /*
     * Let's see if the desscriptor does not initiate the thumbnailing when the
     * filename and the url are missing.
     */
    m_Desc->initiateThumbnailer ();

    QVERIFY (m_Desc->m_Thumbnailer == 0);
    QVERIFY (!m_Desc->isThumbnailLoaded());
    QVERIFY (!m_SignalSink.m_ThumbnailLoaded);

    dropDescriptor ();    
}

/*
 * When the thumbnailer fails the descriptor should destroy the thumbnailer and
 * should stay in a state where it has no thumbnail. Also the descriptor should
 * not emit a signal about the thumbnail load finished.
 */
void
Ut_WallpaperDescriptor::testThumbnailingFailure ()
{
    createDescriptor ();
    m_SignalSink.reset ();

    /*
     * Let's see if the desscriptor does initiate the thumbnailing when a proper
     * url and mimetype is set.
     */
    m_Desc->setUrl ("file:///NoSuchAFile.png");
    m_Desc->setMimeType ("image/png");
    m_Desc->initiateThumbnailer ();

    QVERIFY (m_Desc->m_Thumbnailer != 0);
    QVERIFY (m_Desc->m_Thumbnailer->m_RequestCame);

    /*
     * Ok, now we emulate the case when the thumbnail creation is finished.
     */
    m_Desc->m_Thumbnailer->sendError ();
    m_Desc->m_Thumbnailer->sendFinished ();

    QVERIFY (!m_Desc->isThumbnailLoaded());
    QVERIFY (!m_SignalSink.m_ThumbnailLoaded);
    QVERIFY (m_Desc->m_Thumbnailer == 0);

    dropDescriptor ();    
}

void
Ut_WallpaperDescriptor::testFromFileNames ()
{
    QString file1 ("nolandscapefile.png");
    QString file2 ("noportraitfile.png");
    WallpaperCurrentDescriptor *curr = WallpaperCurrentDescriptor::instance();

    curr->setFromFilenames (file1, file2);

    /*
     * When set from filenames all these properties should have the same value.
     * This is the situation when the current wallpaper is set from the gconf
     * items.
     */
    QString originalPortrait  = curr->originalImageFile (M::Portrait);
    QString originalLandscape = curr->originalImageFile (M::Landscape);
    QVERIFY (originalPortrait == file2);
    QVERIFY (originalLandscape == file1);

    QString suggestedPortrait = curr->suggestedOutputFilename (M::Portrait);
    QString suggestedLandscape = curr->suggestedOutputFilename (M::Landscape);
    QVERIFY (suggestedPortrait != suggestedLandscape);
    QVERIFY (!suggestedPortrait.contains("/"));
    QVERIFY (!suggestedLandscape.contains("/"));
    
    QString editedPortrait = curr->editedFilename (M::Portrait);
    QString editedLandscape = curr->editedFilename (M::Landscape);
    SYS_DEBUG ("*** = %s", SYS_STR(editedPortrait));
    SYS_DEBUG ("*** = %s", SYS_STR(editedLandscape));
    QVERIFY (editedPortrait == file2);
    QVERIFY (editedLandscape == file1);

    QString desktopString = curr->generateDesktopFile ("ThisIsThePath");
    SYS_DEBUG ("*** = %s", SYS_STR(desktopString));
    // FIXME: We obviously should do a much 

    /*
     * When set from image file names the image transformations should be all
     * default.
     */
    WallpaperITrans trans1 = curr->iTrans (M::Portrait);
    WallpaperITrans trans2 = curr->iTrans (M::Landscape);

    QVERIFY (trans1.scale() == 1.0);
    QVERIFY (trans1.x() == 0);
    QVERIFY (trans1.y() == 0);
    
    QVERIFY (trans2.scale() == 1.0);
    QVERIFY (trans2.x() == 0);
    QVERIFY (trans2.y() == 0);
}

/******************************************************************************
 * Low level test programs.
 */
void
Ut_WallpaperDescriptor::dropDescriptor ()
{
    if (m_Desc) {
        delete m_Desc;
        m_Desc = 0;
    }
}

void
Ut_WallpaperDescriptor::createDescriptor ()
{
    bool                 connectSuccess;
    
    if (m_Desc)
        delete m_Desc;

    m_Desc = new WallpaperDescriptor;
    /*
     * Checking if the signals are there and of course connecting to them.
     */
    connectSuccess = connect (
            m_Desc, SIGNAL (thumbnailLoaded (WallpaperDescriptor *)),
            &m_SignalSink, SLOT (thumbnailLoaded (WallpaperDescriptor *)));
    QVERIFY (connectSuccess);

    /*
     * Checking if the slots are there.
     */
    connectSuccess = connect (
            &m_SignalSink, SIGNAL(thumbnailReady(QUrl, QUrl, QPixmap, QString)),
            m_Desc, SLOT (thumbnailReady(QUrl, QUrl, QPixmap, QString)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            &m_SignalSink, SIGNAL(thumbnailError(QString, QUrl)),
            m_Desc, SLOT (thumbnailError(QString, QUrl)));
    QVERIFY (connectSuccess);

    connectSuccess = connect (
            &m_SignalSink, SIGNAL(thumbnailLoadingFinished(int)),
            m_Desc, SLOT (thumbnailLoadingFinished(int)));
    QVERIFY (connectSuccess);
}

QTEST_APPLESS_MAIN(Ut_WallpaperDescriptor)



