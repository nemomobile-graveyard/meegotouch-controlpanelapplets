TEMPLATE = lib
CONFIG += plugin \
    gui \
    dui \
    silent \
    debug

LIBS += -lduicontrolpanel 

INCLUDEPATH += /usr/include/qt4/dui \
    /usr/include/duicontrolpanel 

contains(cov, true) { 
    message("Coverage options enabled")
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    themebusinesslogic.h \
    themeapplet.h \
    themewidget.h \
    themebrief.h \
    themecontainer.h

SOURCES = \
    ../debug.cpp \
    themebusinesslogic.cpp \
    themeapplet.cpp \
    themewidget.cpp \
    themebrief.cpp \
    themecontainer.cpp

css.files = themeapplet.css
DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/dui/duicontrolpanel/style

message("The plugin will be installed to: " $$target.path)
message("CSS path will be: " $$css.path)
INSTALLS += target \
            css \
            desktop