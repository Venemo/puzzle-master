
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
// Copyright (C) 2010-2011, Timur Kristóf <venemo@fedoraproject.org>

import QtQuick 1.0
import net.venemo.puzzlemaster 2.0
import "./components"
import "./components/style"

PuzzleBoard {
    function play() {
        progressDialog.open()
    }

    id: gameBoard
    tolerance: 15
    rotationTolerance: 20
    z: 0
    onLoadProgressChanged: {
        if (progress > 0)
            progressDialog.text = qsTr("Creating puzzle piece %1").arg(progress)
        else
            progressDialog.text = qsTr("The selected image is being processed.")
    }
    onLoaded: {
        appEventHandler.adjustForPlaying()
        progressDialog.close()
    }
    onGameStarted: {
        menuDialog.shouldReenableGame = true
        menuButtonPanel.open()
    }
    onGameWon: {
        appEventHandler.adjustForUi()
        menuButtonPanel.close()
    }
    onVisibleChanged: {
        menuButtonPanel.visible = false
        gameBoard.deleteAllPieces()
        appEventHandler.adjustForUi()
    }

    Panel {
        id: menuButtonPanel
        z: 1
        visible: false
        opacity: 0
        anchors.fill: null
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 82
        height: 60
        color: "#997DB72F"

        Button {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 6
            width: 70
            height: 48
            text: qsTr("...")
            style: GreenButtonStyle { }
            onClicked: menuDialog.open()
        }
    }
    Dialog {
        property bool shouldReenableGame: false

        z: 1
        id: menuDialog
        title: "PuzzleMaster"
        contentHeight: menuDialogColumn.height
        contentWidth: menuDialogColumn.width
        onOpened: {
            appEventHandler.adjustForUi()
            gameBoard.disable()
        }
        onClosed: {
            appEventHandler.adjustForPlaying()
            if (menuDialog.shouldReenableGame)
                gameBoard.enable()
        }
        content: Column {
            id: menuDialogColumn
            spacing: 10

            Button {
                width: 500
                text: qsTr("Solve game")
                onClicked: {
                    menuDialog.shouldReenableGame = false
                    menuDialog.close()
                    gameBoard.assemble()
                }
            }
            Button {
                width: 500
                text: qsTr("Restart game")
                onClicked: {
                    menuDialog.close()
                    menuButtonPanel.close()
                    play()
                }
            }
            Button {
                width: 500
                text: qsTr("Abandon game")
                onClicked: {
                    menuDialog.visible = false
                    gameBoard.visible = false
                    imageChooser.open()
                }
            }
            Button {
                width: 500
                text: qsTr("About")
                onClicked: {
                    menuDialog.close()
                    aboutDialog.open()
                }
            }
            Button {
                width: 500
                text: qsTr("Quit")
                style: RedButtonStyle { }
                onClicked: {
                    menuDialog.close()
                    areYouSureToQuitDialog.open()
                }
            }
        }
    }
    Dialog {
        id: progressDialog
        z: 1
        title: qsTr("Please wait...")
        backgroundColor: "#99101010"
        enableBackgroundClicking: false
        onOpened: gameBoard.startGame(imageChooser.selectedImageUrl, optionsDialog.rows, optionsDialog.columns, true)
    }
    Dialog {
        id: pausedDialog
        z: 1
        title: qsTr("Game paused")
        text: qsTr("Switch back to continue it")
        opacity: 1
        onVisibleChanged: {
            if (pausedDialog.visible)
                menuDialog.visible = false;
            else
                menuDialog.visible = true;
        }
        Connections {
            target: appEventHandler
            onWindowActivated: pausedDialog.visible = false
            onWindowDeactivated: pausedDialog.visible = true
        }
    }
}
