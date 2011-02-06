#-------------------------------------------------
#
# Project created by QtCreator 2010-12-01T19:24:52
#
#-------------------------------------------------

QT += core gui opengl

TARGET = puzzle-master
TEMPLATE = app

VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

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
    puzzleboard.cpp

HEADERS += \
    mainwindow.h \
    puzzleitem.h \
    jigsawpuzzleitem.h \
    settingsdialog.h \
    imagechooser.h \
    highscoresdialog.h \
    jigsawpuzzleboard.h \
    puzzleboard.h \
    util.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    imagechooser.ui \
    highscoresdialog.ui

RESOURCES += \
    resources.qrc

maemo5 {
    CONFIG += mobility
    MOBILITY += sensors
    QT += maemo5
    CONFIG += link_pkgconfig
    PKGCONFIG += hildon-extras-1
}
symbian {
    CONFIG += mobility
    MOBILITY += sensors
    TARGET.UID3 = 0xe5b4435f
    # TARGET.CAPABILITY +=
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
win32 {
    RC_FILE = puzzle-master.rc
}
wince {
    RC_FILE = puzzle-master.rc
}

maemo5 {
    target.path = /opt/usr/bin
    INSTALLS += target
}
