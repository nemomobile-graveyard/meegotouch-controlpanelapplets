SRC_PREFIX = ../../src/wallpaperapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX

QT += \
    testlib \
    dbus 

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_wallpaperbusinesslogic
target.path = /usr/lib/systemui-applets-tests

CONFIG += \
	plugin \
	gui \
	meegotouch \
	duicontrolpanel \
	debug \
	silent 

HEADERS += \
    ../../src/debug.h \
    ut_wallpaperbusinesslogic.h \
    $$SRC_PREFIX/wallpaperapplet.h \
    $$SRC_PREFIX/wallpaperbrief.h \
    $$SRC_PREFIX/wallpaperbusinesslogic.h \
    $$SRC_PREFIX/wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpapereditorwidget.h \
    $$SRC_PREFIX/wallpaperlist.h \
    $$SRC_PREFIX/wallpapermodel.h \
    $$SRC_PREFIX/wallpaperwidget.h

SOURCES += \
    ../../src/debug.cpp \
    ut_wallpaperbusinesslogic.cpp \
    $$SRC_PREFIX/wallpaperapplet.cpp \
    $$SRC_PREFIX/wallpaperbrief.cpp \
    $$SRC_PREFIX/wallpaperbusinesslogic.cpp \
    $$SRC_PREFIX/wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpapereditorwidget.cpp \
    $$SRC_PREFIX/wallpaperlist.cpp \
    $$SRC_PREFIX/wallpapermodel.cpp \
    $$SRC_PREFIX/wallpaperwidget.cpp

INSTALLS += target