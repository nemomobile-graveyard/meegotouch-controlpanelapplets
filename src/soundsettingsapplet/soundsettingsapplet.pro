TEMPLATE = lib
include($$[QT_INSTALL_DATA]/mkspecs/dcpconfig.pri)
CONFIG +=           \
    plugin          \
    gui             \
    meegotouch      \
    debug           \
    link_pkgconfig  \
    duicontrolpanel \
    $$(NULL)

PKGCONFIG += meegotouch dbus-1 profile gconf-2.0 ContentManager gstreamer-0.10

LIBS += -lqttracker
INCLUDEPATH += /usr/include/qt4/dui \
    /usr/include/duicontrolpanel

QT += dbus
contains(cov, true) { 
    message("Coverage options enabled")
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}
MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS =                    \
    debug.h                  \
    drilldownitem.h          \
    trackerconnection.h      \
    alerttone.h              \
    alerttoneappletmaps.h    \
    alerttoneappletwidget.h  \
    alerttonebrowser.h       \
    alerttonedefaults.h      \
    alerttonedefaultsmodel.h \
    alerttonepreview.h       \
    alerttonetoplevel.h      \
    alerttonevolume.h        \
    alerttonewidget.h        \
    gconfstringcombo.h       \
    profileintcombo.h        \
    qgconfdirmanager.h       \
    qgconfvalue.h            \
    qprofilevalue.h          \
    qtrackedvariant.h        \
    soundsettingsapplet.h    \
    static.h                 \
    $$(NULL)

SOURCES =                      \
    debug.cpp                  \
    drilldownitem.cpp          \
    trackerconnection.cpp      \
    alerttone.cpp              \
    alerttoneappletmaps.cpp    \
    alerttoneappletwidget.cpp  \
    alerttonebrowser.cpp       \
    alerttonedefaults.cpp      \
    alerttonedefaultsmodel.cpp \
    alerttonepreview.cpp       \
    alerttonetoplevel.cpp      \
    alerttonevolume.cpp        \
    alerttonewidget.cpp        \
    gconfstringcombo.cpp       \
    profileintcombo.cpp        \
    qgconfdirmanager.cpp       \
    qgconfvalue.cpp            \
    qprofilevalue.cpp          \
    qtrackedvariant.cpp        \
    soundsettingsapplet.cpp    \
    $$(NULL)

DESTDIR = lib

css.files = soundsettingsapplet.css
css.path += $$(DEBIAN_DESTDIR)/$$DCP_CSS_DIR

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/$$DCP_DESKTOP_DIR

target.path += $$(DEBIAN_DESTDIR)/$$DCP_APPLET_DIR

message("The plugin will be installed to:       " $$target.path)
message("The desktop file will be installed to: " $$desktop.path)
message("The .css file will be installed to:    " $$css.path)

INSTALLS += \
    target  \
    desktop \
		css     \
		$$(NULL)
    
OTHER_FILES +=                  \
    soundsettingsapplet.desktop \
		soundsettingsapplet.css     \
		$$(NULL)
