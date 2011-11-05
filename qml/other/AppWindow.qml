import QtQuick 1.0
import net.venemo.puzzlemaster 2.0

Rectangle {
    width: 854
    height: 480

    PuzzleBoard
    {
        width: 854
        height: 480
        anchors.centerIn: parent
        id: puzzleBoard
    }

    Component.onCompleted:
    {
        puzzleBoard.tolerance = 12;
        puzzleBoard.rotationTolerance = 17;
        puzzleBoard.startGame(":/image5.jpg", 3, 3, true);
    }
}
