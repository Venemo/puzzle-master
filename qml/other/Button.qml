import QtQuick 1.0

Rectangle {
    property alias text: buttonText.text
    property Gradient normalGradient: Gradient {
        GradientStop { position: 0; color: "lightblue"; }
        GradientStop { position: 0.5; color: "lightblue"; }
        GradientStop { position: 1; color: "blue"; }
    }
    property Gradient pressedGradient: Gradient {
        GradientStop { position: 0; color: "blue"; }
        GradientStop { position: 1; color: "blue"; }
    }
    signal clicked

    id: button
    radius: 10
    width: buttonText.width * 1.3
    height: buttonText.height * 1.5
    border.color: "#88464646"
    border.width: 1
    smooth: true
    anchors.centerIn: parent
    gradient: normalGradient

    Text {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: 20
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
