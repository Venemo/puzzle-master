
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
    property int animationDuration: 120

    signal opened
    signal closed

    function open() {
        showAnimation.start()
    }
    function close() {
        hideAnimation.start()
    }

    id: panel
    anchors.fill: parent

    NumberAnimation {
        id: showAnimation
        target: panel
        property: "opacity"
        from: 0
        to: 1
        duration: animationDuration
        onStarted: {
            panel.visible = true
            disableAllTheThings.enabled = true
        }
        onStopped: {
            disableAllTheThings.enabled = false
            panel.opened()
        }
    }
    NumberAnimation {
        id: hideAnimation
        target: panel
        property: "opacity"
        from: 1
        to: 0
        duration: animationDuration
        onStarted: {
            disableAllTheThings.enabled = true
        }
        onStopped: {
            panel.visible = false
            panel.closed()
        }
    }
    MouseArea {
        id: disableAllTheThings
        anchors.fill: parent
        enabled: false
        z: 1000
    }
}
