include(../coverage.pri)
include(../check.pri)

SRC_PREFIX = ../../src/aboutapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX 

CONFIG += \
	gui \
	meegotouch \
	cellular-qt 

	#silent

QT += \
    testlib \
    dbus \
    gui

TEMPLATE = app

DEFINES += \
    UNIT_TEST \
    FUNCTIONAL_TEST	

TARGET = ft_aboutbusinesslogic
target.path = /usr/lib/systemui-applets-tests

HEADERS += \
    ../../src/debug.h \
    ft_aboutbusinesslogic.h \
    $$SRC_PREFIX/aboutbusinesslogic.h

SOURCES += \
    ../../src/debug.cpp \
    ft_aboutbusinesslogic.cpp \
    $$SRC_PREFIX/aboutbusinesslogic.cpp

INSTALLS += target
