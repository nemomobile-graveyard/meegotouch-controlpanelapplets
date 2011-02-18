/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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

#include "wallpaperdescriptor.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>

#include <QPixmap>
#include <QPainter>
#include <QThread>

#include <MTheme>

/*
 * Please note that printing non UTF-8 characters might break the tests.
 * Apparently the test engine can not tolerate the debug messages when there are
 * weird file names around.
 */
//#define LOTDEBUG
#define DEBUG
#define WARNING
#include "../debug.h"

#ifdef DEBUG
#  define THUMBNAIL_BG_COLOR "Red"
#else
#  define THUMBNAIL_BG_COLOR "White"
#endif

static const QString dir = "";

/*
 * Good to know: In order to use this flavor under scratchbox one must install
 * the media-settings-harmattan package. Then restart the desktop...
 */
static const QString defaultFlavor = "grid";
static const int defaultThumbnailWidth = 172;
static const int defaultThumbnailHeight = 172;

/******************************************************************************
 * Image implementation.
 */
Image::Image () :
    QObject () ,
    m_ScaledImage (0)
{
    reset ();
}

Image::~Image ()
{
    SYS_DEBUG ("*** m_ScaledImage  = %p", m_ScaledImage);

    if (m_ScaledImage)
        delete m_ScaledImage;
}


Image::Image (
        const Image &orig) :
    QObject (), 
    m_ScaledImage (0)
{
    m_Filename = orig.m_Filename;
    m_MimeType = orig.m_MimeType;
    m_Image    = orig.m_Image;
    m_ImageID  = orig.m_ImageID;
    m_Title    = orig.m_Title;
    m_Cached   = orig.m_Cached;
    m_Url      = orig.m_Url;  
    m_ThumbnailPixmap = orig.m_ThumbnailPixmap;
    m_HasThumbnail = orig.m_HasThumbnail;
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

    m_ThumbnailPixmap = QPixmap (defaultThumbnailWidth, defaultThumbnailHeight);
    m_ThumbnailPixmap.fill (QColor(THUMBNAIL_BG_COLOR));
    m_HasThumbnail = false;

    if (m_ScaledImage) {
        delete m_ScaledImage;
        m_ScaledImage = 0;
    }
}

Image &
Image::operator= (
        const Image &rhs)
{
    if (this != &rhs) {
        m_Filename = rhs.m_Filename;
        m_MimeType = rhs.m_MimeType;
        m_Image    = rhs.m_Image;
        m_ImageID  = rhs.m_ImageID;
        m_Title    = rhs.m_Title;
        m_Cached   = rhs.m_Cached;
        m_Url      = rhs.m_Url;  
        m_ThumbnailPixmap = rhs.m_ThumbnailPixmap;
        m_HasThumbnail = rhs.m_HasThumbnail;
        m_ScaledImage = 0;
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

    //SYS_DEBUG ("Returning %s", SYS_STR(retval));
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

/*
 * Low level loading method that will not just load the image, but also will
 * read the exim metadata and rotate the image if necessary.
 */
bool 
Image::load (
        const QString &fileName)
{
    bool retval;
    #ifdef HAVE_QUILL_METADATA
    QString           rFilterName = "org.maemo.rotate";
    QuillMetadata     metadata (fileName);
    QVariant      orientation = metadata.entry(QuillMetadata::Tag_Orientation);
    int               side = orientation.toInt();
    QuillImageFilter *filter = 0;
    SYS_DEBUG ("Orientation: %s", SYS_STR(orientation.toString()));
    SYS_DEBUG ("side       : %d", side);
    #endif

    SYS_DEBUG ("Loading '%s'", SYS_STR(fileName));
    retval = m_Image.load(fileName);

    if (!retval)
        return retval;

#ifdef HAVE_QUILL_METADATA
    switch (side) {
        case 1:
            // Nothing to do
            break;

        case 6:
            filter = QuillImageFilterFactory::createImageFilter (rFilterName);
            filter->setOption (QuillImageFilter::Angle, QVariant(90));
            break;

        case 8:
            filter = QuillImageFilterFactory::createImageFilter (rFilterName);
            filter->setOption (QuillImageFilter::Angle, -90);
            break;
        
        case 3:
            filter = QuillImageFilterFactory::createImageFilter (rFilterName);
            filter->setOption (QuillImageFilter::Angle, 180);
            break;
    }

    if (filter) {
        m_Image = filter->apply (m_Image);
        delete filter;
    }
#endif

    return retval;
}

/*!
 * \param threadSafe Not in main thread, only thread safe operations are
 *   allowed.
 */
void 
Image::cache (
        bool threadSafe)
{
    if (m_Cached) {
        return;
    } else if (filename().isEmpty() && m_ImageID.isEmpty()) {
        return;
    }
    
    SYS_DEBUG ("Caching %p", this);
    SYS_DEBUG ("*** threadSafe = %s", SYS_BOOL(threadSafe));
    SYS_DEBUG ("*** m_Cached   = %s", SYS_BOOL(m_Cached));
    SYS_DEBUG ("*** filename() = '%s'", SYS_STR(filename()));
    SYS_DEBUG ("*** m_ImageID  = '%s'", SYS_STR(m_ImageID));
    
    /*
     * Secondary thread file loading.
     */
    if (!filename().isEmpty()) {
        SYS_DEBUG ("Doing a thread-safe loading.");
        SYS_DEBUG ("*** Calling WallPaperImage::load(%s) 0", 
                SYS_STR(filename()));
        if (!load(filename())) {
            SYS_WARNING ("Loading failed: %s", SYS_STR(filename()));
            m_Cached = false;
        } else {
            SYS_DEBUG ("Loaded %s size %dx%d", SYS_STR(filename()),
                        m_Image.width(), m_Image.height());
            m_Cached = true;
        }

        return;
    }

    /*
     * Main thread theme image loading.
     */
    if (!threadSafe && !m_ImageID.isEmpty()) {
        QPixmap       *pixmap;
        const QPixmap *cPixmap;
       
        /*
         * First we try the cached pixmap, then we force the loading. We do
         * this, because we had some problems with the theme damon when we used
         * the pixmapCopy() the second time: the application was blocked.
         */
        cPixmap = MTheme::instance()->pixmap (m_ImageID);
        if (cPixmap->width() > 1 && cPixmap->height()) {
            m_Image = cPixmap->toImage();
            m_Cached = true;
            MTheme::instance()->releasePixmap (cPixmap);
            return;
        } else {
            MTheme::instance()->releasePixmap (cPixmap);
        }


        pixmap = MTheme::instance()->pixmapCopy (m_ImageID);
        m_Image = pixmap->toImage();
        
        delete pixmap;
        m_Cached = true;
        return;
    }
}

void
Image::unCache ()
{
    SYS_DEBUG ("Uncaching...");
    if (!m_Cached)
        return;

    m_Cached = false;
    m_Image = WallPaperImage();

    if (m_ScaledImage) {
        delete m_ScaledImage;
        m_ScaledImage = 0;
    }
}

QImage &
Image::image ()
{
    cache ();
    return m_Image;
}

QImage 
Image::scaledImage (
        QSize size)
{
    SYS_DEBUG ("*** requested size = %dx%d", size.width(), size.height());

    /*
     * Let's check if we have some pre-scaling available. We kept the aspect
     * ratio, so most probably only one side is going to be equal.
     */
    SYS_DEBUG ("1");
    if (m_ScaledImage &&
            (m_ScaledImage->width() == size.width() ||
            m_ScaledImage->height() == size.height())) {
        SYS_DEBUG ("Pre-scale hit.");
        return *m_ScaledImage;
    }

    cache ();
    SYS_DEBUG ("*** have  size %dx%d", m_Image.width(), m_Image.height());
    SYS_DEBUG ("m_Image.scaled (size, Qt::KeepAspectRatioByExpanding);");
    return m_Image.scaled (size, Qt::KeepAspectRatioByExpanding);
}

void
Image::preScale (
        QSize size, 
        bool threadSafe)
{
    if (threadSafe) {
        if (!m_Cached) 
            cache (threadSafe);
        if (!m_Cached)
            return;

        if (m_ScaledImage)
            delete m_ScaledImage;

        SYS_DEBUG ("m_Image.scaled(size, Qt::KeepAspectRatioByExpanding)");
        m_ScaledImage = new WallPaperImage(
                m_Image.scaled(size, Qt::KeepAspectRatioByExpanding));
    }
}

bool
Image::setThumbnailPixmap (
        const QPixmap &pixmap)
{
    m_ThumbnailPixmap = pixmap;
    if (pixmap.height() >= 10 &&
            pixmap.width() >= 10)
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
        pixmap = MTheme::instance()->pixmapCopy(m_ImageID, 
                QSize (defaultThumbnailWidth, defaultThumbnailHeight));
        m_ThumbnailPixmap = *pixmap;
        m_HasThumbnail = true;
        delete pixmap;
        retval = true;
    } else if (force) {
        /*
         * And if the force was set that means we have to make our own thumbnail
         * most probably the thumbler failed.
         */
        // FIXME: Do we still need this?
        SYS_WARNING ("No thumbnail available!");
        cache ();
        if (m_Cached) {
            SYS_DEBUG ("m_Image.scaled (");
            m_ThumbnailPixmap = 
                QPixmap::fromImage (m_Image.scaled (
                    defaultThumbnailWidth, defaultThumbnailHeight));
            m_HasThumbnail = true;
            retval = true;
        } else {
            SYS_WARNING ("No image after caching?");
        }
    }

    return retval;
}

/******************************************************************************
 * WallpaperDescriptor implementation.
 */
WallpaperDescriptor::WallpaperDescriptor(
        QObject *parent) : 
    QObject (parent),
    m_Images (NVariants),
    m_Loading (false)
{
}

WallpaperDescriptor::WallpaperDescriptor (
        const WallpaperDescriptor &orig) :
    QObject (),
    m_Images (NVariants),
    m_Loading (false)
{
    m_Images = orig.m_Images;
}

WallpaperDescriptor::WallpaperDescriptor(
        const QString &filename) : 
    QObject (),
    m_Images (NVariants),
    m_Loading (false)
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

/*!
 * \param filename The full path filename for the image file
 * \param variant The purpose of the image file
 *
 * Sets the image file name for the given purpose in the descriptor.
 */
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

    SYS_DEBUG ("=>");
    for (int n = 0; n < m_Images.size(); ++n) {
        SYS_DEBUG ("*** n = %d", n);
        SYS_DEBUG ("*** thisfilename  = %s", SYS_STR(m_Images[n].filename()));
        SYS_DEBUG ("*** otherfilename = %s", SYS_STR(m_Images[variant].filename()));
        if (n == variant)
            continue;

        if (!m_Images[n].filename().isEmpty() &&
                    m_Images[n].filename() == m_Images[variant].filename()) {
            if (m_Images[variant].m_Cached) {
                m_Images[n].m_Image =  m_Images[variant].m_Image;
            }
        }
    }
}

void
WallpaperDescriptor::unCache (
        ImageVariant   variant)
{
    SYS_DEBUG ("Uncaching %d...", variant);
    m_Images[variant].unCache ();
}

void
WallpaperDescriptor::unCache ()
{
    for (int n = Landscape; n < NVariants; ++n) {
        m_Images[n].unCache ();
    }
}

bool
WallpaperDescriptor::loading () const
{
    return m_Loading;
}

void
WallpaperDescriptor::setLoading (
        bool loading)
{
    if (loading == m_Loading)
        return;

    SYS_DEBUG ("setting m_Loading to %s", SYS_BOOL(loading));
    m_Loading = loading;
    emit changed (this);
}

QImage & 
WallpaperDescriptor::image (
            ImageVariant   variant)
{
    return m_Images[variant].image();
}

/*!
 * Will cache the pixmap, then returns the loaded QPixmap.
 */
QImage 
WallpaperDescriptor::scaledImage (
        QSize          size,
        ImageVariant   variant)
{
    SYS_DEBUG ("------------------------------------------------");
    SYS_DEBUG ("*** variant = %d", variant);
    cache (variant);
    return m_Images[variant].scaledImage(size);
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

        if (m_Images[n].thumbnail()) {
            /*
             * FIXME: maybe we should emit signal for every variant?
             */
            if (n == WallpaperDescriptor::Landscape) {
                SYS_DEBUG ("emit thumbnailLoaded()");
                emit thumbnailLoaded (this);
                emit changed (this);
            }

            continue;
        }

        if (m_Images[n].mimeType().isEmpty() ||
                m_Images[n].filename().isEmpty())
            continue;
    
        urisList << m_Images[n].url();
        mimeList << m_Images[n].mimeType();
    }

    if (urisList.size() == 0)
        return;

    m_Thumbnailer = new Thumbnailer;
    #ifdef LOTDEBUG
    QStringList list = Thumbnailer::getFlavors();
    foreach (QString flavor, list) {
        SYS_DEBUG ("*** flavor = %s", SYS_STR(flavor));
    }
    #endif
    connect (m_Thumbnailer, SIGNAL(thumbnail(QUrl,QUrl,QPixmap,QString)),
            this, SLOT(thumbnailReady(QUrl,QUrl,QPixmap,QString)) );
    connect (m_Thumbnailer, SIGNAL(error(QString,QUrl)),
            this, SLOT(thumbnailError(QString,QUrl)) );
    connect (m_Thumbnailer, SIGNAL(finished(int)),
            this, SLOT(thumbnailLoadingFinished(int)));

    m_Thumbnailer->request (urisList, mimeList, true, defaultFlavor);
    //m_Thumbnailer->request (urisList, mimeList, true, "screen");
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

    Q_UNUSED (thumbnailUri);
    Q_UNUSED (flavor);

    SYS_DEBUG ("*** flavor = %s", SYS_STR(flavor));
    SYS_DEBUG ("*** size   = %dx%d", pixmap.width(), pixmap.height());
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

    if (needEmitSignal) {
        SYS_DEBUG ("emit thumbnailLoaded()");
        emit thumbnailLoaded (this);
        emit changed (this);
    }
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
    SYS_WARNING ("*** fileUri = %s", SYS_STR(fileUri.toString()));

    for (int n = Landscape; n < NVariants; ++n) {
        if (m_Images[n].url() == fileUri) {
            bool success;

            SYS_WARNING ("Failed thumbnailing for %dth image: %s", 
                    n, SYS_STR(message));
            success = m_Images[n].thumbnail (true);

            if (success && n == WallpaperDescriptor::Landscape) {
                emit thumbnailLoaded (this);
                emit changed (this);
            }
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

    SYS_DEBUG ("*** left = %d", left);
    if (!m_Thumbnailer.isNull() && left == 0)
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
 * \param orientation Selects which output file we are talking about.
 * \param ver The version number or -1 to use the objects version number.
 * Returns a suggested file name that is used when a modified image file is
 * saved.
 */

// Quick fix: it seems that the jpeg save has some bugs. See NB#182044 and
// NB##180067 for details.
#define ALWAYS_PNG

QString 
WallpaperDescriptor::suggestedOutputFilename (
        M::Orientation orientation,
        int            ver) const
{
    QString retval;
#ifndef ALWAYS_PNG
    QString outputExtension;
#endif
    QString myBasename;
    QString myExtension;

    if (ver < 0)
        ver = version ();


    switch (orientation) {
        case M::Landscape:
#ifndef ALWAYS_PNG
            outputExtension = extension(WallpaperDescriptor::OriginalLandscape);
            if (outputExtension.isEmpty())
                outputExtension = extension(WallpaperDescriptor::Landscape);
#endif

            myBasename = basename (WallpaperDescriptor::OriginalLandscape);
            if (myBasename.isEmpty())
                myBasename = basename (WallpaperDescriptor::Landscape);

            myExtension = "-landscape.";
            break;

        case M::Portrait:
#ifndef ALWAYS_PNG
            outputExtension = extension(WallpaperDescriptor::OriginalPortrait);
            if (outputExtension.isEmpty())
                outputExtension = extension(WallpaperDescriptor::Portrait);
#endif

            myBasename = basename (WallpaperDescriptor::OriginalPortrait);
            if (myBasename.isEmpty())
                myBasename = basename (WallpaperDescriptor::Portrait);

            myExtension = "-portrait.";
            break;
    }

    /*
     * There are some extensions we can not support for writing.
     */
#ifndef ALWAYS_PNG
    if (outputExtension.isEmpty() ||
        QString::compare(outputExtension, "gif", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "bpm", Qt::CaseInsensitive) == 0 ||
        QString::compare(outputExtension, "pgm", Qt::CaseInsensitive) == 0)
        outputExtension = "png";
#endif

    retval = myBasename + myExtension + 
        QString::number(ver) + "." + 
#ifndef ALWAYS_PNG
        outputExtension;
#else
        "png";
#endif

    return retval;
}

QString 
WallpaperDescriptor::suggestedOutputMimeType (
            M::Orientation orientation)
{
    QString retval;

    switch (orientation) {
        case M::Landscape:
            retval = mimeType(WallpaperDescriptor::OriginalLandscape);
            if (retval.isEmpty())
                retval = mimeType (WallpaperDescriptor::Landscape);
            break;

        case M::Portrait:
            retval = mimeType (WallpaperDescriptor::OriginalPortrait);
            if (retval.isEmpty())
                retval = mimeType (WallpaperDescriptor::Portrait);
            break;
    }

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
    QString retval;

    switch (orientation) {
        case M::Portrait:
            retval = filename (WallpaperDescriptor::OriginalPortrait);
            if (retval.isEmpty())
                retval = filename (WallpaperDescriptor::Portrait);

            return retval;

        case M::Landscape:
            retval = filename (WallpaperDescriptor::OriginalLandscape);
            if (retval.isEmpty())
                retval = filename (WallpaperDescriptor::Landscape);

            return retval;
    }

    SYS_WARNING ("Unknown orientation: %d", orientation);
    return filename (WallpaperDescriptor::OriginalLandscape);
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

/*!
 * This method might be called from the non-GUI thread. In that case the method
 * will load the image files as QImage objects, because the QImage can be
 * handled from the secondary threads while QPixmap not.
 */
void
WallpaperDescriptor::loadAll () 
{
    QSize landscapeSize (854, 480);
    QSize portraitSize (480, 854);
    bool  threadSafe = false;

    /*
     * Need to find out if we are in the GUI thread or in some secondary thread
     * created so that we can stay responsive while the images are loaded.
     */
    if (!qApp) {
        SYS_WARNING ("QApplication must be created before calling this method");
    } else {
        threadSafe = qApp->thread() != QThread::currentThread();
    }

    /*
     * We go through the images and load them: FIXME: how could we optimize this
     * and load only those images that are going to be used?
     */
    SYS_DEBUG ("secondary thread = %s", SYS_BOOL(threadSafe));
    for (int n = 0; n < m_Images.size(); ++n) {
        bool alreadyFound = false;
        SYS_DEBUG ("*************** %d ***", n);

        if (m_Images[n].m_Cached)
            continue;

        for (int i = 0; i < n; ++i) {
            if (!m_Images[n].filename().isEmpty() &&
                    m_Images[n].filename() == m_Images[i].filename()) {
                alreadyFound = true;
                SYS_DEBUG ("Not loading %d, it is the same as %d.", n, i);
                if (m_Images[i].m_Cached) {
                    m_Images[n].m_Image =  m_Images[i].m_Image;
                }
                break;
            }
        }
        
        if (alreadyFound) {
            continue;
        }

        m_Images[n].cache (threadSafe);
    }

    /*
     * Not just the loading but the scaling is also blocking the GUI thread.
     * Here we try to find out what size we are about to use and we do some
     * thread safe pre-scaling.
     */
    m_Images[WallpaperDescriptor::Landscape].preScale (
            landscapeSize, threadSafe);
    m_Images[WallpaperDescriptor::Portrait].preScale (
            portraitSize, threadSafe);

    m_Images[WallpaperDescriptor::OriginalLandscape].preScale (
            landscapeSize, threadSafe);
    m_Images[WallpaperDescriptor::OriginalPortrait].preScale (
            portraitSize, threadSafe);
}

