
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
import "./components/style"

Panel {
    function startGame() {
        imageChooser.shouldStartGame = true
        imageChooser.close()
    }

    property string selectedImageUrl: ""
    property int columnNumber: 3
    property Dialog fileSelectorDialog: null

    signal accepted

    id: imageChooser
    color: "#000000"

    Component.onCompleted: {
        var selectorComponent = null

        if (allowGalleryModel) {
            console.log("ImageChooser: trying to load GallerySelectorDialog")
            selectorComponent = Qt.createComponent("GallerySelectorDialog.qml")
        }
        if (selectorComponent === null || selectorComponent.status === Component.Error || selectorComponent.status === Component.Null) {
            console.log("ImageChooser: trying to load FileSelectorDialog")
            selectorComponent = Qt.createComponent("FileSelectorDialog.qml")
        }
        if (selectorComponent === null || selectorComponent.status === Component.Error || selectorComponent.status === Component.Null) {
            console.log("Nor FileSelectorDialog neither GallerySelectorDialog could be loaded! Adding custom images will not be possible.")
        }
        else {
            fileSelectorDialog = selectorComponent.createObject(imageChooser)
        }
    }
    Rectangle {
        id: imageChooserTop
        height: 60
        color: "#7DB72F"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        radius: 15

        Rectangle {
            color: "#7DB72F"
            height: imageChooserTop.radius + 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        TextEdit {
            text: qsTr("Welcome! Choose an image.")
            anchors.centerIn: parent
            font.pixelSize: 30
            color: "#ffffff"
            activeFocusOnPress: false
        }
    }
    Rectangle {
        color: "#ffffff"
        anchors.top: imageChooserTop.bottom
        anchors.bottom: imageChooserBottom.top
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true

        GridView {
            id: imageSelectorGrid
            focus: true
            cacheBuffer: allowScrollbars ? initialSize.height * 3 : initialSize.height
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: - imageSelectorGridScrollBar.width / 2
            cellWidth: (imageChooser.width - imageSelectorGridScrollBar.width) / imageChooser.columnNumber - 5
            cellHeight: imageSelectorGrid.cellWidth / imageChooser.width * imageChooser.height
            width: imageSelectorGrid.cellWidth * imageChooser.columnNumber
            currentIndex: -1
            highlightMoveDuration: 80
            model: ListModel {
                property int initialImageCount: 0

                id: imagesModel
                Component.onCompleted: {
                    imagesModel.initialImageCount = imagesModel.count
                    var urls = appSettings.loadCustomImages()
                    for (var i = 0; i < urls.length; i++) {
                        imagesModel.insert(0, { url: urls[i] })
                    }
                }

                ListElement { url: ":/pics/image1.jpg" }
                ListElement { url: ":/pics/image2.jpg" }
                ListElement { url: ":/pics/image3.jpg" }
                ListElement { url: ":/pics/image4.jpg" }
                ListElement { url: ":/pics/image5.jpg" }
            }
            delegate: Item {
                width: imageSelectorGrid.cellWidth
                height: imageSelectorGrid.cellHeight

                Image {
                    asynchronous: false
                    fillMode: Image.PreserveAspectCrop
                    width: imageSelectorGrid.cellWidth - 10
                    height: imageSelectorGrid.cellHeight - 10
                    clip: true
                    anchors.centerIn: parent
                    source: url
                    sourceSize.width: imageSelectorGrid.cellWidth - 10
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        imageChooser.selectedImageUrl = url
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
            anchors.rightMargin: 7
        }
    }
    Rectangle {
        id: imageChooserBottom
        height: imageChooserTop.height
        color: "#7DB72F"
        radius: 15
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            color: "#7DB72F"
            height: imageChooserBottom.radius + 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
        }
        Button {
            text: qsTr("Start game")
            height: 48
            anchors.centerIn: parent
            style: GreenButtonStyle { }
            onClicked: {
                if (imageChooser.selectedImageUrl != "")
                    imageChooser.accepted()
                else
                    youMustChooseDialog.open()
            }
        }
        Button {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 6
            width: 70
            height: 48
            text: qsTr("...")
            style: GreenButtonStyle { }
            onClicked: {
                menuDialog.open()
            }
        }
    }
    Dialog {
        id: youMustChooseDialog
        title: qsTr("Please choose")
        text: qsTr("You must choose an image before continuing.")
        acceptButtonText: qsTr("Ok")
    }
    Dialog {
        id: menuDialog
        title: qsTr("Puzzle Master")
        contentHeight: menuDialogColumn.height
        contentWidth: menuDialogColumn.width
        content: Column {
            id: menuDialogColumn
            spacing: 10

            Button {
                width: 500
                text: qsTr("About")
                onClicked: {
                    menuDialog.close()
                    aboutDialog.open()
                }
            }
            Button {
                width: 500
                text: qsTr("Add custom image")
                visible: fileSelectorDialog !== null
                onClicked: {
                    menuDialog.close()
                    fileSelectorDialog.open()
                }
            }
            Button {
                width: 500
                text: qsTr("Remove custom image")
                visible: imageSelectorGrid.currentIndex >= 0 && imageSelectorGrid.currentIndex < imagesModel.count - imagesModel.initialImageCount
                onClicked: {
                    menuDialog.close()
                    appSettings.removeCustomImage(imagesModel.get(imageSelectorGrid.currentIndex).url)
                    imagesModel.remove(imagesModel.get(imageSelectorGrid.currentIndex))
                }
            }
            Button {
                width: 500
                text: qsTr("Quit")
                style: RedButtonStyle { }
                onClicked: {
                    menuDialog.close()
                    areYouSureToQuitDialog.open()
                }
            }
        }
    }
    Connections {
        target: fileSelectorDialog
        onAccepted: {
            if (appSettings.addCustomImage(fileSelectorDialog.selectedImageUrl))
                imagesModel.insert(0, { url: fileSelectorDialog.selectedImageUrl })
        }
    }
}
