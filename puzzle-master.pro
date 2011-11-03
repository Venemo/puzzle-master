#-------------------------------------------------
#
# Project created by QtCreator 2010-12-01T19:24:52
#
#-------------------------------------------------

QT += core gui opengl

TARGET = puzzle-master
TEMPLATE = app

VERSION = 1.2.4
DEFINES += HAVE_OPENGL

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
maemo5 {
    # Maemo 5 Fremantle
    
    QT += maemo5
    DEFINES += MOBILE HAVE_QACCELEROMETER

    CONFIG += mobility
    MOBILITY += sensors

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
