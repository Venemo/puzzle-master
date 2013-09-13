
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

import QtQuick 1.0
import "./components"
import "./components/style"

Panel {
    function startGame() {
        imageChooser.shouldStartGame = true
        imageChooser.close()
    }

    property string selectedImagePath: ""
    property int columnNumber: 3
    property variant fileSelectorDialog: null
    property bool canAddCustomImage: (typeof(fileSelectorDialog) != "undefined" && fileSelectorDialog !== null) || appEventHandler.showPlatformFileDialog()

    signal accepted

    id: imageChooser
    color: "#a909a7"
    radius: 15

    Keys.onEscapePressed: {
        if (areYouSureToQuitDialog.visible)
            areYouSureToQuitDialog.close();
        else
            areYouSureToQuitDialog.open();
    }

    Component.onCompleted: {
        var selectorComponent = null

        if (appEventHandler.showPlatformFileDialog()) {
            console.log("The platform's native file selector dialog will be used to add custom images");
        }
        else {
            if (allowGalleryModel) {
                console.log("ImageChooser: trying to load GallerySelectorDialog")
                selectorComponent = Qt.createComponent("GallerySelectorDialog.qml")
            }
            if (selectorComponent === null || selectorComponent.status === Component.Error || selectorComponent.status === Component.Null) {
                console.log("Nor FileSelectorDialog neither GallerySelectorDialog could be loaded! Adding custom images will not be possible.")
            }
            else {
                fileSelectorDialog = selectorComponent.createObject(imageChooser)
            }
        }
    }
    Rectangle {
        id: imageChooserTop
        height: 60 * uiScalingFactor
        color: "transparent"
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Button {
            id: appSwitcherButton
            visible: appEventHandler.showAppSwitcherButton()
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 6
            width: 65 * uiScalingFactor
            height: 45 * uiScalingFactor
            text: ""
            style: purpleButtonStyle
            onClicked: {
                appEventHandler.displayAppSwitcher()
            }
            Rectangle {
                color: "#FFFFFF"
                border.color: "#7DB72F"
                border.width: 1
                width: 25 * uiScalingFactor
                height: 25 * uiScalingFactor
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -5
                anchors.horizontalCenterOffset: -5
            }
            Rectangle {
                color: "#FFFFFF"
                border.color: "#7DB72F"
                border.width: 1
                width: 25 * uiScalingFactor
                height: 25 * uiScalingFactor
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 5
                anchors.horizontalCenterOffset: 5
            }
        }
        MenuButton {
            id: menuButton
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 7 * uiScalingFactor
            }
            width: 65 * uiScalingFactor
            height: 45 * uiScalingFactor
            style: purpleButtonStyle
            onClicked: menuDialog.open()
        }
        TextEdit {
            text: qsTr("Welcome! Choose an image.")
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            horizontalAlignment: TextEdit.AlignHCenter
            verticalAlignment: TextEdit.AlignVCenter
            font.pixelSize: 30 * textScalingFactor
            color: "#ffffff"
            activeFocusOnPress: false
        }
    }
    Item {
        anchors {
            top: imageChooserTop.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        clip: true

        Image {
            anchors.fill: parent
            source: "qrc:/pics/background.jpg"
            fillMode: Image.Tile
        }
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            gradient: Gradient {
                GradientStop { color: "#99000000"; position: 0.0 }
                GradientStop { color: "#00000000"; position: 1.0 }
            }
            height: 70
        }

        GridView {
            id: imageSelectorGrid
            focus: true
            cacheBuffer: allowScrollbars ? initialSize.height * 3 : initialSize.height
            anchors {
                top: parent.top
                bottom: parent.bottom
                topMargin: 15
                bottomMargin: 5
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: - imageSelectorGridScrollBar.width / 2
            }
            cellWidth: (imageChooser.width - imageSelectorGridScrollBar.width) / imageChooser.columnNumber - 5
            cellHeight: imageSelectorGrid.cellWidth / imageChooser.width * imageChooser.height
            width: imageSelectorGrid.cellWidth * imageChooser.columnNumber
            currentIndex: -1
            flickableDirection: Flickable.VerticalFlick
            interactive: true
            model: ListModel {
                property int initialImageCount: 0

                id: imagesModel
                Component.onCompleted: {
                    // These are the built-in images
                    if (canAddCustomImage) {
                        imagesModel.append({ path: ":/pics/add-image.png", specialThing: "add-image", isBuiltIn: true });
                    }
                    imagesModel.append({ path: ":/pics/image1.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image4.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image16.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image18.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image10.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image3.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image6.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image5.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image11.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image17.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image12.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image13.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image7.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image8.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image9.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image14.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image15.jpg", isBuiltIn: true });
                    imagesModel.append({ path: ":/pics/image2.jpg", isBuiltIn: true });

                    // Set initial count
                    imagesModel.initialImageCount = imagesModel.count;
                    // Load custom images
                    var paths = appSettings.loadCustomImages();
                    for (var i = 0; i < paths.length; i++) {
                        imagesModel.insert(canAddCustomImage ? 1 : 0, { path: paths[i] });
                    }
                }
            }
            delegate: Item {
                width: imageSelectorGrid.cellWidth
                height: imageSelectorGrid.cellHeight

                Rectangle {
                    color: "#ffffff"
                    anchors.fill: delegateImage
                    anchors.margins: -5
                    border.width: 3
                    border.color: "#33000000"
                    visible: typeof(model.specialThing) === "undefined"
                }

                Image {
                    id: delegateImage
                    asynchronous: false
                    fillMode: Image.PreserveAspectCrop
                    width: imageSelectorGrid.cellWidth - 25
                    height: imageSelectorGrid.cellHeight - 25
                    clip: true
                    anchors.centerIn: parent
                    source: appWindow.pathToUrl(path)
                    sourceSize {
                        width: imageSelectorGrid.cellWidth - 25
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {

                        var selected = imagesModel.get(index);
                        if (selected.specialThing === "add-image") {
                            menuDialog.close()
                            if (appEventHandler.showPlatformFileDialog()) {
                                appEventHandler.displayPlatformFileDialog();
                            }
                            else {
                                fileSelectorDialog.open()
                            }
                        }
                        else {
                            imageSelectorGrid.currentIndex = index;
                            imageChooser.accepted();
                        }
                    }
                }
                Button {
                    style: redButtonStyle
                    text: "-"
                    width: 50 * uiScalingFactor
                    height: 50 * uiScalingFactor
                    font.pixelSize: 25 * textScalingFactor
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }
                    visible: !model.isBuiltIn
                    onClicked: {
                        var selected = imagesModel.get(index);
                        appSettings.removeCustomImage(decodeURI(selected.path));
                        imagesModel.remove(index);
                    }
                }
            }
            onCurrentIndexChanged: {
                var selected = imagesModel.get(imageSelectorGrid.currentIndex);
                imageChooser.selectedImagePath = selected.path;
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
            spacing: 10 * uiScalingFactor

            Button {
                width: 500 * uiScalingFactor
                text: qsTr("About")
                onClicked: {
                    menuDialog.close()
                    aboutDialog.open()
                }
            }
            Button {
                width: 500 * uiScalingFactor
                text: qsTr("Quit")
                style: redButtonStyle
                onClicked: {
                    menuDialog.close()
                    areYouSureToQuitDialog.open()
                }
            }
        }
    }
    Connections {
        target: fileSelectorDialog ? fileSelectorDialog : null
        onAccepted: {
            if (appSettings.addCustomImage(decodeURI(fileSelectorDialog.selectedImagePath)))
                imagesModel.insert(canAddCustomImage ? 1 : 0, { path: fileSelectorDialog.selectedImagePath })
        }
    }
    Connections {
        target: appEventHandler
        onPlatformFileDialogAccepted: {
            if (appSettings.addCustomImage(decodeURI(filePath)))
                imagesModel.insert(canAddCustomImage ? 1 : 0, { path: filePath })
        }
    }
}
