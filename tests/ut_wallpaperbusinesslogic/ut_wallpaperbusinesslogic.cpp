/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et sw=4 ts=4 sts=4: */
#include "filesystemstub.h"
#include "ut_wallpaperbusinesslogic.h"

#include "wallpaperbusinesslogic.h"
#include "wallpapercurrentdescriptor.h"
#include "wallpaperitrans.h"


#include <QPixmap>

#include <MApplication>
#include <MGConfItem>

#define DEBUG
#include "../../src/debug.h"

static const QString PortraitKey = 
    "/desktop/meego/background/portrait/picture_filename";
static const QString LandscapeKey = 
    "/desktop/meego/background/landscape/picture_filename";

/******************************************************************************
 * Ut_WallpaperBusinessLogic implementation. 
 */
void 
Ut_WallpaperBusinessLogic::init()
{
}

void 
Ut_WallpaperBusinessLogic::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./Ut_WallpaperBusinessLogic";

void 
Ut_WallpaperBusinessLogic::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Api = new WallpaperBusinessLogic;

    QVERIFY (m_Api->m_EditedImage == 0);
}

void 
Ut_WallpaperBusinessLogic::cleanupTestCase()
{
    delete m_App;
    delete m_Api;
}

/*!
 * Checks if the GConfItem objects are created with the right keys.
 */
void
Ut_WallpaperBusinessLogic::testGConfItems ()
{
    QVERIFY (m_Api->m_LandscapeGConfItem != 0);
    QVERIFY (m_Api->m_LandscapeGConfItem->key() == LandscapeKey); 
            
    QVERIFY (m_Api->m_PortraitGConfItem != 0);
    QVERIFY (m_Api->m_PortraitGConfItem->key() == PortraitKey);
}

/*!
 * Checks if the dirpath the businesslogic uses as a target directory to save
 * fiels is actually an absolute path inside the /home directory. Please note
 * that this test will not actually create this directory, hence this test is in
 * the unit test set.
 */
void
Ut_WallpaperBusinessLogic::testDirPath ()
{
    QString path = m_Api->dirPath ();

    SYS_DEBUG ("*** dirPath() = %s", SYS_STR(path));
    QVERIFY (!path.isEmpty());
    QVERIFY (path.startsWith("/home") || path.startsWith("/root"));
    QVERIFY (path.endsWith("/"));
}

void
Ut_WallpaperBusinessLogic::testEditedImage ()
{
    WallpaperDescriptor desc;

    m_Api->setEditedImage (&desc);
    QVERIFY (m_Api->editedImage() == &desc);

    m_Api->setEditedImage (0);
    QVERIFY (m_Api->editedImage() == 0);
}

/*
 * The tracker is stubbed, so we can test the available wllpapers. It is only
 * the current wallpaper singleton, we have to do something with that too...
 */
void
Ut_WallpaperBusinessLogic::testAvailableWallpapers ()
{
    QList<WallpaperDescriptor *> availableWallpapers;
    int n;

    availableWallpapers = m_Api->availableWallpapers ();
    /*
     * There should be at least one available wallpaper, that is the current
     * wallpaper.
     */
    SYS_DEBUG ("We have %d available wallpapers.", availableWallpapers.size());
    QVERIFY (availableWallpapers.size() > 0);

    n = 0;
    foreach (WallpaperDescriptor *desc, availableWallpapers) {
        QString   filename, title, basename;
        QString   extension, mimetype;
        QString   suggestedp, suggestedl;
        QString   originalp, originall;

        filename = desc->filename ();
        title = desc->title ();
        basename = desc->basename ();
        extension = desc->extension ();
        mimetype = desc->mimeType ();
        suggestedp = desc->suggestedOutputFilename (M::Portrait);
        suggestedl = desc->suggestedOutputFilename (M::Landscape);
        originalp = desc->originalImageFile (M::Portrait);
        originall = desc->originalImageFile (M::Landscape);

        if (filename.isEmpty() ||
                title.isEmpty() ||
                basename.isEmpty() ||
                mimetype.isEmpty() ||
                suggestedp.isEmpty() ||
                suggestedl.isEmpty() ||
                originalp.isEmpty() ||
                originall.isEmpty()) {
            /*
             * These might prove usefull in the future, but obviously generate
             * too much output.
             */
            SYS_DEBUG ("*** available wallpaper #%3d ***", n);
            SYS_DEBUG ("*** filename   = %s", SYS_STR(filename));
            SYS_DEBUG ("*** title      = %s", SYS_STR(title));
            SYS_DEBUG ("*** basename   = %s", SYS_STR(basename));
            SYS_DEBUG ("*** mimetype   = %s", SYS_STR(mimetype));
            SYS_DEBUG ("*** extension  = %s", SYS_STR(extension));
            SYS_DEBUG ("*** suggestedp = %s", SYS_STR(suggestedp));
            SYS_DEBUG ("*** suggestedl = %s", SYS_STR(suggestedl));
            SYS_DEBUG ("*** originalp  = %s", SYS_STR(originalp));
            SYS_DEBUG ("*** originall  = %s", SYS_STR(originall));
        }

        QVERIFY (!filename.isEmpty());
        QVERIFY (!title.isEmpty());
        QVERIFY (!basename.isEmpty());
        QVERIFY (!mimetype.isEmpty());
        QVERIFY (!suggestedp.isEmpty());
        QVERIFY (!suggestedl.isEmpty());
        QVERIFY (!originalp.isEmpty());
        QVERIFY (!originall.isEmpty());

        ++n;
    }
}


/*!
 * Checks the low level WallpaperITrans class, its tag methods and overloaded
 * operators.
 */
void
Ut_WallpaperBusinessLogic::testITrans ()
{
    WallpaperITrans trans1, trans2;

    SYS_DEBUG ("Checking default values of WallpaperITrans...");
    QVERIFY (trans1.x() == 0);
    QVERIFY (trans1.y() == 0);
    QVERIFY (trans1.scale() == 1.0);
    QVERIFY (trans1.orientation() == M::Landscape);

    SYS_DEBUG ("Checking the set functions...");
    trans2.setOffset (QPointF(10, 11));
    trans2.setScale (2.0);
    trans2.setOrientation (M::Portrait);
    trans2.setExpectedSize (QSize(864, 480));

    QVERIFY (trans2.x() == 10);
    QVERIFY (trans2.y() == 11);
    QVERIFY (trans2.scale() == 2.0);
    QVERIFY (trans2.orientation() == M::Portrait);
    QVERIFY (trans2.expectedWidth() == 864);
    QVERIFY (trans2.expectedHeight() == 480);

    SYS_DEBUG ("Testing the copy constructor.");
    WallpaperITrans trans3 (trans2);
    QVERIFY (trans3.x() == 10);
    QVERIFY (trans3.y() == 11);
    QVERIFY (trans3.scale() == 2.0);
    QVERIFY (trans3.orientation() == M::Portrait);
    QVERIFY (trans3.expectedWidth() == 864);
    QVERIFY (trans3.expectedHeight() == 480);

    SYS_DEBUG ("testing operator=...");
    trans1 = trans2;
    QVERIFY (trans1.x() == 10);
    QVERIFY (trans1.y() == 11);
    QVERIFY (trans1.scale() == 2.0);
    QVERIFY (trans1.orientation() == M::Portrait);
    QVERIFY (trans1.expectedWidth() == 864);
    QVERIFY (trans1.expectedHeight() == 480);

    SYS_DEBUG ("Testing operator*...");
    SYS_DEBUG ("*** trans1.scale() = %d", trans2 * 2);
    QVERIFY (trans2 * 2 == 4);
}

void
Ut_WallpaperBusinessLogic::testCreateDirectory ()
{
    m_Api->ensureHasDirectory ();
    m_Api->ensureHasDirectory ();

    QString dirPath = m_Api->dirPath ();
    QDirStub dir (dirPath);
    QVERIFY (dir.exists());
}

void
Ut_WallpaperBusinessLogic::testBackupFiles ()
{
    QFileStub desktopFile(m_Api->dirPath() + "wallpaper.desktop");
    desktopFile.open (QIODevice::WriteOnly);

    QFileStub desktopFileBak (m_Api->dirPath() + "wallpaper.desktop.BAK");

    /*
     *
     */
    m_Api->createBackupFiles ();

    SYS_DEBUG ("*** desktopfile.exists = %s", 
            SYS_BOOL(desktopFile.exists()));
    SYS_DEBUG ("*** backupfile.exists  = %s", 
            SYS_BOOL(desktopFileBak.exists()));
    QVERIFY (!desktopFile.exists());
    QVERIFY (desktopFileBak.exists());

    m_Api->deleteBackupFiles ();
    QVERIFY (!desktopFileBak.exists());
}

/******************************************************************************
 * Private functions.
 */
QTEST_APPLESS_MAIN(Ut_WallpaperBusinessLogic)


