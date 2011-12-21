import QtQuick 1.0
import net.venemo.puzzlemaster 2.0

Rectangle {
    width: 854
    height: 480

    PuzzleBoard
    {
        anchors.fill: parent
        id: puzzleBoard
        tolerance: 15
        rotationTolerance: 20

        onGameWon: {
            playButton.visible = true;
        }
    }

    Rectangle {
        id: playButton
        color: "white"
        radius: 10
        width: 200
        height: 75
        border.color: "gray"
        anchors.centerIn: parent

        Text {
            text: qsTr("Click here to play")
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                playButton.visible = false;
                puzzleBoard.startGame(":/image5.jpg", 3, 4, true);
            }
        }
    }
}
