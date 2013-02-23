
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

QtObject {
    property Gradient normalGradient: Gradient {
        GradientStop { position: 0; color: "white"; }
        GradientStop { position: 1; color: "#dddddd"; }
    }
    property Gradient pressedGradient: Gradient {
        GradientStop { position: 0; color: "white"; }
        GradientStop { position: 1; color: "white"; }
    }
    property string borderColor: "#b7b7b7"
    property string fontColor: "black"
}
