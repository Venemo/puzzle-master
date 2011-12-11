
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
    property string selectionColor: "blue"
    property string selectedImageUrl: ""
    property int columnNumber: 3

    signal accepted

    id: imageChooser
    color: "black"

    Rectangle {
        id: imageChooserTop
        height: 70
        color: "#7DB72F"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        z: 1
        radius: 15

        Rectangle {
            color: "#7DB72F"
            height: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }

        Text {
            text: qsTr("Welcome! Select an image.")
            anchors.centerIn: parent
            font.pixelSize: 30
            color: "#ffffff"
        }
    }

    Rectangle {
        property Rectangle selectedItemBorder: null

        id: imageChooserMiddle
        color: "white"
        z: 0
        anchors.top: imageChooserTop.bottom
        anchors.bottom: imageChooserBottom.top
        anchors.left: parent.left
        anchors.right: parent.right

        Flickable {
            enabled: true
            contentHeight: imageSelectorGrid.height
            anchors.fill: parent
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Grid {
                id: imageSelectorGrid
                spacing: 5
                columns: imageChooser.columnNumber
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: ListModel {
                        ListElement { imageUrl: ":/image1.jpg" }
                        ListElement { imageUrl: ":/image2.jpg" }
                        ListElement { imageUrl: ":/image3.jpg" }
                        ListElement { imageUrl: ":/image4.jpg" }
                        ListElement { imageUrl: ":/image5.jpg" }

                        ListElement { imageUrl: ":/image1.jpg" }
                        ListElement { imageUrl: ":/image2.jpg" }
                        ListElement { imageUrl: ":/image3.jpg" }
                        ListElement { imageUrl: ":/image4.jpg" }
                        ListElement { imageUrl: ":/image5.jpg" }
                    }

                    delegate: Rectangle {
                        id: imageBorder
                        width: imageItem.width + 10
                        height: imageItem.height + 10
                        color: imageBorder === imageChooserMiddle.selectedItemBorder ? imageChooser.selectionColor : "white"

                        Image {
                            id: imageItem
                            asynchronous: true
                            fillMode: Image.PreserveAspectFit
                            width: imageChooser.width / imageChooser.columnNumber - 20
                            anchors.centerIn: parent
                            source: imageUrl

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    imageChooserMiddle.selectedItemBorder = imageBorder;
                                    imageChooser.selectedImageUrl = imageUrl;
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    Rectangle {
        id: imageChooserBottom
        height: 70
        color: "#7DB72F"
        z: 1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            text: "Start game"
            anchors.centerIn: parent
            onClicked: {
                if (imageChooser.selectedImageUrl != "")
                {
                    imageChooser.accepted();
                }
                else
                {
                    youMustChooseDialog.open();
                }
            }
        }
        Button {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            width: 70
            text: "..."
            onClicked: {
            }
        }
    }

    Dialog {
        id: youMustChooseDialog
        z: 2
        title: qsTr("Please choose")
        text: qsTr("You must choose an image before continuing.")
        acceptButtonText: qsTr("Ok")
    }
}
