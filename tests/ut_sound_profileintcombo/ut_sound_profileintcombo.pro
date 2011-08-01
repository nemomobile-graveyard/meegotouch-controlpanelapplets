include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib \
    dbus

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_profileintcombo
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    duicontrolpanel

PKGCONFIG += dbus-1 profile gconf-2.0 ContentManager gstreamer-0.10

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS += \
    ut_profileintcombo.h \
    $$SRC_PREFIX/../debug.h \
    $$SRC_PREFIX/trackerconnection.h \
    $$SRC_PREFIX/alerttoneappletmaps.h \
    $$SRC_PREFIX/qprofilevalue.h \
    $$SRC_PREFIX/qtrackedvariant.h  \
    $$SRC_PREFIX/profileintcombo.h



SOURCES += \
    ut_profileintcombo.cpp  \
    $$SRC_PREFIX/../debug.cpp \
    $$SRC_PREFIX/trackerconnection.cpp \
    $$SRC_PREFIX/alerttoneappletmaps.cpp \
    $$SRC_PREFIX/qprofilevalue.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp  \
    $$SRC_PREFIX/profileintcombo.cpp

INSTALLS += target
