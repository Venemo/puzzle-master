
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

PuzzleBoard {
    function play() {
        progressDialog.open()
    }
    function close() {
        menuDialog.visible = false
        gameBoard.visible = false
    }
    function open() {
        gameBoard.visible = true
    }

    property bool isHintDisplayed: false
    property bool waitForHint: false

    id: gameBoard
    game.tolerance: (- appSettings.snapDifficulty + 3) * 7 * uiScalingFactor
    game.rotationTolerance: (- appSettings.snapDifficulty + 3) * 9 * uiScalingFactor
    z: 0
    onVisibleChanged: {
        menuButtonPanel.visible = false
        gameBoard.game.deleteAllPieces()
        appEventHandler.adjustForUi()
    }
    game.onGameAboutToBeWon: {
        menuButtonPanel.close()
    }
    game.onLoadProgressChanged: {
        if (progress > 0)
            progressDialog.text = qsTr("Creating puzzle piece %1 of %2").arg(progress).arg(appSettings.rows * appSettings.columns)
        else
            progressDialog.text = qsTr("The selected image is being processed.")
    }
    game.onLoaded: {
        appEventHandler.adjustForPlaying()
        progressDialog.close()
    }
    game.onGameStarted: {
        menuDialog.shouldReenableGame = true
        menuButtonPanel.open()
    }
    game.onGameWon: {
        appEventHandler.adjustForUi()
        menuButtonPanel.close()
    }
    game.onRestoreComplete: {
        menuDialog.shouldReenableGame = true
        gameBoard.isHintDisplayed = false
        gameBoard.waitForHint = false
    }
    game.onAssembleComplete: {
        gameBoard.isHintDisplayed = true
        gameBoard.waitForHint = false
    }

    Rectangle {
        id: rotationGuide
        color: "#fff"
        width: 64 * uiScalingFactor
        height: width
        radius: width / 2
        x: game.rotationGuideCoordinates.x - width / 2
        y: game.rotationGuideCoordinates.y - height / 2

        Rectangle {
            color: "#9fce00"
            width: 64 * uiScalingFactor - 6
            height: width
            radius: width / 2
            anchors.centerIn: parent

            Image {
                anchors.fill: parent
                fillMode: Image.Stretch
                source: "qrc:/pics/rotation-guide.png"
            }

            MouseArea {
                id: rotationGuideMouseArea
                anchors.fill: parent
                onMousePositionChanged: {
                    if (rotationGuideMouseArea.pressed) {
                        var x = rotationGuide.x + mouse.x;
                        var y = rotationGuide.y + mouse.y;
                        game.rotateWithGuide(x, y);
                    }
                }
                onPressed: {
                    gameBoard.enableAutoRepaint();
                    var x = rotationGuide.x + mouse.x;
                    var y = rotationGuide.y + mouse.y;
                    game.startRotateWithGuide(x, y);
                }
                onReleased: {
                    gameBoard.disableAutoRepaint();
                }
            }
        }
    }

    Image {
        anchors.fill: parent
        source: "qrc:/pics/background.jpg"
        fillMode: Image.Tile
        z: -1

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            gradient: Gradient {
                GradientStop { color: "#77000000"; position: 0.0 }
                GradientStop { color: "#00000000"; position: 1.0 }
            }
            height: 70
        }
    }
    Panel {
        id: menuButtonPanel
        z: 200
        visible: false
        width: 80 * uiScalingFactor
        height: 60 * uiScalingFactor
        color: "#99a909a7"
        anchors {
            fill: null
            top: parent.top
            right: parent.right
        }

        MenuButton {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 6 * uiScalingFactor
            width: 65 * uiScalingFactor
            height: 45 * uiScalingFactor
            style: purpleButtonStyle
            onClicked: menuDialog.open()
        }
    }
    Dialog {
        property bool shouldReenableGame: false

        z: 200
        id: menuDialog
        title: qsTr("Puzzle Master")
        contentHeight: menuDialogColumn.height
        contentWidth: menuDialogColumn.width
        onOpened: {
            appEventHandler.adjustForUi()
            gameBoard.game.disable()
        }
        onClosed: {
            appEventHandler.adjustForPlaying()
            if (menuDialog.shouldReenableGame)
                gameBoard.game.enable()
        }
        content: Column {
            id: menuDialogColumn
            spacing: 10 * uiScalingFactor

            Button {
                width: 500 * uiScalingFactor
                text: waitForHint ? qsTr("Wait...") : (gameBoard.isHintDisplayed ? qsTr("Continue game") : qsTr("Get a hint!"))
                onClicked: {
                    if (!waitForHint) {
                        if (gameBoard.isHintDisplayed) {
                            gameBoard.waitForHint = true
                            menuDialog.close()
                            gameBoard.game.restore()
                        }
                        else {
                            menuDialog.shouldReenableGame = false
                            gameBoard.waitForHint = true
                            menuDialog.close()
                            gameBoard.game.assemble()
                        }
                    }
                }
                style: greenButtonStyle
            }
            Row {
                spacing: 10 * uiScalingFactor

                Button {
                    width: 245 * uiScalingFactor
                    text: qsTr("Restart")
                    onClicked: areYouSureToRestartDialog.open()
                }
                Button {
                    width: 245 * uiScalingFactor
                    text: qsTr("Surrender")
                    onClicked: areYouSureToSurrenderDialog.open()
                }
            }
            Button {
                width: 500 * uiScalingFactor
                text: qsTr("About")
                onClicked: {
                    menuDialog.close()
                    aboutDialog.open()
                }
            }
            Button {
                width: 500 * uiScalingFactor
                text: qsTr("Quit")
                style: redButtonStyle
                onClicked: {
                    menuDialog.close()
                    areYouSureToQuitDialog.open()
                }
            }
        }
        Button {
            id: appSwitcherButton
            visible: appEventHandler.showAppSwitcherButton()
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.topMargin: 6
            width: 70 * uiScalingFactor
            height: 48 * uiScalingFactor
            text: ""
            style: greenButtonStyle
            onClicked: {
                appEventHandler.displayAppSwitcher()
            }
            Rectangle {
                color: "#FFFFFF"
                border.color: "#7DB72F"
                border.width: 1
                width: 25 * uiScalingFactor
                height: 25 * uiScalingFactor
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -5
                anchors.horizontalCenterOffset: -5
            }
            Rectangle {
                color: "#FFFFFF"
                border.color: "#7DB72F"
                border.width: 1
                width: 25 * uiScalingFactor
                height: 25 * uiScalingFactor
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 5
                anchors.horizontalCenterOffset: 5
            }
        }
    }
    Dialog {
        id: progressDialog
        z: 200
        title: qsTr("Please wait...")
        backgroundColor: "#99101010"
        enableBackgroundClicking: false
        onOpened: {
            if (!gameBoard.game.startGame(decodeURI(imageChooser.selectedImagePath), appSettings.rows, appSettings.columns, allowRotation)) {
                progressDialog.close()
                failedToStartDialog.open()
            }
        }
    }
    Dialog {
        id: pausedDialog
        z: 200
        title: qsTr("Game paused")
        text: qsTr("Switch back to continue it.")
        backgroundColor: "#99101010"
        opacity: 1
        onVisibleChanged: {
            if (pausedDialog.visible)
                menuDialog.visible = false
            else
                menuDialog.visible = true
        }
        Connections {
            target: appEventHandler
            onWindowActivated: pausedDialog.visible = false
            onWindowDeactivated: pausedDialog.visible = true
        }
    }
    Dialog {
        id: areYouSureToSurrenderDialog
        z: 200
        title: qsTr("Are you sure?")
        text: qsTr("Do you want to give up this game?\nYou can't continue it then. Use \"Get a hint!\" if you need a clue.")
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        onAccepted: {
            menuDialog.visible = false
            gameBoard.close()
            imageChooser.open()
        }
    }
    Dialog {
        id: areYouSureToRestartDialog
        z: 200
        title: qsTr("Are you sure?")
        text: qsTr("Do you want to restart this game?")
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        onAccepted: {
            menuDialog.close()
            menuButtonPanel.close()
            play()
        }
    }
    Dialog {
        id: failedToStartDialog
        z: 200
        enableBackgroundClicking: false
        title: qsTr("An error has occoured")
        text: qsTr("Sorry, we couldn't start the game. Please try to start it with another picture.")
        acceptButtonText: qsTr("Ok")
        onAccepted: {
            gameBoard.close()
            imageChooser.open()
        }
    }
}
