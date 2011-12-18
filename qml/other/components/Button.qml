
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
    gradient: normalGradient

    TextEdit {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: 25
        color: button.fontColor
        activeFocusOnPress: false
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
