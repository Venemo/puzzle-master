
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
import QtMobility.gallery 1.1
import "./components"

Dialog
{
    property string selectedImageUrl: ""

    id: fileSelectorDialog
    enableBackgroundClicking: false
    title: qsTr("Select a picture")
    acceptButtonText: imageSelectorGrid.currentIndex >= 0 ? qsTr("Select") : ""
    rejectButtonText: qsTr("Cancel")
    contentWidth: imageSelectorGrid.width
    contentHeight: imageSelectorGrid.height
    content: GridView {
        id: imageSelectorGrid
        cellWidth: imageSelectorGrid.width / 4 - 5
        cellHeight: imageSelectorGrid.cellWidth / fileSelectorDialog.width * fileSelectorDialog.height
        width: fileSelectorDialog.width * 4 / 5
        height: fileSelectorDialog.height - 160
        clip: true
        focus: true
        cacheBuffer: imageSelectorGrid.height / 2
        currentIndex: -1
        highlightMoveDuration: 80

        model: DocumentGalleryModel {
            id: picsModel
            rootType: DocumentGallery.Image
            properties: [ "url" ]
            sortProperties: [ "-dateTaken" ]
            autoUpdate: true
        }
        delegate: Item {
            width: imageSelectorGrid.cellWidth
            height: imageSelectorGrid.cellHeight

            Image {
                id: imageItem
                asynchronous: true
                width: imageSelectorGrid.cellWidth - 10
                height: imageSelectorGrid.cellHeight - 10
                fillMode: Image.PreserveAspectCrop
                clip: true
                source: url
                sourceSize.width: imageSelectorGrid.cellWidth - 10
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    fileSelectorDialog.selectedImageUrl = url
                    imageSelectorGrid.currentIndex = index
                }
            }
        }
        highlight: Rectangle {
            color: "#538312"
            radius: 5
        }
    }
}