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
            puzzleBoard.visible = false;
            imageChooser.visible = true;
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
}
