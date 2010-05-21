/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"
#include "wallpapercurrentdescriptor.h"
#include "wallpaperitrans.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QProcessEnvironment>
#include <QPainter>

#include <Tracker>

#include <MTheme>
#include <MGConfItem>

#define DEBUG
#include "../debug.h"

static const QString PortraitKey = 
    "/desktop/meego/background/portrait/picture_filename";
static const QString LandscapeKey = 
    "/desktop/meego/background/landscape/picture_filename";

static const QString wallpaperDir = ".wallpapers";
static const QString destopFileName = "wallpaper.desktop";
static const QString backupExtension = ".BAK";

static const QString defaultLandscapeImageFile = 
"/usr/share/themes/base/meegotouch/duihome/images/HomeWallpaperLandscape.png";
static const QString defaultLandscapeMimeType = "image/png";

static const QString defaultPortraitImageFile = 
"/usr/share/themes/base/meegotouch/duihome/images/HomeWallpaperPortrait.png";

WallpaperBusinessLogic::WallpaperBusinessLogic()
{
    WallpaperCurrentDescriptor *currentDesc;
    QString                     desktopFile = dirPath() + destopFileName;
    bool                        success;
    
    m_LandscapeGConfItem = new MGConfItem (LandscapeKey);
    m_PortraitGConfItem = new MGConfItem (PortraitKey);
    m_EditedImage = 0;

    currentDesc = WallpaperCurrentDescriptor::instance ();
    success = currentDesc->setFromDestopFile (desktopFile);
    if (!success) {
        SYS_DEBUG ("Loading of %s failed. Trying image files from GConf.",
                SYS_STR(desktopFile));
        success = currentDesc->setFromFilenames (
                m_LandscapeGConfItem->value().toString(),
                m_PortraitGConfItem->value().toString());
    }

    if (!success) {
        SYS_DEBUG ("Trying default images.");
        success = currentDesc->setFromFilenames (
                defaultLandscapeImageFile,
                defaultPortraitImageFile);
        if (success) {
            currentDesc->setMimeType (defaultLandscapeMimeType);
        }
    }
}

WallpaperBusinessLogic::~WallpaperBusinessLogic()
{
    delete m_LandscapeGConfItem;
    delete m_PortraitGConfItem;
}

/*!
 * FIXME: This method is deprecated, should use the 
 * WallpaperCurrentDescriptor::instance () instead.
 */
WallpaperDescriptor *
WallpaperBusinessLogic::Wallpaper (
        bool portrait)
{
    Q_UNUSED (portrait);
    SYS_WARNING ("Deprecated method.");
    return WallpaperCurrentDescriptor::instance ();
}

/*!
 * A high level method to set the current wallpaper.
 */
void
WallpaperBusinessLogic::setBackground (
        WallpaperITrans     *landscapeITrans,
        WallpaperITrans     *portraitITrans,
        WallpaperDescriptor *desc)
{
    bool success;

    SYS_DEBUG ("******** Saving background *********");
    if (desc == 0)
        desc = m_EditedImage;

    Q_ASSERT (landscapeITrans);
    Q_ASSERT (portraitITrans);
    Q_ASSERT (desc);

    success = ensureHasDirectory ();
    if (!success)
        return;

    createBackupFiles ();

    success = writeFiles (landscapeITrans, portraitITrans, desc);
    if (!success) {
        // FIXME: Should restore backup files here.
        return;
    }
   
    deleteBackupFiles ();

    /*
     *
     */
    WallpaperCurrentDescriptor *currentDesc;
    currentDesc = WallpaperCurrentDescriptor::instance ();
    currentDesc->setFromDestopFile (dirPath() + destopFileName);
    emit wallpaperChanged ();

    SYS_DEBUG ("****** End saving backgroun ********");
}

/*!
 * This function does not support image manipulatios, it is deprecated.
 * FIXME: To remove this function.
 */
void
WallpaperBusinessLogic::setBackground (
        WallpaperDescriptor *desc)
{
    if (desc == 0)
        desc = m_EditedImage;

    SYS_WARNING ("Deprecated method.");
#if 0
    Q_ASSERT (m_PortraitGConfItem != 0);
    Q_ASSERT (m_LandscapeGConfItem != 0);

    SYS_DEBUG ("*** filename = %s", SYS_STR(desc->filename()));
    m_PortraitGConfItem->set (desc->filename());
    m_LandscapeGConfItem->set (desc->filename());
#endif
}



/*!
 * Returns a list wallpaper descriptors with the available wallpapers. The
 * current wallpaper is the first element of the available wallpapers.
 *
 * FIXME: This function needs some polishing.
 */
QList<WallpaperDescriptor *>
WallpaperBusinessLogic::availableWallpapers () const
{
    QList<WallpaperDescriptor *> list;
    const QString query = 
"SELECT ?uri ?title ?mime ?height ?width WHERE { "
" ?item nie:url ?uri." 

" ?item nie:mimeType ?mime." 
" FILTER regex(?mime, \"image\")."

" ?item nfo:height ?height."
" ?item nfo:width ?width."
" FILTER ( ?height > \"300\" )."
" FILTER ( ?width  > \"300\" )."

" OPTIONAL { ?item nie:title ?title }."
"}"
;

    list << WallpaperCurrentDescriptor::instance ();

    QVector<QStringList> result = ::tracker()->rawSparqlQuery(query);

    SYS_DEBUG ("*** result.size() = %d", result.size());
    foreach (QStringList partlist, result) {
        WallpaperDescriptor *desc;

        #ifdef LOTDEBUG
        SYS_DEBUG ("*** url     = %s", SYS_STR(partlist[FieldUrl]));
        #endif

        desc = new WallpaperDescriptor;
        desc->setUrl (partlist[FieldUrl]);
        desc->setTitle (partlist[FieldTitle]);
        desc->setMimeType (partlist[FieldMime]);
        list << desc; 
    }

    return list;
}

/*!
 * While a wallpaper image is edited the WallpaperBusinessLogic holds a
 * descriptor on it. This function is used to set this wallpaper descriptor.
 */
void
WallpaperBusinessLogic::setEditedImage (
        WallpaperDescriptor  *desc)
{
    SYS_DEBUG ("*** desc = %s", SYS_STR(desc->filename()));
    m_EditedImage = desc;
}

/*!
 * Returns the wallpaper that is being edited.
 */
WallpaperDescriptor *
WallpaperBusinessLogic::editedImage ()
{
    SYS_DEBUG ("*** m_EditedImage = %s", SYS_STR(m_EditedImage->filename()));
    return m_EditedImage;
}

/*********************************************************************************
 * Low level file handling functions.
 */

/*!
 * Returns the directory path that is used to store the information about the
 * edited wallpaper. This is the data storing directory for the wallpaper
 * applet.
 */
QString
WallpaperBusinessLogic::dirPath () const
{
    QString homeDir (getenv("HOME"));
    QString dirPath = homeDir + "/" + wallpaperDir + "/";

    return dirPath;
}

/*!
 * \returns true if the directory exists or could be created
 *
 * If the data store directory does not exists this method will create it.
 */
bool
WallpaperBusinessLogic::ensureHasDirectory ()
{
    QString path = dirPath();
    QDir    dir (path);

    if (dir.exists()) {
        SYS_DEBUG ("Directory %s already exists.", SYS_STR(path));
        return true;
    }

    if (!dir.mkpath(path)) {
        SYS_WARNING ("Unable to create %s directory.", SYS_STR(path));
        return false;
    }

    return true;
}

/*!
 * Takes the desktop file, the saved landscape image file and the saved portrait
 * file and moves/renames them to create a backup version of each of them.
 */
void
WallpaperBusinessLogic::createBackupFiles ()
{
    QString  path = dirPath();
    QString  desktopPath = path + destopFileName;
    QString  filename;
    
    makeBackup (desktopPath);

    filename = WallpaperCurrentDescriptor::instance()->editedFilename (
            M::Portrait);
    if (!filename.isEmpty())
        makeBackup (filename);

    filename = WallpaperCurrentDescriptor::instance()->editedFilename (
            M::Landscape);
    if (!filename.isEmpty())
        makeBackup (filename);
}

/*!
 * Removes all the files from the applet's data directory that has the file name
 * extension indicating that it is a backup file.
 */
void
WallpaperBusinessLogic::deleteBackupFiles ()
{
    QString     path = dirPath();
    QDir        dir (path);
    QStringList nameFilters ("*.BAK");

    dir.setNameFilters (nameFilters);
    foreach (QString backupFileName, dir.entryList (QDir::Files)) {
        SYS_DEBUG ("Removing backup file %s", SYS_STR(backupFileName));
        QFile backupFile (path + backupFileName);

        if (!backupFile.remove()) {
            SYS_WARNING ("Unable to remove %s backup file.",
                    SYS_STR((path + backupFileName)));
        }
    }
}

/*!
 * \returns true if the files could be created and saved.
 *
 * FIXME: This method should and will be moved to the WallpaperCurrentDescriptor
 * class.
 */
bool
WallpaperBusinessLogic::writeFiles (
        WallpaperITrans     *landscapeITrans,
        WallpaperITrans     *portraitITrans,
        WallpaperDescriptor *desc)
{
    Q_ASSERT (landscapeITrans);
    Q_ASSERT (portraitITrans);
    Q_ASSERT (desc);
    /*
     * These are pretty constants.
     */
    QString  path = dirPath();
    QString  desktopPath = path + destopFileName;
    QFile    file (desktopPath);
    
    WallpaperCurrentDescriptor *currentDesc = 
        WallpaperCurrentDescriptor::instance();

    // There is of course a reason why we use the version number from the
    // current descriptor: otherwise the re-editing of the original filename
    // might end up with the same version number.
    int      version = currentDesc->version () + 1;
    QString  versionString = QString::number(version);
    QString  portraitFilePath = path + 
        desc->suggestedOutputFilename (M::Portrait, version);
    QString  landscapeFilePath = path + 
        desc->suggestedOutputFilename (M::Landscape, version);


    /*
     * Opening the output file.
     */
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        SYS_DEBUG ("Opening file %s for writing failed.", SYS_STR(desktopPath));
        return false;
    }

    /*
     * FIXME: This is the new stuff, but in order to use it the current
     * wallpaper descriptor has to be upgraded with from the arguments of this
     * method.
     */
    QTextStream out(&file);

    QString outString = currentDesc->generateDesktopFile (path);
    SYS_DEBUG ("outString = %s", SYS_STR(outString));

    /*
     * Then this is deprecated.
     */
    out << "[Desktop Entry]\n";
    out << "Type=WallpaperImage\n";
    out << "Name=" << desc->title() << "\n";
    out << "Version=" << QString::number(version) << "\n";
    out << "\n";

    out << "[DCP Landscape Wallpaper]\n";
    out << "OriginalFile=" << desc->originalImageFile(M::Landscape) << "\n";
    out << "EditedFile=" << landscapeFilePath << "\n";
    out << "MimeType=" << desc->mimeType() << "\n";
    out << "HorOffset=" << landscapeITrans->x() << "\n";
    out << "VertOffset=" << landscapeITrans->y() << "\n";
    out << "Scale=" << landscapeITrans->scale() << "\n";
    out << "\n";

    out << "[DCP Portrait Wallpaper]\n";
    out << "OriginalFile=" << desc->originalImageFile(M::Portrait) << "\n";
    out << "EditedFile=" << portraitFilePath << "\n";
    out << "MimeType=" << desc->mimeType() << "\n";
    out << "HorOffset=" << portraitITrans->x() << "\n";
    out << "VertOffset=" << portraitITrans->y() << "\n";
    out << "Scale=" << portraitITrans->scale() << "\n";
    out << "\n";

    makeImageFile (portraitFilePath, desc, portraitITrans);
    makeImageFile (landscapeFilePath, desc, landscapeITrans);
    
    SYS_DEBUG ("saving landscape = %s", SYS_STR(landscapeFilePath));
    SYS_DEBUG ("saving portrait  = %s", SYS_STR(portraitFilePath));
    m_PortraitGConfItem->set (portraitFilePath);
    m_LandscapeGConfItem->set (landscapeFilePath);

    return true;
}

/*!
 * \param filePath The path of the file to save the image into.
 * \param desc The image that should be saved.
 * \param transformations The structure that descibes how to modify the image.
 *
 * This is a low level image manupilation method that takes a wallpaper and
 * saves it into a file with the given manupilations.
 * 
 * FIXME: Maybe this method should be moved into some other class?
 */
void
WallpaperBusinessLogic::makeImageFile (
            const QString        &filePath,
            WallpaperDescriptor  *desc,
            WallpaperITrans      *transformations)
{
    QPixmap   pixmap (transformations->expectedSize());
    QPainter  painter (&pixmap);
    qreal     scale = transformations->scale();
    QPixmap   image;
    qreal     ratio, ratio1;

    SYS_DEBUG ("*** expectedsize = %dx%d", 
            transformations->expectedWidth(),
            transformations->expectedHeight());
    /*
     * And this is exactly why we should move this kind of stuff into the image
     * descriptor classes.
     */
    if (desc->isCurrent()) {
        WallpaperCurrentDescriptor *cdesc;
        
        cdesc = qobject_cast<WallpaperCurrentDescriptor*> (desc);
        image.load (cdesc->originalImageFile (transformations->orientation()));
    } else {
        image = desc->pixmap();
    }

    ratio = 
        (qreal) transformations->expectedHeight () / 
        (qreal) image.height();

    ratio1 = 
        (qreal) transformations->expectedWidth () / 
        (qreal) image.width();
    
    if (ratio1 > ratio)
        ratio = ratio1;

    /*
     * Let's fill the pixmap with black, so the area not covered by the original
     * pixmap is initialized.
     */
    pixmap.fill (QColor("black"));
    
    /*
     * Then we draw the scaled image with the appropriate offset.
     */
    painter.drawPixmap (
                transformations->x(), transformations->y(),
                (scale * image.width ()) * ratio,
                (scale * image.height ()) * ratio,
                image);

    SYS_DEBUG ("Saving file into %s", SYS_STR(filePath));
    pixmap.save (filePath);
}

/*!
 * Takes a full path file name, removes its backup file if there is one, renames
 * the file to create a backup file.
 */
void 
WallpaperBusinessLogic::makeBackup (
        const QString &filePath)
{
    QString  backupFilePath = filePath + backupExtension;
    QFile    file (filePath);
    QFile    backupFile (backupFilePath);

    if (!file.exists())
        return;
    
    if (backupFile.exists()) {
        if (!backupFile.remove()) {
            SYS_WARNING ("Unable to remove %s backup file.", 
                    SYS_STR(backupFilePath));
            return;
        }
    }

    SYS_DEBUG ("Moving %s -> %s", SYS_STR(filePath), SYS_STR(backupFilePath));
    file.rename (backupFilePath);
}

