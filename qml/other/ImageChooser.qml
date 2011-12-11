import QtQuick 1.0

Rectangle {
    property string selectionColor: "blue"
    property string selectedImageUrl: null

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
            text: "Please select an image"
            anchors.centerIn: parent
            font.pixelSize: 30
            color: "#ffffff"
        }
    }

    Rectangle {
        property Rectangle selectedItemBorder: null
        property Image selectedItemImage: null

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
                        color: "white"

                        Image {
                            id: imageItem
                            asynchronous: true
                            fillMode: Image.PreserveAspectFit
                            width: 200
                            anchors.centerIn: parent
                            source: imageUrl

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (imageChooserMiddle.selectedItemBorder !== null)
                                        imageChooserMiddle.selectedItemBorder.color = "white";

                                    imageBorder.color = imageChooser.selectionColor;
                                    imageChooserMiddle.selectedItemBorder = imageBorder;
                                    imageChooserMiddle.selectedItemImage = imageItem;
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
                if (imageChooser.selectedImageUrl !== null && imageChooser.selectedImageUrl != "")
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
        title: "Hey"
        text: "You must choose an image before continuing."
        acceptButtonText: "Ok"
    }
}
