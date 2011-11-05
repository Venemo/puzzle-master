import QtQuick 1.1
import com.nokia.meego 1.0
import com.meego.extras 1.0
import net.venemo.puzzlemaster 2.0

Page
{
    id: gamePage
    orientationLock: PageOrientation.LockLandscape

    PuzzleBoard
    {
        width: 854
        height: 480
        anchors.centerIn: parent
        id: puzzleBoard
    }

    Component.onCompleted:
    {
        puzzleBoard.tolerance = 10;
        puzzleBoard.rotationTolerance = 15;
        puzzleBoard.startGame(":/image5.jpg", 3, 3, true);
    }
}
