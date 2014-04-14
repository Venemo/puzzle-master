
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

import QtQuick 2.0
import QtQuick.Dialogs 1.0

FileDialog {
    property variant appWindow: null
    property variant imageChooser: null
    property variant gameBoard: null

    id: filePicker
    title: qsTr("Welcome! Choose an image.")
    nameFilters: [ "Image files (*.jpg *.jpeg *.png *.bmp)", "All files (*)" ]
    onAccepted: {
        appEventHandler.activateAppWindow();

        if (appWindow.filePickerOnStart) {
            var url = imageChooser.sanitizePath(filePicker.fileUrl.toString());
            imageChooser.selectedImagePath = url;
            gameBoard.open();
            gameBoard.play();
        }
    }
    onRejected: {
        if (appWindow.filePickerOnStart) {
            console.log("User didn't choose an image, quitting.");
            Qt.quit();
        }
    }
}
