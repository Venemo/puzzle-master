
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

PuzzleBoard {
    function play() {
        gameBoard.startGame(imageChooser.selectedImageUrl, optionsDialog.rows, optionsDialog.columns, true);
    }

    id: gameBoard
    tolerance: 10
    rotationTolerance: 20
    z: 0

    Rectangle {
        z: 1
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
            text: "..."
            onClicked: {
                menuDialog.open();
            }
        }
    }
    Dialog {
        z: 1
        id: menuDialog
        title: "Puzzle Master"
        contentHeight: menuDialogColumn.height
        contentWidth: menuDialogColumn.width
        content: Column {
            id: menuDialogColumn
            spacing: 10

            Button {
                width: 500
                text: qsTr("Solve game")
                onClicked: {
                    menuDialog.close();
                    gameBoard.assemble();
                }
            }
            Button {
                width: 500
                text: qsTr("Restart game")
                onClicked: {
                    menuDialog.close();
                    play();
                }
            }
            Button {
                width: 500
                text: qsTr("Abandon game")
                onClicked: {
                    menuDialog.close();
                    gameBoard.visible = false;
                    imageChooser.visible = true;
                }
            }
            Button {
                width: 500
                text: qsTr("About")
                onClicked: {
                    menuDialog.close();
                    aboutDialog.open();
                }
            }
            Button {
                width: 500
                text: qsTr("Quit")
                normalGradient: Gradient {
                    GradientStop { position: 0; color: "#ED1C24"; }
                    GradientStop { position: 1; color: "#AA1317"; }
                }
                pressedGradient: Gradient {
                    GradientStop { position: 0; color: "#AA1317"; }
                    GradientStop { position: 1; color: "#AA1317"; }
                }
                borderColor: "#980C10"
                onClicked: {
                    menuDialog.close();
                    areYouSureToQuitDialog.open();
                }
            }
        }
    }
}
