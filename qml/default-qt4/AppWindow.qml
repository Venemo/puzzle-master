
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

import QtQuick 1.0
import net.venemo.puzzlemaster 2.0
import "./components"
import "./components/style"

Rectangle {
    width: initialSize.width
    height: initialSize.height
    color: "#000"
    id: appWindow

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
    }
    Dialog {
        id: gameWonDialog
        title: qsTr("You're a winner!")
        text: qsTr("Congratulations, you have successfully solved the given puzzle.")
        enableBackgroundClicking: false
        acceptButtonText: qsTr("Play again")
        rejectButtonText: qsTr("Choose other")
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
