
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
    contentWidth: optionsColumn.width
    contentHeight: optionsColumn.height
    content: Grid {
        id: optionsColumn
        spacing: 20
        width: 500
        columns: 2

        TextEdit {
            color: "#ffffff"
            font.pixelSize: 25
            text: qsTr("Columns")
            activeFocusOnPress: false
        }
        Row {
            id: columnsRow
            width: parent.width - 100
            spacing: 10

            Slider {
                id: columnsSlider
                width: parent.width - 30
                minValue: 2
                maxValue: 8
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
                font.pixelSize: 25
                text: columnsSlider.value
                activeFocusOnPress: false
                anchors.verticalCenter: columnsSlider.verticalCenter
            }
        }
        TextEdit {
            color: "#ffffff"
            font.pixelSize: 25
            text: qsTr("Rows")
            activeFocusOnPress: false
        }
        Row {
            id: rowsRow
            width: parent.width - 100
            spacing: 10

            Slider {
                id: rowsSlider
                width: parent.width - 30
                minValue: 2
                maxValue: 6
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
                font.pixelSize: 25
                text: rowsSlider.value
                activeFocusOnPress: false
                anchors.verticalCenter: rowsSlider.verticalCenter
            }
        }
        TextEdit {
            color: "#ffffff"
            font.pixelSize: 25
            text: qsTr("Snap")
            activeFocusOnPress: false
        }
        Row {
            id: snapDifficultyRow
            width: parent.width - 100
            spacing: 10

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
                font.pixelSize: 25
                text: snapDifficultySlider.value == 0 ? "Easy" : (snapDifficultySlider.value == 1 ? "Normal" : "Difficult")
                activeFocusOnPress: false
                anchors.verticalCenter: snapDifficultySlider.verticalCenter
            }
        }
    }
}
