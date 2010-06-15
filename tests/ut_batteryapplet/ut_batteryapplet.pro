include(../coverage.pri)
include(../check.pri)

SRC_PREFIX = ../../src/batteryapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib

TEMPLATE = app

DEFINES += \
	UNIT_TEST 

TARGET = ut_batteryapplet
target.path = /usr/lib/systemui-applets-tests

CONFIG += \
	gui \
	meegotouch \
    qmsystem \
	plugin \
	duicontrolpanel \
	silent 

HEADERS += \
    ../../src/debug.h \
    ut_batteryapplet.h \
    $$SRC_PREFIX/batteryapplet.h \
    $$SRC_PREFIX/batterybrief.h \
    $$SRC_PREFIX/batterybusinesslogic.h \
    $$SRC_PREFIX/batteryimage.h \
    $$SRC_PREFIX/batterywidget.h \
    $$SRC_PREFIX/dcpbattery.h \
    $$SRC_PREFIX/slidercontainer.h \
    $$SRC_PREFIX/timecontainer.h

SOURCES += \
    ../../src/debug.cpp \
    ut_batteryapplet.cpp \
    $$SRC_PREFIX/batteryapplet.cpp \
    $$SRC_PREFIX/batterybrief.cpp \
    $$SRC_PREFIX/batterybusinesslogic.cpp \
    $$SRC_PREFIX/batteryimage.cpp \
    $$SRC_PREFIX/batterywidget.cpp \
    $$SRC_PREFIX/slidercontainer.cpp \
    $$SRC_PREFIX/timecontainer.cpp

INSTALLS += target
