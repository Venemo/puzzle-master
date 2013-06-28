
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

Panel {
    property string backgroundColor: "#ee101010"
    property string fontColor: "#ffffff"
    property string title: ""
    property string text: ""
    property string acceptButtonText: ""
    property string rejectButtonText: ""
    property alias content: contentField.children
    property int contentHeight: 0
    property int contentWidth: 500 * uiScalingFactor
    property bool enableBackgroundClicking: true
    property bool shouldAccept: false

    signal rejected
    signal accepted

    function reject() {
        shouldAccept = false
        close()
    }
    function accept() {
        shouldAccept = true
        close()
    }

    id: dialog
    color: dialog.backgroundColor
    visible: false
    opacity: 0
    onClosed: {
        if (shouldAccept)
            dialog.accepted()
        else
            dialog.rejected()
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (enableBackgroundClicking)
                dialog.reject()
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
        font.pixelSize: 35 * textScalingFactor
        visible: dialog.title != ""
        activeFocusOnPress: false

        anchors.left: separatorRect.left
        anchors.bottom: separatorRect.top
        anchors.bottomMargin: 10 * uiScalingFactor
    }
    Rectangle {
        id: separatorRect
        color: dialog.fontColor
        height: 2
        width: dialog.contentWidth
        visible: dialog.title != ""

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: textText.visible ? textText.top : contentField.top
        anchors.bottomMargin: 10 * uiScalingFactor
    }
    TextEdit {
        id: textText
        color: dialog.fontColor
        text: dialog.text
        font.pixelSize: 25 * textScalingFactor
        visible: dialog.text != ""
        width: dialog.contentWidth
        wrapMode: Text.Wrap
        activeFocusOnPress: false

        anchors.left: separatorRect.left
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: contentField.top
        anchors.bottomMargin: 10 * uiScalingFactor
    }
    Item {
        id: contentField
        width: dialog.contentWidth
        height: dialog.contentHeight

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: uiScalingFactor * (titleText.height + separatorRect.height + (dialog.text != "" ? textText.height : 0)) / 2 - 30
    }
    Row {
        id: buttonRow
        spacing: 10 * uiScalingFactor

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: contentField.bottom
        anchors.topMargin: 10 * uiScalingFactor

        Button {
            id: acceptButton
            text: acceptButtonText
            width: 240 * uiScalingFactor
            onClicked: dialog.accept()
            visible: acceptButtonText != ""
            style: GreenButtonStyle { }
        }
        Button {
            id: rejectButton
            text: rejectButtonText
            width: 240 * uiScalingFactor
            onClicked: dialog.reject()
            visible: rejectButtonText != ""
            style: RedButtonStyle { }
        }
    }
}
