
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

#ifndef HELPERTYPES_H
#define HELPERTYPES_H

namespace PuzzleHelpers
{

enum TabStatus
{
    LeftTab =      1<<0,
    LeftBlank =    1<<1,
    LeftBorder =   1<<2,

    TopTab =       1<<3,
    TopBlank =     1<<4,
    TopBorder =    1<<5,

    RightTab =     1<<6,
    RightBlank =   1<<7,
    RightBorder =  1<<8,

    BottomTab =    1<<9,
    BottomBlank =  1<<10,
    BottomBorder = 1<<11
};

enum MatchMode
{
    NoMatch = 0,
    ExactMatch,
    HorizontalFlipMatch,
    VerticalFlipMatch,
    HorizontalAndVerticalFlipMatch
};

struct Correction
{
    int sxCorrection;
    int syCorrection;
    int xCorrection;
    int yCorrection;
    int widthCorrection;
    int heightCorrection;
};

struct GameDescriptor
{
    QSize viewportSize, pixmapSize, unitSize;
    int rows, cols, tabSize, tabOffset, tabFull, tabTolerance, strokeThickness, usabilityThickness;
};

class ImageProcessor;

class ShapeProcessor;

}

#endif // HELPERTYPES_H
