
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
import Qt.labs.folderlistmodel 1.0
import "./components"

Dialog
{
    property string selectedImageUrl: ""

    onOpened: {
        console.log("Creating folder list model");
        if (!imageSelectorGrid.model)
            imageSelectorGrid.model = Qt.createQmlObject(" \
                import QtQuick 1.0; \
                import Qt.labs.folderlistmodel 1.0; \
                 \
                FolderListModel { \
                    folder: picturesFolder; \
                    showDirs: false; \
                    showOnlyReadable: true; \
                    nameFilters: [ '*.jpg', '*.jpeg', '*.png', '*.bmp' ]; \
                }", imageSelectorGrid, "FolderListModelSnippet.qml");
    }

    id: fileSelectorDialog
    enableBackgroundClicking: false
    title: qsTr("Select a picture")
    acceptButtonText: imageSelectorGrid.currentIndex >= 0 ? qsTr("Select") : ""
    rejectButtonText: qsTr("Cancel")
    contentWidth: imageSelectorItem.width
    contentHeight: imageSelectorItem.height
    content: Item {
        id: imageSelectorItem
        width: imageSelectorGrid.width + imageSelectorGridScrollBar.width
        height: imageSelectorGrid.height

        GridView {
            id: imageSelectorGrid
            cellWidth: imageSelectorGrid.width / 4 - 5
            cellHeight: imageSelectorGrid.cellWidth / fileSelectorDialog.width * fileSelectorDialog.height
            width: fileSelectorDialog.width * 4 / 5
            height: fileSelectorDialog.height - 160
            clip: true
            focus: true
            cacheBuffer: allowScrollbars ? initialSize.height * 3 : initialSize.height
            currentIndex: -1
            highlightMoveDuration: 80
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
                    source: filePath
                    sourceSize.width: imageSelectorGrid.cellWidth - 10
                    anchors.centerIn: parent
                }
                Rectangle {
                    color: "#77000000"
                    height: 25
                    clip: true
                    anchors.left: imageItem.left
                    anchors.top: imageItem.top
                    anchors.right: imageItem.right

                    TextEdit {
                        text: fileName
                        color: "#ffffff"
                        wrapMode: Text.WrapAnywhere
                        font.pixelSize: 17
                        anchors.centerIn: parent
                    }
                }
                TextEdit {
                    text: qsTr("An error has occoured")
                    visible: imageItem.status == Image.Error
                    color: "#ffffff"
                    anchors.centerIn: parent
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        selectedImageUrl = filePath
                        imageSelectorGrid.currentIndex = index
                    }
                }
            }
            highlight: Rectangle {
                color: "#538312"
                radius: 5
            }
        }
        VerticalScrollBar {
            id: imageSelectorGridScrollBar
            visibleWhen: allowScrollbars
            enableScrolling: allowScrollbars
            flickable: imageSelectorGrid
            anchors.right: parent.right
        }
    }
}
