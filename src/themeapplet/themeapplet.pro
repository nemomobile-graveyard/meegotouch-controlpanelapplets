TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouch \
          duicontrolpanel \
          silent

contains(cov, true) { 
    message("Coverage options enabled")
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    themedescriptor.h \
    themebusinesslogic.h \
    themeapplet.h \
    themelistmodel.h \
    themewidget.h \
    themebrief.h \
    themecellcreator.h \
    themedialog.h

SOURCES = \
    ../debug.cpp \
    themedescriptor.cpp \
    themebusinesslogic.cpp \
    themeapplet.cpp \
    themelistmodel.cpp \
    themewidget.cpp \
    themebrief.cpp \
    themecellcreator.cpp \
    themedialog.cpp

DESTDIR = lib
rfs.files += theme-rfs.sh
rfs.path += $$system(pkg-config --variable osso_rfs_scripts_dir clean-device)
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets
css.files = themeapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/duicontrolpanel/style

message("The plugin will be installed to: " $$target.path)
message("CSS path will be: " $$css.path)
INSTALLS += target \
            css \
	    rfs \
            desktop
