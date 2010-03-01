# SRC_PREFIX = ../../src
# STUB_PREFIX = ../stubs
# INCLUDEPATH += $$SRC_PREFIX \
#               $$STUB_PREFIX

QT += testlib
TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_template
target.path = /usr/lib/systemui-applets-tests
CONFIG += dui silent 

HEADERS += \
    ut_template.h \

SOURCES += \
    ut_template.cpp \

INSTALLS += target