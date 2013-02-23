
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

VerticalSlider {
    property Flickable flickable: null
    property bool enableScrolling: true
    property bool visibleWhen: true

    id: verticalScrollBar
    width: visibleWhen ? 40 : 0
    visible: visibleWhen && flickable.contentHeight > flickable.height
    maxValue: flickable.contentHeight - flickable.height
    minValue: 0
    value: flickable.contentY
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.topMargin: 10
    anchors.bottomMargin: 10

    Connections {
        target: flickable
        onContentYChanged: {
            verticalScrollBar.value = flickable.contentY
        }
    }
    Binding {
        when: enableScrolling
        target: flickable
        property: "contentY"
        value: verticalScrollBar.value
    }
    Binding {
        target: flickable
        property: "boundsBehavior"
        value: enableScrolling ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds
    }
}
