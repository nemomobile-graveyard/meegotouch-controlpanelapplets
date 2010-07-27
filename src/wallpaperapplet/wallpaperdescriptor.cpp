/* -*- Mode: C; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/* vim:set et ai sw=4 ts=4 sts=4: tw=80 cino="(0,W2s,i2s,t0,l1,:0" */

#include "wallpaperdescriptor.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QPainter>

#include <MTheme>

/*
 * Please note that printing non UTF-8 characters might break the tests.
 * Apparently the test engine can not tolerate the debug messages when there are
 * weird file names around.
 */
#define DEBUG
#define LOTDEBUG
#define WARNING
#include "../debug.h"

#ifdef DEBUG
#  define THUMBNAIL_BG_COLOR "Red"
#else
#  define THUMBNAIL_BG_COLOR "White"
#endif

const QString dir = "";

/******************************************************************************
 * Image implementation.
 */
Image::Image ()
{
    reset ();
}

Image::Image (
        const Image &orig) :
    QObject ()
{
    m_Filename = orig.m_Filename;
    m_MimeType = orig.m_MimeType;
    m_ImageID  = orig.m_ImageID;
    m_Title    = orig.m_Title;
    m_Cached   = orig.m_Cached;
    m_Url      = orig.m_Url;  
    m_ThumbnailPixmap = orig.m_ThumbnailPixmap;
    m_HasThumbnail = orig.m_HasThumbnail;
    m_Pixmap   = orig.m_Pixmap;
}

void 
Image::reset ()
{
    m_Filename = "";
    m_MimeType = "";
    m_ImageID  = "";
    m_Title    = "";
    m_Cached   = false;
    m_Url      = QUrl ();

    m_ThumbnailPixmap = QPixmap (100, 100);
    m_ThumbnailPixmap.fill (QColor(THUMBNAIL_BG_COLOR));
    m_HasThumbnail = false;

    m_Pixmap = QPixmap ();
}

Image &
Image::operator= (
        const Image &rhs)
{
    if (this != &rhs) {
        m_Filename = rhs.m_Filename;
        m_MimeType = rhs.m_MimeType;
        m_ImageID  = rhs.m_ImageID;
        m_Title    = rhs.m_Title;
        m_Cached   = rhs.m_Cached;
        m_Url      = rhs.m_Url;  
        m_ThumbnailPixmap = rhs.m_ThumbnailPixmap;
        m_HasThumbnail = rhs.m_HasThumbnail;
        m_Pixmap   = rhs.m_Pixmap;
    }

    return *this;
}

void
Image::setFilename (
        const QString &fileName)
{
    if (fileName == m_Filename)
        return;
    
    reset ();
    m_Filename     = fileName;
    m_Url          = QUrl::fromLocalFile (fileName);
}
    
void
Image::setMimeType (
        const QString &mimeType)
{
    m_MimeType = mimeType;
}

static const char *
mimetypes[][2] = 
{
    { "bmp",  "image/bmp"  },
    { "gif",  "image/gif"  },
    { "jpg",  "image/jpeg" },
    { "jpeg", "image/jpeg" },
    { "jpe",  "image/jpeg" },
    { "tif",  "image/tiff" },
    { "png",  "image/png"  },
    { NULL,   NULL}
};

QString
Image::mimeType () const
{
    QString retval = m_MimeType;

    /*
     * One desparate attempt to produce a mimetype for it is very important.
     * This mechanism will fire up only for the images that are not coming from
     * the Tracker, for the Tracker actually tells us the mimetype. And it does
     * not really matter, because if the thumbnail generation fails we do the
     * thumbnail ourselves.
     */
    if (retval.isEmpty() && !m_Filename.isEmpty()) {
        QString ext = extension();

        for (int n = 0; mimetypes[n][0] != NULL; ++n) {
            if (!QString::compare(ext, mimetypes[n][0], Qt::CaseInsensitive)) {
                SYS_DEBUG ("Guessing mimetype %s", SYS_STR(retval));
                retval = QString(mimetypes[n][1]);
            }
        }
    }

    return retval;
}

QString
Image::filename () const
{
    return m_Filename;
}

void
Image::setImageID (
        const QString &imageID)
{
    if (m_ImageID == imageID)
        return;

    reset ();
    m_ImageID = imageID;
}

QString
Image::imageID () const
{
    return m_ImageID;
}

void
Image::setUrl (
        const QString &urlString)
{
    QUrl     newUrl;

    newUrl.setEncodedUrl (urlString.toAscii());
    if (newUrl == m_Url)
        return;

    reset ();
    m_Filename = newUrl.path();
    m_Url = newUrl;
}

void
Image::setTitle (
        const QString &title)
{
    m_Title = title;
}

QString 
Image::title () const
{
    QString retval;

    retval = m_Title;
    // If we have no title set we try the imageID, if that is also empty we use
    // the basename of the image file.
    if (retval.isEmpty()) {
        if (!m_ImageID.isEmpty())
            retval = m_ImageID;
        else 
            retval = basename ();
    }

    SYS_DEBUG ("Returning %s", SYS_STR(retval));
    return retval;
}

QString
Image::basename () const 
{  
    QFileInfo fileInfo (m_Filename);

    return fileInfo.baseName();
}

QString
Image::extension () const 
{
    QFileInfo fileInfo (m_Filename);

    return fileInfo.suffix();
}

void 
Image::cache ()
{
    bool success;

    SYS_DEBUG ("Caching...");

    if (m_Cached)
        return;

    /*
     * If the wallpaper is set by an image ID we load the image using the
     * current theme.
     */
    if (!m_ImageID.isEmpty()) {
        QPixmap *pixmap = MTheme::instance()->pixmapCopy (m_ImageID);
        
        SYS_DEBUG ("Cached %dx%d image from theme.", 
                pixmap->width(),
                pixmap->height());
        m_Pixmap = *pixmap;
        delete pixmap;
        
        m_Cached = true;
        return;
    }

    /*
     * If the image is set by a filename, we load that file.
     */
    success = m_Pixmap.load (filename());
    if (!success) {
        QFile file (filename());
        
        SYS_WARNING ("Loading of %s has been failed: %m", SYS_STR(filename()));
        SYS_WARNING ("*** exists = %s", SYS_BOOL(file.exists()));
        
        m_Cached = false;
        return;
    }

    m_Cached = true;
}

void
Image::unCache ()
{
    if (!m_Cached)
        return;

    m_Pixmap = QPixmap();
    m_Cached = false;
}

QPixmap
Image::pixmap ()
{
    cache();
    return m_Pixmap;
}

QPixmap 
Image::scaled (QSize size)
{
    cache ();
    return m_Pixmap.scaled (size, Qt::KeepAspectRatioByExpanding);
}

bool
Image::setThumbnailPixmap (
        const QPixmap &pixmap)
{
    m_ThumbnailPixmap = pixmap;
    if (pixmap.height() >= 100 &&
            pixmap.width() >= 100)
        m_HasThumbnail = true;
    else
        m_HasThumbnail = false;

    return m_HasThumbnail;
}

bool
Image::thumbnail (
        bool force)
{
    QPixmap *pixmap;
    bool     retval = false;

    if (!m_ImageID.isEmpty()) {
        /*
         * If we have an image ID instead of a filename the thumbler will not
         * help us. In this case we shall create a thumbnail using the theme.
         */
        pixmap = MTheme::instance()->pixmapCopy(m_ImageID, QSize (100, 100));
        m_ThumbnailPixmap = *pixmap;
        m_HasThumbnail = true;
        delete pixmap;
        retval = true;
    } else if (force) {
        /*
         * And if the force was set that means we have to make our own thumbnail
         * most probably the thumbler failed.
         */
        cache ();
        if (m_Cached) {
            m_ThumbnailPixmap = m_Pixmap.scaled (100, 100);
            m_HasThumbnail = true;
            retval = true;
        }
    }

    SYS_DEBUG ("Returning %s", SYS_BOOL(retval));
    return retval;
}

/******************************************************************************
 * WallpaperDescriptor implementation.
 */
WallpaperDescriptor::WallpaperDescriptor(
        QObject *parent) : 
    QObject (parent),
    m_Images (NVariants)
{
}

WallpaperDescriptor::WallpaperDescriptor (
        const WallpaperDescriptor &orig) :
    QObject (),
    m_Images (NVariants)
{
    m_Images = orig.m_Images;
}

WallpaperDescriptor::WallpaperDescriptor(
        const QString &filename) : 
    QObject (),
    m_Images (NVariants)
{
    setFilename (filename);
}

WallpaperDescriptor::~WallpaperDescriptor()
{
}


void
WallpaperDescriptor::setImageID (
        const QString &imageID,
        ImageVariant   variant)
{
    m_Images[variant].setImageID (imageID);
}

QString
WallpaperDescriptor::imageID (
            ImageVariant   variant) const
{
    return m_Images[variant].imageID();
}

void
WallpaperDescriptor::setFilename (
        const QString &filename,
        ImageVariant   variant)
{
    SYS_DEBUG ("*** variant = %d", variant);
    m_Images[variant].setFilename (filename);
}

QString
WallpaperDescriptor::filename (
        ImageVariant   variant) const
{
    return m_Images[variant].filename();
}

void
WallpaperDescriptor::setTitle (
        const QString &title,
        ImageVariant   variant)
{
    m_Images[variant].setTitle (title);
}

QString
WallpaperDescriptor::title (
        ImageVariant   variant) const
{
    return m_Images[variant].title();
}

void
WallpaperDescriptor::setUrl (
        const QString &urlString,
        ImageVariant   variant)
{
    m_Images[variant].setUrl (urlString);
}

QString
WallpaperDescriptor::basename (
        ImageVariant   variant) const
{
    return m_Images[variant].basename();
}

/*!
 * \returns The filename extension of the filename. 
 *
 * The filename extension is used when the applet creates a new file to save the
 * edited image. The edited file will use the same extension used in the
 * original filename.
 */
QString
WallpaperDescriptor::extension ( 
        ImageVariant   variant) const
{
    return m_Images[variant].extension ();
}


QPixmap
WallpaperDescriptor::thumbnailPixmap() const
{
    return m_Images[WallpaperDescriptor::Landscape].thumbnailPixmap();
}

bool 
WallpaperDescriptor::isThumbnailLoaded (
        ImageVariant   variant) const
{
    return m_Images[variant].hasThumbnail ();
}

void 
WallpaperDescriptor::cache (
    ImageVariant   variant)
{
    m_Images[variant].cache ();
}

void
WallpaperDescriptor::unCache (
        ImageVariant   variant)
{
    m_Images[variant].unCache ();
}

/*!
 * Will cache the pixmap, then returns the loaded QPixmap.
 */
QPixmap
WallpaperDescriptor::pixmap (
        ImageVariant variant)
{
    return m_Images[variant].pixmap();
}

QPixmap 
WallpaperDescriptor::scaled (
        QSize  size,
        ImageVariant   variant)
{
    return m_Images[variant].scaled(size);
}

void
WallpaperDescriptor::setMimeType (
        const QString &mimeType,
        ImageVariant   variant)
{
    m_Images[variant].setMimeType (mimeType);
}

QString
WallpaperDescriptor::mimeType (
        ImageVariant   variant) const
{
    return m_Images[variant].mimeType ();
}

/*!
 * This function will initiate the thumbnail generation. The thumbnail will be
 * loaded into the pixmap that is returned by the thumbnailPixmap() function. I
 * guess the pixmap will notify the 
 */
void 
WallpaperDescriptor::initiateThumbnailer ()
{
    QList<QUrl>      urisList;
    QStringList      mimeList;

    /*
     * If the thumbnailer is already initiated we return.
     */
    if (m_Thumbnailer != 0) {
        return;
    }

    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].hasThumbnail())
            continue;

        if (m_Images[n].thumbnail())
            continue;

        if (m_Images[n].mimeType().isEmpty() ||
                m_Images[n].filename().isEmpty())
            continue;
    
        urisList << m_Images[n].url();
        mimeList << m_Images[n].mimeType();
    }

    if (urisList.size() == 0)
        return;

    m_Thumbnailer = new Thumbnailer;
    connect (m_Thumbnailer, SIGNAL(thumbnail(QUrl,QUrl,QPixmap,QString)),
            this, SLOT(thumbnailReady(QUrl,QUrl,QPixmap,QString)) );
    connect (m_Thumbnailer, SIGNAL(error(QString,QUrl)),
            this, SLOT(thumbnailError(QString,QUrl)) );
    connect (m_Thumbnailer, SIGNAL(finished(int)),
            this, SLOT(thumbnailLoadingFinished(int)));

    #ifdef USE_PAINTER
    m_Thumbnailer->request (urisList, mimeList);
    #else
    m_Thumbnailer->request (urisList, mimeList, true);
    #endif
}

/*!
 * This slot is called when the thumbnail has been generated.
 */
void 
WallpaperDescriptor::thumbnailReady (
            QUrl         fileUri, 
            QUrl         thumbnailUri, 
            QPixmap      pixmap, 
            QString      flavor)
{
    bool success;
    bool needEmitSignal = false;

    /*
     * FIXME: should store the thumbnail URL as well.
     * FIXME: maybe we should emit a signal for every variant...
     */
    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].url() == fileUri) {
            SYS_DEBUG ("Has thumbnail for %dth image", n);
            success = m_Images[n].setThumbnailPixmap (pixmap);
            if (success && n == 0)
                needEmitSignal = true;
        }
    }

    if (needEmitSignal)
        emit thumbnailLoaded (this);
}

/*!
 * This slot is called when the thumbler has been detected some error. We could
 * do something, but until we find some smart thing we can do we just print an
 * error message.
 */
void
WallpaperDescriptor::thumbnailError (
            QString      message,
            QUrl         fileUri)
{
    for (int n = Landscape; n < NVariants; ++n) {

        if (m_Images[n].url() == fileUri) {
            bool success;

            SYS_WARNING ("Failed thumbnailing for %dth image: %s", 
                    n, SYS_STR(message));
            success = m_Images[n].thumbnail (true);

            if (success && n == WallpaperDescriptor::Landscape)
                emit thumbnailLoaded (this);
        }
    }
}

/*!
 * Called when the generation of the thumbnail is finished. This is the point
 * where we can destroy the thumbler object.
 */
void 
WallpaperDescriptor::thumbnailLoadingFinished (
            int          left)
{
    Q_UNUSED (left);
    delete m_Thumbnailer;
}

/*!
 * \returns true if this is the current wallpaper.
 */
bool
WallpaperDescriptor::isCurrent () const
{
    return false;
}

/*!
 * Applications need to listen the wallpaper GConf items, so whenever we save a
 * file we need to modify the filename of the saved images. We could re-use the
 * filename, but then the GConf key would not be changed, so the applications
 * would not be able to recognize the change.
 * 
 * For this purpose we use a version number in the filenames. The
 * WallpaperDescriptor returns one default value for version number, the
 * WallpaperCurrentDescriptor on the other hand reads the version number from
 * the desktop file. 
 */
int
WallpaperDescriptor::version () const 
{
    return 0;
}

/*
 * Returns a suggested file name that is used when a modified image file is
 * saved.
 */
QString 
WallpaperDescriptor::suggestedOutputFilename (
        M::Orientation orientation,
        int            ver) const
{
    QString retval;
    QString outputExtension;

    if (ver < 0)
        ver = version ();

    /*
     * There are some extensions we can not support for writing.
     */
    outputExtension = extension();
    if (QString::compare(outputExtension, "gif", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "bpm", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "pgm", Qt::CaseInsensitive) == 0)
        outputExtension = "jpeg";

    switch (orientation) {
        case M::Landscape:
            retval = "-landscape.";
            break;

        case M::Portrait:
            retval = "-portrait.";
            break;
    }

    retval = basename() + retval + 
        QString::number(ver) + "." + outputExtension;

    return retval;
}

/*
 * Returns the original image file that was loaded, modified and saved into a
 * new file. For this class the original filename is the only filename.
 *
 * FIXME: What if the original file is deleted?
 */
QString 
WallpaperDescriptor::originalImageFile (
        M::Orientation orientation) const
{
    Q_UNUSED (orientation);

    return filename ();
}

/*
 * This function always return true, only the inherited
 * WallpaperCurrentDescriptor has tests in it.
 */
bool
WallpaperDescriptor::valid () const
{
    return true;
}
