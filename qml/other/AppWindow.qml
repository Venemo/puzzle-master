
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
    width: 854
    height: 480

    PuzzleBoard {
        id: puzzleBoard
        tolerance: 10
        rotationTolerance: 20
        anchors.fill: parent
        onGameWon: {
            gameWonDialog.open();
        }
    }

    ImageChooser {
        id: imageChooser
        anchors.fill: parent
        onAccepted: {
            puzzleBoard.visible = true;
            imageChooser.visible = false;
            puzzleBoard.startGame(imageChooser.selectedImageUrl, 3, 4, true);
        }
    }

    Dialog {
        id: gameWonDialog
        title: qsTr("You rock!")
        text: qsTr("Congratulations, you have successfully solved the given puzzle.")
        acceptButtonText: qsTr("Play again")
        rejectButtonText: qsTr("Choose other")
        onAccepted: {
            puzzleBoard.startGame(imageChooser.selectedImageUrl, 3, 4, true);
        }
        onRejected: {
            puzzleBoard.visible = false;
            imageChooser.visible = true;
        }
    }
}
