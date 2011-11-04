import QtQuick 1.1
import com.nokia.meego 1.0
import com.meego.extras 1.0
import net.venemo.puzzlemaster 2.0

Page
{
    id: gamePage
    lockInLandscape: true

    PuzzleBoard
    {
        width: 854
        height: 480
        anchors.centerIn: parent
        id: puzzleBoard
    }

    Component.onCompleted: puzzleBoard.startGame(":/5.jpg", 3, 3, true);
}
