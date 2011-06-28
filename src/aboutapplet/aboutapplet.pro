include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

SUBDIRS += translations

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          mobility \
          duicontrolpanel \
          link_pkgconfig

QT += dbus
MOBILITY += systeminfo

contains(DEFINES, HAVE_CONTENT_ACTION) {
    PKGCONFIG += contentaction-0.1
}

LICENSE_FILENAME = license.txt
LICENSE_DIR = /usr/share/duicontrolpanel-aboutapplet
LICENSE_PATH = $$LICENSE_DIR/$$LICENSE_FILENAME
DEFINES += LICENSE_PATH=\\\"$${LICENSE_PATH}\\\"

license.files += $$LICENSE_FILENAME
license.path = $$LICENSE_DIR
INSTALLS += license

MOC_DIR = .moc
OBJECTS_DIR = .objects

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

HEADERS =                   \
    ../debug.h              \
    aboutbusinesslogic.h    \
    aboutapplet.h           \
    aboutwidget.h

SOURCES =                   \
    ../debug.cpp            \
    aboutbusinesslogic.cpp  \
    aboutapplet.cpp         \
    aboutwidget.cpp

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.files = libaboutapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libaboutapplet/style

target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target  \
            desktop \
            css

