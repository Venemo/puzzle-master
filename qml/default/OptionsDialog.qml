
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
import "./components"

Dialog {
    property alias columns: columnsSlider.value
    property alias rows: rowsSlider.value
    property alias snapDifficulty: snapDifficultySlider.value

    id: optionsDialog
    title: qsTr("Game difficulty")
    acceptButtonText: qsTr("Play")
    rejectButtonText: qsTr("Cancel")
    enableBackgroundClicking: false
    contentWidth: optionsGrid.width
    contentHeight: optionsGrid.height
    content: Grid {
        property int leftColumnWidth: Math.max(Math.max(columnsCaption.width, rowsCaption.width), snapDifficultyCaption.width)

        id: optionsGrid
        spacing: 20 * uiScalingFactor
        width: 500 * uiScalingFactor
        columns: 2

        TextEdit {
            id: columnsCaption
            color: "#ffffff"
            font.pixelSize: 25 * textScalingFactor
            text: qsTr("Columns")
            activeFocusOnPress: false
        }
        Row {
            id: columnsRow
            width: parent.width - optionsGrid.leftColumnWidth - 20
            spacing: 10 * uiScalingFactor

            Slider {
                id: columnsSlider
                width: parent.width - 30
                minValue: 2
                maxValue: appSettings.advancedMode ? 15 : 8
                value: appSettings.columns
            }
            Binding {
                target: appSettings
                property: "columns"
                value: columnsSlider.value
            }
            TextEdit {
                id: columnsLabel
                color: "#ffffff"
                font.pixelSize: 25 * textScalingFactor
                text: columnsSlider.value
                activeFocusOnPress: false
                anchors.verticalCenter: columnsSlider.verticalCenter
            }
        }
        TextEdit {
            id: rowsCaption
            color: "#ffffff"
            font.pixelSize: 25 * textScalingFactor
            text: qsTr("Rows")
            activeFocusOnPress: false
        }
        Row {
            id: rowsRow
            width: parent.width - optionsGrid.leftColumnWidth - 20
            spacing: 10 * uiScalingFactor

            Slider {
                id: rowsSlider
                width: parent.width - 30
                minValue: 2
                maxValue: appSettings.advancedMode ? 12 : 6
                value: appSettings.rows
            }
            Binding {
                target: appSettings
                property: "rows"
                value: rowsSlider.value
            }
            TextEdit {
                id: rowsLabel
                color: "#ffffff"
                font.pixelSize: 25 * textScalingFactor
                text: rowsSlider.value
                activeFocusOnPress: false
                anchors.verticalCenter: rowsSlider.verticalCenter
            }
        }
        TextEdit {
            id: snapDifficultyCaption
            color: "#ffffff"
            font.pixelSize: 25 * textScalingFactor
            text: qsTr("Snap")
            activeFocusOnPress: false
        }
        Row {
            id: snapDifficultyRow
            width: parent.width - optionsGrid.leftColumnWidth - 20
            spacing: 10 * uiScalingFactor

            Slider {
                id: snapDifficultySlider
                width: parent.width - snapDifficultyLabel.width - 10
                minValue: 0
                maxValue: 2
                value: appSettings.snapDifficulty
            }
            Binding {
                target: appSettings
                property: "snapDifficulty"
                value: snapDifficultySlider.value
            }
            TextEdit {
                id: snapDifficultyLabel
                color: "#ffffff"
                font.pixelSize: 25 * textScalingFactor
                text: snapDifficultySlider.value == 0 ? qsTr("Easy") : (snapDifficultySlider.value == 1 ? qsTr("Normal") : qsTr("Difficult"))
                activeFocusOnPress: false
                anchors.verticalCenter: snapDifficultySlider.verticalCenter
            }
        }
        Item {
            height: 10 * uiScalingFactor
            width: 10
        }
    }
    Button {
        id: advancedDifficultyButton
        anchors {
            top: parent.top
            topMargin: 7 * uiScalingFactor
            right: parent.right
            rightMargin: 7 * uiScalingFactor
        }
        width: 65 * uiScalingFactor
        height: 45 * uiScalingFactor
        text: "..."
        onClicked: {
            advancedDifficultyMessageDialog.open();
        }
        opacity: 0.7
    }
    Dialog {
        id: advancedDifficultyMessageDialog
        title: qsTr("Isn't this a bit difficult?")
        text: qsTr("The selected options will make the game VERY difficult.\nAre you sure to proceed?")
        acceptButtonText: qsTr("Yes")
        rejectButtonText: qsTr("No")
        onAccepted: {
            appSettings.advancedMode = true;
        }
        onRejected: {
            appSettings.advancedMode = false;
        }
    }
}
