
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
    property alias content: contentField.children
    property int contentHeight: 0
    property int contentWidth: 500
    property bool enableBackgroundClicking: true

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
    function close() {
        dialog.visible = false;
    }

    id: dialog
    color: dialog.backgroundColor
    visible: false
    anchors.fill: parent

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (enableBackgroundClicking)
                dialog.reject();
        }
    }

    MouseArea {
        anchors.left: separatorRect.left
        anchors.right: separatorRect.right
        anchors.top: titleText.top
        anchors.bottom: buttonRow.bottom
    }

    TextEdit {
        id: titleText
        color: dialog.fontColor
        text: dialog.title
        font.pixelSize: 35
        visible: dialog.title != ""
        activeFocusOnPress: false

        anchors.left: separatorRect.left
        anchors.bottom: separatorRect.top
        anchors.bottomMargin: 10
    }
    Rectangle {
        id: separatorRect
        color: dialog.fontColor
        height: 2
        width: dialog.contentWidth
        visible: dialog.title != ""

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: textText.visible ? textText.top : contentField.top
        anchors.bottomMargin: 10
    }
    TextEdit {
        id: textText
        color: dialog.fontColor
        text: dialog.text
        font.pixelSize: 25
        visible: dialog.text != ""
        width: dialog.contentWidth
        wrapMode: Text.Wrap
        activeFocusOnPress: false

        anchors.left: separatorRect.left
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: contentField.top
        anchors.bottomMargin: 10
    }
    Item {
        id: contentField
        width: dialog.contentWidth
        height: dialog.contentHeight

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: (titleText.height + separatorRect.height + (dialog.text != "" ? textText.height : 0)) / 2
    }
    Row {
        id: buttonRow
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.top: contentField.bottom
        anchors.topMargin: 10

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
