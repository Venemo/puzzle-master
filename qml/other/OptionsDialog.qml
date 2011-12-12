
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

Dialog {
    property int columns: 4
    property int rows: 4

    id: optionsDialog
    title: qsTr("Game options")
    acceptButtonText: qsTr("Play")
    rejectButtonText: qsTr("Cancel")
    enableBackgroundClicking: false
    contentWidth: optionsColumn.width
    contentHeight: optionsColumn.height
    content: Column {
        id: optionsColumn
        spacing: 10
        width: 500

        Text {
            color: "white"
            font.pixelSize: 25
            text: qsTr("Columns")
        }
        Row {
            width: parent.width
            spacing: 10

            Slider {
                id: columnsSlider
                width: parent.width - 20
                minValue: 2
                maxValue: 5
                value: optionsDialog.columns
                onValueChanged: optionsDialog.columns = value
            }
            Text {
                color: "white"
                font.pixelSize: 25
                text: columnsSlider.value
                anchors.verticalCenter: columnsSlider.verticalCenter
            }
        }
        Text {
            color: "white"
            font.pixelSize: 25
            text: qsTr("Rows")
        }
        Row {
            width: parent.width
            spacing: 10

            Slider {
                id: rowsSlider
                width: parent.width - 20
                minValue: 2
                maxValue: 5
                value: optionsDialog.rows
                onValueChanged: optionsDialog.rows = value
            }
            Text {
                color: "white"
                font.pixelSize: 25
                text: rowsSlider.value
                anchors.verticalCenter: rowsSlider.verticalCenter
            }
        }
    }
}
