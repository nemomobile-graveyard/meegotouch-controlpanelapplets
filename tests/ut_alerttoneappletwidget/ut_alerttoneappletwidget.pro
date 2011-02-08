include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib \
    dbus

LIBS += -lqttracker

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_alerttoneappletwidget
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    duicontrolpanel

PKGCONFIG += dbus-1 profile gconf-2.0 ContentManager gstreamer-0.10

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

HEADERS += \
    ut_alerttoneappletwidget.h \
    $$SRC_PREFIX/../debug.h \
    $$SRC_PREFIX/drilldownitem.h \
    $$SRC_PREFIX/trackerconnection.h \
    $$SRC_PREFIX/alerttone.h \
    $$SRC_PREFIX/alerttoneappletwidget.h \
    $$SRC_PREFIX/alerttonewidget.h \
    $$SRC_PREFIX/alerttonevolume.h \
    $$SRC_PREFIX/alerttonetoplevel.h \
    $$SRC_PREFIX/alerttoneappletmaps.h \
    $$SRC_PREFIX/alerttonepreview.h \
    $$SRC_PREFIX/gconfstringcombo.h \
    $$SRC_PREFIX/profileintcombo.h \
    $$SRC_PREFIX/qgconfdirmanager.h \
    $$SRC_PREFIX/qgconfvalue.h \
    $$SRC_PREFIX/qprofilevalue.h \
    $$SRC_PREFIX/static.h \
    $$SRC_PREFIX/qtrackedvariant.h

SOURCES += \
    ut_alerttoneappletwidget.cpp \
    $$SRC_PREFIX/../debug.cpp \
    $$SRC_PREFIX/drilldownitem.cpp \
    $$SRC_PREFIX/trackerconnection.cpp \
    $$SRC_PREFIX/alerttone.cpp \
    $$SRC_PREFIX/alerttoneappletwidget.cpp \
    $$SRC_PREFIX/alerttonewidget.cpp \
    $$SRC_PREFIX/alerttonevolume.cpp \
    $$SRC_PREFIX/alerttonetoplevel.cpp \
    $$SRC_PREFIX/alerttoneappletmaps.cpp \
    $$SRC_PREFIX/alerttonepreview.cpp \
    $$SRC_PREFIX/gconfstringcombo.cpp \
    $$SRC_PREFIX/profileintcombo.cpp \
    $$SRC_PREFIX/qgconfdirmanager.cpp \
    $$SRC_PREFIX/qgconfvalue.cpp \
    $$SRC_PREFIX/qprofilevalue.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp


INSTALLS += target



