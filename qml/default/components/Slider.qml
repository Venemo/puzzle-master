
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

    function adjustHandleX() {
        var val = value < minValue ? minValue : (value > maxValue ? maxValue : value)
        handle.x = picker.width * (val - minValue) / (maxValue - minValue)
    }
    function calculateValue() {
        value = Math.round((maxValue - minValue) * handle.x / picker.width) + minValue
    }

    id: slider
    height: 40
    width: 100
    color: "transparent"

    onValueChanged: {
        if (!sliderDragArea.pressed)
            adjustHandleX()
    }
    onMaxValueChanged: {
        if (value > maxValue)
            value = maxValue
        else if (!sliderDragArea.pressed)
            adjustHandleX()
    }

    onVisibleChanged: adjustHandleX()

    Rectangle {
        id: picker
        height: 15 * uiScalingFactor
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
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: handle.width / 2
        anchors.rightMargin: handle.width / 2
        anchors.verticalCenter: parent.verticalCenter
    }
    Button {
        id: handle
        x: 0
        width: slider.height * uiScalingFactor - 4
        height: slider.height * uiScalingFactor - 4
        radius: width / 2 - 1
        anchors.verticalCenter: parent.verticalCenter
    }
    MouseArea {
        id: sliderDragArea
        anchors.fill: parent
        onMouseXChanged: {
            if (pressed)
            {
                var xx = mouseX - handle.width / 2
                handle.x = (xx < 0 ? 0 : (xx > picker.width ? picker.width : xx))
                calculateValue()
            }
        }
        onPressed: handle.gradient = handle.style.pressedGradient
        onReleased: {
            handle.gradient = handle.style.normalGradient
            calculateValue()
            adjustHandleX()
        }
    }
}
