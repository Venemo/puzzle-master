
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
import net.venemo.puzzlemaster 2.0
import "./components"
import "./components/style"

Rectangle {
    width: initialSize.width
    height: initialSize.height
    color: "#000000"
    id: appWindow

    property bool filePickerOnStart: false
    property variant filePicker: null

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

    // Loads the file picker
    function loadFilePicker() {
        // Check if it's already loaded
        if (typeof(filePicker) !== "undefined" && filePicker !== null) {
            console.log("AppWindow: filePicker is already loaded");
            return;
        }

        // Try to load it
        console.log("AppWindow: trying to load FileSelectorDialog");
        var selectorComponent = Qt.createComponent("FileSelectorDialog.qml");
        if (selectorComponent === null || selectorComponent.status === Component.Error || selectorComponent.status === Component.Null) {
            console.log("Can't load FileSelectorDialog");
        }
        else {
            filePicker = selectorComponent.createObject(appWindow);
            filePicker.appWindow = appWindow;
            filePicker.imageChooser = imageChooser;
            filePicker.gameBoard = gameBoard;
        }
    }

    // Parses command line arguments: returns true if file picker should be shown
    function parseCommandLine() {
        if (Qt.application.arguments.length <= 1) {
            return false;
        }

        // Whether to show filepicker on start
        appWindow.filePickerOnStart = (Qt.application.arguments.indexOf("--filepicker-on-start") >= 0);

        // Rows
        var iRows = Qt.application.arguments.indexOf("--rows");
        if (iRows >= 0) {
            var rows = parseInt(Qt.application.arguments[iRows + 1]);
            appSettings.rows = isNaN(rows) ? 3 : rows;
        }

        // Columns
        var iCols = Qt.application.arguments.indexOf("--cols");
        if (iCols >= 0) {
            var cols = parseInt(Qt.application.arguments[iCols + 1]);
            appSettings.columns = isNaN(cols) ? 3 : cols;
        }

        if (appWindow.filePickerOnStart) {
            loadFilePicker();
            if (filePicker === null) {
                console.log("Can't show file picker, quitting.");
                Qt.quit();
                return false;
            }
            return true;
        }
        return false;
    }

    Component.onCompleted: {
        // Check if ImageChooser has a picker component
        if (typeof(imageChooser.fileSelectorDialog) === "undefined" || imageChooser.fileSelectorDialog === null) {
            loadFilePicker();
            imageChooser.fileSelectorDialog = filePicker;
        }

        // Parse command line parameters, if any
        if (parseCommandLine()) {
            // command-line arguments tell that we should start with a file picker
            filePicker.open();
        }
        else {
            // No command-line arguments
            imageChooser.visible = true;
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
            if (!appSettings.advancedMode && (appSettings.rows >= 4 || appSettings.columns >= 6)) {
                difficultyDialog.open();
            }
            else {
                imageChooser.startGame();
            }
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
