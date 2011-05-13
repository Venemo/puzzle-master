#-------------------------------------------------
#
# Project created by QtCreator 2010-12-01T19:24:52
#
#-------------------------------------------------

QT += core gui opengl

TARGET = puzzle-master
TEMPLATE = app

VERSION = 1.2.2
DEFINES += HAVE_OPENGL

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
DESTDIR = build

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    puzzleitem.cpp \
    jigsawpuzzleitem.cpp \
    settingsdialog.cpp \
    imagechooser.cpp \
    highscoresdialog.cpp \
    jigsawpuzzleboard.cpp \
    puzzleboard.cpp \
    newgamedialog.cpp

HEADERS += \
    mainwindow.h \
    puzzleitem.h \
    jigsawpuzzleitem.h \
    settingsdialog.h \
    imagechooser.h \
    highscoresdialog.h \
    jigsawpuzzleboard.h \
    puzzleboard.h \
    util.h \
    newgamedialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    imagechooser.ui \
    highscoresdialog.ui \
    newgamedialog.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translations/puzzle-master_hu_HU.ts

OTHER_FILES += \
    data/about.txt \
    data/puzzle-master.desktop \
    puzzle-master.rc

unix {
    INSTALLS += target iconfile desktopfile

    target.path = /usr/bin
    iconfile.path = /usr/share/pixmaps
    iconfile.files = data/puzzle-master.png
    desktopfile.path = /usr/share/applications
    desktopfile.files = data/puzzle-master.desktop
}
contains(QT_CONFIG, hildon) {
    # Maemo 4 Diablo

    QT -= opengl
    DEFINES -= HAVE_OPENGL
    DEFINES += MOBILE
    
    target.path = /opt/puzzle-master
    desktopfile.path = /usr/share/applications/hildon
}
maemo5 {
    # Maemo 5 Fremantle
    
    QT += maemo5
    DEFINES += MOBILE HAVE_QACCELEROMETER

    CONFIG += mobility link_pkgconfig
    MOBILITY += sensors
    PKGCONFIG += hildon-extras-1
    LIBS += -lglib-2.0 -lgobject-2.0 -lgmodule-2.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lpangocairo-1.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lfreetype -lfontconfig -lpangoft2-1.0 -lgtk-x11-2.0 -lhildon-1 -lhildon-extras-1
    
    target.path = /opt/puzzle-master
    desktopfile.path = /usr/share/applications/hildon
}
symbian {
    # Symbian
    QT -= opengl
    DEFINES += MOBILE HAVE_QACCELEROMETER
    DEFINES -= HAVE_OPENGL

    CONFIG += mobility
    MOBILITY += sensors

    ICON = data/puzzle-master-44x44.svg

    LIBS += -lcommondialogs -lplatformenv -leikcoctl -lavkon -lcone -leikcore

    TARGET.UID3 = 0xe5b4435f
    # TARGET.CAPABILITY +=
    TARGET.EPOCSTACKSIZE = 0x28000
    # Max. heap size is 20 MiB
    TARGET.EPOCHEAPSIZE = 0x020000 0x20971520
}
win32 {
    # Windows desktop
    RC_FILE = puzzle-master.rc
}
wince {
    # Windows Mobile
    QT -= opengl
    DEFINES -= HAVE_OPENGL
    DEFINES += MOBILE
    RC_FILE = puzzle-master.rc
}
