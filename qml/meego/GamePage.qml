import QtQuick 1.1
import com.nokia.meego 1.0
import com.meego.extras 1.0
import net.venemo.puzzlemaster 2.0

Page
{
    id: gamePage

    PuzzleBoard
    {
        width: 450
        height: 450
        anchors.centerIn: parent
        id: puzzleBoard
    }

    Component.onCompleted: puzzleBoard.startGame(":/5.jpg", 3, 3, true);
}
