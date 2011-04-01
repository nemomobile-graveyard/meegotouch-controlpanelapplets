include(../common_top.pri)

SRC_PREFIX = ../../src/wallpaperapplet
MC_PREFIX = ../../src/libmeegocontrol
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX

QT += \
    testlib \
    dbus 

LIBS += \
    -lthumbnailer

INCLUDEPATH += \
    /usr/include/thumbnailer \
    $$MC_PREFIX

TEMPLATE = app

DEFINES += \
    UNIT_TEST \
    FUNCTIONAL_TEST

TARGET = ft_wallpaperbusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin \
    gui \
    quill \
    meegotouchcore \
    duicontrolpanel \
    link_pkgconfig

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}


PKGCONFIG += ContentManager


HEADERS += \
    ../../src/debug.h \
    ft_wallpaperbusinesslogic.h \
    $$SRC_PREFIX/gridimagewidget.h \
    $$SRC_PREFIX/wallpaperapplet.h \
    $$SRC_PREFIX/wallpaperbrief.h \
    $$SRC_PREFIX/wallpapergconf.h \
    $$SRC_PREFIX/wallpaperbusinesslogic.h \
    $$SRC_PREFIX/wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpapercurrentdescriptor.h \
    $$SRC_PREFIX/wallpaperinfoheader.h \
    $$SRC_PREFIX/wallpapereditorwidget.h \
    $$SRC_PREFIX/wallpapereditorsheet.h \
    $$SRC_PREFIX/wallpaperlist.h \
    $$SRC_PREFIX/wallpapermodel.h \
    $$SRC_PREFIX/wallpaperitrans.h \
    $$SRC_PREFIX/wallpaperwidget.h

SOURCES += \
    ../../src/debug.cpp \
    ft_wallpaperbusinesslogic.cpp \
    $$SRC_PREFIX/gridimagewidget.cpp \
    $$SRC_PREFIX/wallpaperapplet.cpp \
    $$SRC_PREFIX/wallpaperbrief.cpp \
    $$SRC_PREFIX/wallpaperbusinesslogic.cpp \
    $$SRC_PREFIX/wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpapercurrentdescriptor.cpp \
    $$SRC_PREFIX/wallpaperinfoheader.cpp \
    $$SRC_PREFIX/wallpapereditorwidget.cpp \
    $$SRC_PREFIX/wallpapereditorsheet.cpp \
    $$SRC_PREFIX/wallpaperlist.cpp \
    $$SRC_PREFIX/wallpapermodel.cpp \
    $$SRC_PREFIX/wallpaperitrans.cpp \
    $$SRC_PREFIX/wallpaperwidget.cpp

INSTALLS += target
