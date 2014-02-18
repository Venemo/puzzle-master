
// This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

import QtQuick 2.0
import QtQuick.Dialogs 1.0
import net.venemo.puzzlemaster 2.0
import "./components"
import "./components/style"

Rectangle {
    width: initialSize.width
    height: initialSize.height
    color: "#000000"
    id: appWindow

    property bool filePickerOnStart: false

    function pathToUrl(path) {
        var newurl = path;
        if (newurl.indexOf("file://") !== 0 && newurl[0] !== ':') {
            if (newurl[0] !== '/') {
                newurl = '/' + newurl;
            }
            newurl = "file://" + newurl;
        }
        if (newurl[0] === ':') {
            newurl = "qrc" + newurl;
        }

        return newurl;
    }

    Component.onCompleted: {
        // Parse command line parameters, if any

        if (Qt.application.arguments.length <= 1) {
            // No command-line arguments
            imageChooser.visible = true;
        }
        else {
            appWindow.filePickerOnStart = (Qt.application.arguments.indexOf("--filepicker-on-start") >= 0);

            // Rows
            var iRows = Qt.application.arguments.indexOf("--rows");
            if (iRows >= 0) {
                var rows = parseInt(Qt.application.arguments[iRows + 1]);
                if (isNaN(rows)) {
                    rows = 3;
                }
                appSettings.rows = rows;
            }

            // Columns
            var iCols = Qt.application.arguments.indexOf("--cols");
            if (iCols >= 0) {
                var cols = parseInt(Qt.application.arguments[iCols + 1]);
                if (isNaN(cols)) {
                    cols = 3;
                }
                appSettings.columns = cols;
            }

            if (appWindow.filePickerOnStart) {
                // Show a file picker on start, instead of the start screen
                // NOTE: This feature can NOT be backported to the Qt4 version without serious hassle.
                filePicker.open();
            }
            else {
                imageChooser.visible = true;
            }
        }
    }

    FileDialog {
        id: filePicker
        title: qsTr("Welcome! Choose an image.")
        nameFilters: [ "Image files (*.jpg *.jpeg *.png *.bmp)", "All files (*)" ]
        onAccepted: {
            var url = filePicker.fileUrl.toString();
            if (url.indexOf("file://") >= 0) {
                url = url.substring("file://".length);
            }
            imageChooser.selectedImagePath = url;
            appEventHandler.activateAppWindow();
            gameBoard.open();
            gameBoard.play();
        }
        onRejected: {
            console.log("User didn't choose an image, quitting.");
            Qt.quit();
        }
    }

    RedButtonStyle {
        id: redButtonStyle
    }
    GreenButtonStyle {
        id: greenButtonStyle
    }
    PurpleButtonStyle {
        id: purpleButtonStyle
    }

    AppSettings {
        id: appSettings
        onCustomImageAlreadyAdded: alreadyAddedDialog.open()
    }
    GameBoard {
        id: gameBoard
        visible: false
        anchors.fill: parent
        game.onGameWon: {
            gameWonDialog.open()
        }
    }
    ImageChooser {
        property bool shouldStartGame: false

        id: imageChooser
        anchors.fill: parent
        visible: false

        onAccepted: {
            optionsDialog.open()
        }
        onClosed: {
            if (imageChooser.shouldStartGame) {
                imageChooser.shouldStartGame = false
                gameBoard.open()
                gameBoard.play()
            }
        }
        onVisibleChanged: {
            if (imageChooser.visible) {
                if (appWindow.filePickerOnStart) {
                    // With "--filepicker-on-start" returning to imageChooser is not allowed,
                    // and doing so is a sign that the user is done playing.
                    Qt.quit();
                }
                else {
                    // Activate app window if not yet active
                    appEventHandler.activateAppWindow();
                }
            }
        }
    }
    Dialog {
        id: gameWonDialog
        title: qsTr("You're a winner!")
        text: qsTr("Congratulations, you have successfully solved the given puzzle.")
        enableBackgroundClicking: false
        acceptButtonText: qsTr("Play again")
        rejectButtonText: appWindow.filePickerOnStart ? qsTr("Quit") : qsTr("Choose other")
        backgroundColor: "#99101010"
        onAccepted: {
            gameBoard.play()
        }
        onRejected: {
            gameBoard.close()
            imageChooser.open()
        }
    }
    Dialog {
        id: areYouSureToQuitDialog
        title: qsTr("Are you sure?")
        text: qsTr("Do you wish to quit the game?")
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        onAccepted: Qt.quit()
    }
    Dialog {
        id: aboutDialog
        title: qsTr("About Puzzle Master %1").arg(appVersion)
        text: qsTr("Brought to you by Timur Kristóf.\nPuzzle Master is a fun and addictive jigsaw puzzle game.\nUsing Qt.")
        acceptButtonText: qsTr("Ok")
    }
    OptionsDialog {
        id: optionsDialog
        onAccepted: {
            if (appSettings.rows >= 4 || appSettings.columns >= 6)
                difficultyDialog.open()
            else
                imageChooser.startGame()
        }
    }
    Dialog {
        id: difficultyDialog
        title: qsTr("Isn't this a bit difficult?")
        text: qsTr("The selected options will make the game VERY difficult.\nAre you sure to proceed?")
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        onAccepted: imageChooser.startGame()
        onRejected: optionsDialog.open()
    }
    Dialog {
        id: alreadyAddedDialog
        title: ""
        text: qsTr("You have already added the selected image.")
        acceptButtonText: qsTr("Ok")
    }
}
