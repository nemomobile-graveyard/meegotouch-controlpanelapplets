include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
MOC_DIR = .moc
OBJECTS_DIR = .objects

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

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
    ../libmeegocontrol \
    /usr/include/thumbnailer 

HEADERS =                          \
    ../debug.h                     \
    wallpaperconfiguration.h       \
    wallpaperutils.h               \
    gridimagewidget.h              \
    wallpaperdescriptor_p.h        \
    wallpaperdescriptor.h          \
    wallpapermodel.h               \
    wallpaperlist.h                \
    wallpaperbusinesslogic.h       \
    wallpaperapplet.h              \
    wallpaperwidget.h              \
    wallpaperinfoheader.h          \
    wallpapereditorsheet.h         \
    wallpapereditorwidget.h        \
    wallpaperitrans.h

SOURCES =                          \
    ../debug.cpp                   \
    wallpaperconfiguration.cpp     \
    wallpaperutils.cpp             \
    gridimagewidget.cpp            \
    wallpaperdescriptor_p.cpp      \
    wallpaperdescriptor.cpp        \
    wallpaperlist.cpp              \
    wallpapermodel.cpp             \
    wallpaperbusinesslogic.cpp     \
    wallpaperapplet.cpp            \
    wallpaperwidget.cpp            \
    wallpaperinfoheader.cpp        \
    wallpapereditorsheet.cpp       \
    wallpapereditorwidget.cpp      \
    wallpaperitrans.cpp

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
