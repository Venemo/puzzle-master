
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
    property int minValue: 0
    property int maxValue: 10
    property int value: minValue

    function adjustHandleY() {
        var val = value < minValue ? minValue : (value > maxValue ? maxValue : value)
        handle.y = picker.height * (val - minValue) / (maxValue - minValue)
    }
    function calculateValue() {
        value = Math.round((maxValue - minValue) * handle.y / picker.height) + minValue
    }

    id: verticalSlider
    height: 100
    width: 40
    color: "transparent"

    onValueChanged: {
        if (!sliderDragArea.pressed)
            adjustHandleY()
    }
    onVisibleChanged: adjustHandleY()

    Rectangle {
        id: picker
        width: 15 * uiScalingFactor
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#888888"
            }
            GradientStop {
                position: 1
                color: "#575757"
            }
        }
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin: handle.height / 2
        anchors.bottomMargin: handle.height / 2
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Button {
        id: handle
        y: 0
        width: verticalSlider.width * uiScalingFactor - 4
        height: verticalSlider.width * uiScalingFactor - 4
        radius: height / 2 - 1
        anchors.horizontalCenter: parent.horizontalCenter
    }
    MouseArea {
        id: sliderDragArea
        anchors.fill: verticalSlider
        onMouseYChanged: {
            if (pressed)
            {
                var yy = mouseY - handle.height / 2
                handle.y = (yy < 0 ? 0 : (yy > picker.height ? picker.height : yy))
                calculateValue()
            }
        }
        onPressed: {
            handle.gradient = handle.style.pressedGradient
        }
        onReleased: {
            handle.gradient = handle.style.normalGradient
            calculateValue()
            adjustHandleY()
        }
    }
}
