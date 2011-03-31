include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
MOC_DIR = .moc
OBJECTS_DIR = .objects

QT += dbus

CONFIG += plugin \
          gui \
          meegotouchcore \
          quill \
          duicontrolpanel

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += link_pkgconfig
    PKGCONFIG += ContentManager
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

LIBS += \
    -lthumbnailer

INCLUDEPATH += \
    /usr/include/thumbnailer 

HEADERS = \
    ../debug.h \
    gridimagewidget.h \
    wallpaperdescriptor.h \
    wallpapercurrentdescriptor.h   \
    wallpapermodel.h               \
    wallpaperlist.h                \
    wallpaperbusinesslogic.h       \
    wallpaperapplet.h              \
    wallpaperwidget.h              \
    wallpaperinfoheader.h          \
    wallpapereditorsheet.h         \
    wallpapereditorwidget.h \
    wallpaperitrans.h \
    wallpaperbrief.h 

SOURCES = \
    ../debug.cpp \
    gridimagewidget.cpp \
    wallpaperdescriptor.cpp \
    wallpapercurrentdescriptor.cpp \
    wallpaperlist.cpp \
    wallpapermodel.cpp \
    wallpaperbusinesslogic.cpp     \
    wallpaperapplet.cpp            \
    wallpaperwidget.cpp            \
    wallpaperinfoheader.cpp        \
    wallpapereditorsheet.cpp       \
    wallpapereditorwidget.cpp      \
    wallpaperitrans.cpp \
    wallpaperbrief.cpp 

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

backup.files = wallpaper.conf
backup.path  = /usr/share/backup-framework/applications/

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libwallpaperapplet/style
css.files = libwallpaperapplet.css

INSTALLS +=     \
    target      \
    desktop     \
    backup      \
    css

