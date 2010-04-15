TEMPLATE = lib
CONFIG += plugin \
    gui \
    meegotouch \
    silent \
    debug \
    duicontrolpanel

LIBS += -lduicontrolpanel 

INCLUDEPATH += /usr/include/duicontrolpanel 

QT += dbus
contains(cov, true) { 
    message("Coverage options enabled")
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}
MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS =                \
    ../debug.h           \
    leddbusinterface.h   \
    ledapplet.h          \
    ledwidget.h          \
    ledbrief.h 

SOURCES =                \
    ../debug.cpp         \
    leddbusinterface.cpp \
    ledapplet.cpp        \
    ledwidget.cpp        \
    ledbrief.cpp

css.files = ledapplet.css
DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/m/duicontrolpanel/style
message("The plugin will be installed to: " $$target.path)
message("CSS path will be: " $$css.path)
INSTALLS += target \
    css \
    desktop
    
OTHER_FILES += ledapplet.css \
    ledapplet.desktop

