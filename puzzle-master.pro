
# This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Copyright (C) 2010-2011, Timur Kristóf <venemo@fedoraproject.org>

QT += core gui declarative

TARGET = puzzle-master
TEMPLATE = app
VERSION = 1.9.97
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    main.cpp \
    appsettings.cpp \
    appeventhandler.cpp \
    puzzleitem.cpp \
    puzzleboard.cpp

HEADERS += \
    util.h \
    appsettings.h \
    appeventhandler.h \
    puzzleitem.h \
    puzzleboard.h

RESOURCES += \
    translations.qrc \
    pics-original.qrc \
    ui-other.qrc

TRANSLATIONS += \
    translations/puzzle-master_hu_HU.ts \
    translations/puzzle-master_sv_SE.ts \
    translations/puzzle-master_ru_RU.ts \
    translations/puzzle-master_it_IT.ts \
    translations/puzzle-master_cs_CZ.ts

OTHER_FILES += \
    qml/other/components/Panel.qml \
    qml/other/components/Dialog.qml \
    qml/other/components/Button.qml \
    qml/other/components/Slider.qml \
    qml/other/components/style/ButtonStyle.qml \
    qml/other/components/style/GreenButtonStyle.qml \
    qml/other/components/style/RedButtonStyle.qml \
    qml/other/AppWindow.qml \
    qml/other/ImageChooser.qml \
    qml/other/GameBoard.qml \
    qml/other/OptionsDialog.qml \
    qml/other/FileSelectorDialog.qml \
    qml/other/GallerySelectorDialog.qml \
    qml/other/components/VerticalSlider.qml \
    qml/other/components/VerticalScrollBar.qml

# Platforms

unix:!symbian {
    QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -O3 -ffast-math
    QMAKE_LFLAGS += -pie -rdynamic
    DEFINES += HAVE_OPENGL DISABLE_QMLGALLERY
    INSTALLS += target iconfile desktopfile

    target.path = /usr/bin
    iconfile.path = /usr/share/icons/hicolor/scalable/apps
    iconfile.files = installables/puzzle-master.svg
    desktopfile.path = /usr/share/applications
    desktopfile.files = installables/puzzle-master.desktop
}
contains(MEEGO_EDITION, harmattan) {
    # We want to use applauncherd here, and it has an accelerometer
    DEFINES += HAVE_APPLAUNCHERD HAVE_QACCELEROMETER HAVE_SWIPELOCK MEEGO_EDITION_HARMATTAN
    # The MeeGo graphics system is better than using QGLWidget
    DEFINES -= HAVE_OPENGL DISABLE_QMLGALLERY
}
maemo5 {
    # We want the accelerometer
    DEFINES += HAVE_QACCELEROMETER DISABLE_ROTATION
    DEFINES -= DISABLE_QMLGALLERY
    # And the weird Maemo5 paths
    target.path = /opt/puzzle-master
    desktopfile.path = /usr/share/applications/hildon
}
symbian {
    QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -O3 -ffast-math
    # We want the accelerometer, but not OpenGL
    DEFINES += HAVE_QACCELEROMETER HAVE_DEVICEINFO QT_NO_OPENGL APP_VERSION=\"$$VERSION\"
    DEFINES -= HAVE_OPENGL DISABLE_QMLGALLERY APP_VERSION=\\\"$$VERSION\\\"
    QT -= opengl
    # Symbian icon
    ICON = installables/puzzle-master.svg
    TARGET = PuzzleMaster
    # Some weird Symbian stuff...
    TARGET.UID3 = 0xe5b4435f
    TARGET.EPOCSTACKSIZE = 0x28000
    LIBS += -lcone -leikcore -lavkon
    # Max. heap size is 20 MiB
    TARGET.EPOCHEAPSIZE = 0x020000 0x20971520
}
win32 {
    DEFINES += HAVE_OPENGL _USE_MATH_DEFINES _CRT_SECURE_NO_WARNINGS
}

# Features

contains(DEFINES, HAVE_APPLAUNCHERD) {
    # If we want to use applauncherd from MeeGo/Harmattan
    CONFIG += qdeclarative-boostable link_pkgconfig
    PKGCONFIG += qdeclarative-boostable
    INCLUDEPATH += /usr/include/applauncherd
    desktopfile.files = installables/puzzle-master-applauncherd.desktop
}
contains(DEFINES, HAVE_OPENGL) {
    # If we want QGLWidget as viewport
    QT += opengl
    DEFINES -= QT_NO_OPENGL
}
contains(DEFINES, HAVE_QACCELEROMETER) {
    # Ability to enable accelerometer
    CONFIG += mobility
    MOBILITY += sensors
}
contains(DEFINES, HAVE_DEVICEINFO) {
    # Checking for device models
    CONFIG += mobility
    MOBILITY += systeminfo
}
