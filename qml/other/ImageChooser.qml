import QtQuick 1.0

Rectangle {
    property string selectionColor: "blue"
    property string selectedImageUrl: null
    property int deselectedImageHeight: 300
    property int selectedImageHeight: 350

    signal accepted

    id: imageChooser

    Rectangle {
        id: imageChooserTop
        height: 70
        color: "white"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            text: "Please select an image"
            anchors.centerIn: parent
        }
        Rectangle {
            color: "black"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 3
        }
    }

    Rectangle {
        property Rectangle selectedItemBorder: null
        property Image selectedItemImage: null

        id: imageChooserMiddle
        color: "white"
        anchors.top: imageChooserTop.bottom
        anchors.bottom: imageChooserBottom.top
        anchors.left: parent.left
        anchors.right: parent.right

        Flickable {
            anchors.fill: parent
            enabled: true
            contentWidth: imageSelectorRow.width

            Row {
                id: imageSelectorRow
                spacing: 5
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter

                Repeater {
                    model: ListModel {
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
                        anchors.verticalCenter: parent.verticalCenter

                        Image {
                            id: imageItem
                            asynchronous: true
                            fillMode: Image.PreserveAspectFit
                            height: 300
                            anchors.centerIn: parent
                            source: imageUrl

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (imageChooserMiddle.selectedItemBorder !== null)
                                        imageChooserMiddle.selectedItemBorder.color = "white";
                                    if (imageChooserMiddle.selectedItemImage !== null)
                                        imageChooserMiddle.selectedItemImage.height = imageChooser.deselectedImageHeight;

                                    imageBorder.color = imageChooser.selectionColor;
                                    imageItem.height = imageChooser.selectedImageHeight;
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
        color: "white"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            anchors.centerIn: parent
            text: "Start game"
            onClicked: {
                if (imageChooser.selectedImageUrl !== null && imageChooser.selectedImageUrl != "")
                {
                    imageChooser.accepted();
                }
            }
        }
    }
}
