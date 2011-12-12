
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
    property int minValue: 0
    property int maxValue: 10
    property int value: minValue

    function adjustHandleX() {
        handle.x = picker.width * (value - minValue) / (maxValue - minValue);
    }

    id: slider
    height: 40
    width: 100
    color: "transparent"

    onValueChanged: adjustHandleX()
    onVisibleChanged: adjustHandleX()

    Rectangle {
        id: picker
        height: 15
        gradient: Gradient {
            GradientStop { position: 0; color: "#888888"; }
            GradientStop { position: 1; color: "#575757"; }
        }
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: handle.width / 2
        anchors.rightMargin: handle.width / 2
        anchors.verticalCenter: parent.verticalCenter
    }

    Button {
        id: handle
        x: 0
        width: slider.height - 4
        height: slider.height - 4
        radius: width / 2 - 1
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        anchors.fill: parent
        onMouseXChanged: handle.x = (mouseX < 0 ? 0 : (mouseX > picker.width ? picker.width : mouseX))
        onReleased: {
            value = Math.round((maxValue - minValue) * handle.x / picker.width) + minValue;
            adjustHandleX();
        }
    }
}
