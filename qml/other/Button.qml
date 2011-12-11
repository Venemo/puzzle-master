import QtQuick 1.0

Rectangle {
    property alias text: buttonText.text
    property Gradient normalGradient: Gradient {
        GradientStop { position: 0; color: "#7DB72F"; }
        GradientStop { position: 1; color: "#4E7D0E"; }
    }
    property Gradient pressedGradient: Gradient {
        GradientStop { position: 0; color: "#4E7D0E"; }
        GradientStop { position: 1; color: "#4E7D0E"; }
    }
    property string borderColor: "#538312"
    property string fontColor: "#ffffff"
    signal clicked

    id: button
    radius: 10
    width: buttonText.width * 1.5
    height: buttonText.height * 1.75
    border.color: button.borderColor
    border.width: 2
    smooth: true
    gradient: normalGradient

    Text {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: 25
        color: button.fontColor
    }
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        onPressed: {
            button.gradient = button.pressedGradient;
        }
        onReleased: {
            button.gradient = button.normalGradient;
        }
        onClicked: button.clicked();
    }
}
