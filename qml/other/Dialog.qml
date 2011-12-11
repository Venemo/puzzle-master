
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
    property string backgroundColor: "#ee101010"
    property string fontColor: "#ffffff"
    property string title: ""
    property string text: ""
    property string acceptButtonText: ""
    property string rejectButtonText: ""

    signal rejected
    signal accepted

    function reject() {
        dialog.visible = false;
        dialog.rejected();
    }

    function accept() {
        dialog.visible = false;
        dialog.accepted();
    }

    function open() {
        dialog.visible = true;
    }

    id: dialog
    color: dialog.backgroundColor
    visible: false
    anchors.fill: parent

    MouseArea {
        anchors.fill: parent
        onClicked: dialog.reject();
    }

    MouseArea {
        anchors.fill: dialogColumn
    }

    Column {
        id: dialogColumn
        spacing: 10
        anchors.centerIn: parent

        Text {
            color: dialog.fontColor
            text: dialog.title
            font.pixelSize: 35
            visible: dialog.title != ""
        }
        Rectangle {
            color: dialog.fontColor
            height: 2
            width: parent.width
            visible: dialog.title != ""
        }
        Text {
            color: dialog.fontColor
            text: dialog.text
            font.pixelSize: 25
        }
        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: acceptButton
                text: acceptButtonText
                width: 170
                onClicked: dialog.accept();
                visible: acceptButtonText != ""
            }
            Button {
                id: rejectButton
                text: rejectButtonText
                width: 170
                onClicked: dialog.reject();
                normalGradient: Gradient {
                    GradientStop { position: 0; color: "#ED1C24"; }
                    GradientStop { position: 1; color: "#AA1317"; }
                }
                pressedGradient: Gradient {
                    GradientStop { position: 0; color: "#AA1317"; }
                    GradientStop { position: 1; color: "#AA1317"; }
                }
                borderColor: "#980C10"
                visible: rejectButtonText != ""
            }
        }
    }

}
