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
