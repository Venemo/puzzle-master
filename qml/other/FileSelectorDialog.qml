
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
    property int columnNumber: 5

    id: fileSelectorDialog
    title: "Select a picture"
    acceptButtonText: "Select"
    rejectButtonText: "Cancel"
    contentWidth: fileSelectorFlickable.width
    contentHeight: fileSelectorFlickable.height
    content: Flickable {
        id: fileSelectorFlickable
        enabled: true
        clip: true
        contentHeight: imageSelectorGrid.height
        width: 600
        height: fileSelectorDialog.height - 150

        Grid {
            property Rectangle selectedItemBorder: null

            id: imageSelectorGrid
            spacing: 5
            columns: fileSelectorDialog.columnNumber

            Repeater {
                model: DocumentGalleryModel {
                    id: picsModel
                    rootType: DocumentGallery.Image
                    properties: [ "url" ]
                    sortProperties: [ "-dateTaken" ]
                }
                delegate: Rectangle {
                    id: imageBorder
                    width: imageItem.width + 10
                    height: imageItem.height + 10
                    color: imageBorder === imageSelectorGrid.selectedItemBorder ? "#538312" : "transparent"

                    Image {
                        function selectItem() {
                            imageSelectorGrid.selectedItemBorder = imageBorder;
                            fileSelectorDialog.selectedImageUrl = url;
                        }

                        id: imageItem
                        asynchronous: true
                        width: fileSelectorDialog.contentWidth / fileSelectorDialog.columnNumber - 20
                        height: width / fileSelectorDialog.width * fileSelectorDialog.height
                        fillMode: Image.PreserveAspectCrop
                        clip: true
                        anchors.centerIn: parent
                        source: url
                        sourceSize.width: fileSelectorDialog.contentWidth / fileSelectorDialog.columnNumber - 20

                        MouseArea {
                            anchors.fill: parent
                            onClicked: imageItem.selectItem();
                        }
                    }
                }
            }
        }
    }
}
