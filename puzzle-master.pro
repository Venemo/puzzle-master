#-------------------------------------------------
#
# Project created by QtCreator 2010-12-01T19:24:52
#
#-------------------------------------------------

QT += core gui declarative

TARGET = puzzle-master
TEMPLATE = app
VERSION = 2.0

SOURCES += \
    main.cpp \
    appsettings.cpp \
    puzzleitem.cpp \
    puzzleboard.cpp

HEADERS += \
    util.h \
    appsettings.h \
    puzzleitem.h \
    puzzleboard.h

RESOURCES += \
    resources.qrc \
    otherGui.qrc

TRANSLATIONS += \
    translations/puzzle-master_hu_HU.ts

OTHER_FILES += \
    data/about.txt \
    qml/other/AppWindow.qml \
    qml/other/Button.qml \
    qml/other/ImageChooser.qml \
    qml/other/Dialog.qml

unix {
    QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
    QMAKE_LFLAGS += -pie -rdynamic
    DEFINES += HAVE_OPENGL
    INSTALLS += target iconfile desktopfile
    OTHER_FILES += data/puzzle-master.desktop

    target.path = /usr/bin
    iconfile.path = /usr/share/pixmaps
    iconfile.files = data/puzzle-master.png
    desktopfile.path = /usr/share/applications
    desktopfile.files = data/puzzle-master.desktop
}
contains(MEEGO_EDITION, harmattan) {
    # We want to use applauncherd here, and it has an accelerometer
    DEFINES += HAVE_APPLAUNCHERD HAVE_QACCELEROMETER
    # The MeeGo graphics system is better than using QGLWidget
    DEFINES -= HAVE_OPENGL
}
maemo5 {
    # We want the accelerometer
    DEFINES += HAVE_QACCELEROMETER
    # And the weird Maemo5 paths
    target.path = /opt/puzzle-master
    desktopfile.path = /usr/share/applications/hildon
}
symbian {
    # We want the accelerometer, but not OpenGL
    DEFINES += HAVE_QACCELEROMETER
    DEFINES -= HAVE_OPENGL
    # Symbian icon
    ICON = data/puzzle-master-44x44.svg
    # Some weird Symbian stuff...
    TARGET.UID3 = 0xe5b4435f
    TARGET.EPOCSTACKSIZE = 0x28000
    # Max. heap size is 20 MiB
    TARGET.EPOCHEAPSIZE = 0x020000 0x20971520
}
win32 {
    # Icon on Windows
    RC_FILE = puzzle-master.rc
    OTHER_FILES += puzzle-master.rc
}

contains(DEFINES, HAVE_APPLAUNCHERD) {
    # If we want to use applauncherd from MeeGo/Harmattan
    CONFIG += qdeclarative-boostable link_pkgconfig
    PKGCONFIG += qdeclarative-boostable
    INCLUDEPATH += /usr/include/applauncherd
}
contains(DEFINES, HAVE_OPENGL) {
    QT += opengl
}
contains(DEFINES, HAVE_QACCELEROMETER) {
    CONFIG += mobility
    MOBILITY += sensors
}
