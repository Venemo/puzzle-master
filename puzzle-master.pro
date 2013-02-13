
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
# Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

QT = core gui declarative

TARGET = puzzle-master
TEMPLATE = app
VERSION = 2.1.92
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    main.cpp \
    appsettings.cpp \
    appeventhandler.cpp \
    puzzleitem.cpp \
    puzzleboard.cpp \
    helpers/shapeprocessor.cpp \
    helpers/imageprocessor.cpp

HEADERS += \
    util.h \
    appsettings.h \
    appeventhandler.h \
    puzzleitem.h \
    puzzleboard.h \
    helpers/shapeprocessor.h \
    helpers/imageprocessor.h \
    helpers/helpertypes.h

RESOURCES += \
    translations.qrc \
    pics-original.qrc \
    ui-other.qrc \
    background.qrc

TRANSLATIONS += \
    translations/puzzle-master_hu_HU.ts \
    translations/puzzle-master_sv_SE.ts \
    translations/puzzle-master_ru_RU.ts \
    translations/puzzle-master_it_IT.ts \
    translations/puzzle-master_cs_CZ.ts \
    translations/puzzle-master_he_IL.ts \
    translations/puzzle-master_tr_TR.ts \
    translations/puzzle-master_fi_FI.ts

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
    qml/other/components/VerticalScrollBar.qml \
    qml/other/components/MenuButton.qml \
    qml/other/components/style/PurpleButtonStyle.qml

# Platforms

unix:!symbian {
    QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden -O3 -ffast-math
    !blackberry {
        QMAKE_LFLAGS += -pie -rdynamic
    }
    DEFINES += HAVE_OPENGL DISABLE_QMLGALLERY FORCE_PLATFORM_FILE_DIALOG
    INSTALLS += target iconfile desktopfile

    target.path = /usr/bin
    iconfile.path = /usr/share/icons/hicolor/scalable/apps
    iconfile.files = installables/puzzle-master.svg
    desktopfile.path = /usr/share/applications
    desktopfile.files = installables/puzzle-master.desktop
}
contains(MEEGO_EDITION, harmattan) {
    # We want to use applauncherd here + harmattan specifics and the touchscreen
    DEFINES += HAVE_APPLAUNCHERD HAVE_SWIPELOCK MEEGO_EDITION_HARMATTAN DISABLE_SCROLLBARS
    # The MeeGo graphics system is better than using QGLWidget
    DEFINES -= HAVE_OPENGL DISABLE_QMLGALLERY FORCE_PLATFORM_FILE_DIALOG
    # Optification is needed by the Nokia Store
    target.path = /opt/puzzle-master
    INSTALLS += splash
    splash.files = installables/puzzle-master-splash.jpg
    splash.path = /opt/puzzle-master
}
maemo5 {
    QT += dbus
    # Disable rotation (no multitouch) and scrollbars
    DEFINES += DISABLE_ROTATION DISABLE_SCROLLBARS
    DEFINES -= DISABLE_QMLGALLERY
    # And the weird Maemo5 paths
    target.path = /opt/puzzle-master
    iconfile.path = /usr/share/pixmaps
    iconfile.files = installables/puzzle-master.png
    desktopfile.files = installables/puzzle-master-fremantle.desktop
    desktopfile.path = /usr/share/applications/hildon
}
symbian {
    QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -O3 -ffast-math
    # No OpenGL, no scrollbars and custom appversion hack
    DEFINES += QT_NO_OPENGL APP_VERSION=\"$$VERSION\" DISABLE_SCROLLBARS
    DEFINES -= HAVE_OPENGL DISABLE_QMLGALLERY FORCE_PLATFORM_FILE_DIALOG APP_VERSION=\\\"$$VERSION\\\"
    QT -= opengl
    CONFIG += mobility
    MOBILITY += systeminfo
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
    TARGET = PuzzleMaster
    DEFINES += HAVE_OPENGL FORCE_PLATFORM_FILE_DIALOG _USE_MATH_DEFINES _CRT_SECURE_NO_WARNINGS
    RC_FILE = installables/puzzle-master.rc
}
blackberry {
    message("Building Puzzle Master for BlackBerry!")

    DEFINES += DISABLE_SCROLLBARS HAVE_OPENGL

    # NOTE: this will bundle Qt with the app and hack the rpath to make it find Qt
    QMAKE_LFLAGS += '-Wl,-rpath,\'./app/native/lib\''
    package.target = $${TARGET}.bar
    package.depends = $$TARGET
    package.commands = blackberry-nativepackager \
        # NOTE: you need to put your debug token here
        -devMode -debugToken ~/MyBbToken.bar \
        -package $${TARGET}.bar -arg -platform -arg blackberry \
        # NOTE: bar-descriptor.xml and installables/puzzle-master-playbook.png must be in the build directory
        bar-descriptor.xml $$TARGET \
        -e installables/puzzle-master-86.png puzzle-master-playbook.png \
        -e $$[QT_INSTALL_LIBS]/libQtCore.so.4 lib/libQtCore.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtGui.so.4 lib/libQtGui.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtOpenGL.so.4 lib/libQtOpenGL.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtNetwork.so.4 lib/libQtNetwork.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtDeclarative.so.4 lib/libQtDeclarative.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtScript.so.4 lib/libQtScript.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtSvg.so.4 lib/libQtSvg.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtSql.so.4 lib/libQtSql.so.4 \
        -e $$[QT_INSTALL_LIBS]/libQtXmlPatterns.so.4 lib/libQtXmlPatterns.so.4 \
        -e $$[QT_INSTALL_PLUGINS]/imageformats/libqjpeg.so plugins/imageformats/libqjpeg.so \
        -e $$[QT_INSTALL_PLUGINS]/platforms/libblackberry.so plugins/platforms/libblackberry.so

    QMAKE_EXTRA_TARGETS += package
}

# Features

contains(DEFINES, HAVE_APPLAUNCHERD) {
    # If we want to use applauncherd from MeeGo/Harmattan
    CONFIG += qdeclarative-boostable link_pkgconfig
    PKGCONFIG += qdeclarative-boostable
    INCLUDEPATH += /usr/include/applauncherd
    desktopfile.files = installables/puzzle-master-applauncherd.desktop
    contains(MEEGO_EDITION, harmattan) {
        desktopfile.files = installables/puzzle-master-harmattan.desktop
    }
}
contains(DEFINES, HAVE_OPENGL) {
    # If we want QGLWidget as viewport
    QT += opengl
    DEFINES -= QT_NO_OPENGL
}
contains(DEFINES, HAVE_DEVICEINFO_CHECK) {
    # Checking for device models
    CONFIG += mobility
    MOBILITY += systeminfo
}
