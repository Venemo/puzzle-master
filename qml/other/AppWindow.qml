
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

Rectangle {
    width: initialSize.width
    height: initialSize.height

    AppSettings {
        id: appSettings
    }
    GameBoard {
        id: gameBoard
        anchors.fill: parent
        onGameWon: {
            gameWonDialog.open();
        }
    }
    ImageChooser {
        id: imageChooser
        anchors.fill: parent
        onAccepted: {
            optionsDialog.open();
        }
    }
    Dialog {
        id: gameWonDialog
        title: qsTr("You rock!")
        text: qsTr("Congratulations, you have successfully solved the given puzzle.")
        enableBackgroundClicking: false
        acceptButtonText: qsTr("Play again")
        rejectButtonText: qsTr("Choose other")
        backgroundColor: "#99101010"
        onAccepted: {
            gameBoard.play();
        }
        onRejected: {
            gameBoard.visible = false;
            imageChooser.visible = true;
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
        title: qsTr("About Puzzle Master")
        text: qsTr("Brought to you by Timur Kristóf.\nPuzzle Master is a fun and addictive jigsaw puzzle game.\nUsing Qt.")
        acceptButtonText: qsTr("Ok")
    }
    OptionsDialog {
        id: optionsDialog
        onAccepted: {
            imageChooser.visible = false;
            gameBoard.visible = true;
            gameBoard.play();
        }
        onVisibleChanged: {
            optionsDialog.columns = appSettings.columns;
            optionsDialog.rows = appSettings.rows;
        }
        onColumnsChanged: appSettings.columns = columns
        onRowsChanged: appSettings.rows = rows
    }
}
