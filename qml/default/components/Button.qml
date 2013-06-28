
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
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

import QtQuick 2.0
import "./style"

Rectangle {
    property alias text: buttonText.text
    property alias font: buttonText.font
    property variant style: ButtonStyle { }

    signal clicked

    id: button
    radius: 10
    width: buttonText.width * 1.5
    height: buttonText.height * 1.75
    border.color: button.style.borderColor
    border.width: 2
    gradient: button.style.normalGradient

    TextEdit {
        id: buttonText
        anchors.centerIn: parent
        font.pixelSize: 25 * textScalingFactor
        color: button.style.fontColor
        activeFocusOnPress: false
    }
    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        anchors.margins: enableMobileTweaks ? -10 : 0
        onPressed: button.gradient = button.style.pressedGradient
        onReleased: {
            button.gradient = button.style.normalGradient;
            if (enableMobileTweaks) {
                button.clicked();
            }
        }
        onClicked: {
            if (!enableMobileTweaks) {
                button.clicked();
            }
        }
    }
}
