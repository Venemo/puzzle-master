
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

#ifndef PUZZLEPIECESHAPE_H
#define PUZZLEPIECESHAPE_H

#include <QString>
#include <QPixmap>
#include <QPainterPath>
#include <QList>
#include "helpers/helpertypes.h"

class PuzzleItem;

namespace PuzzleHelpers
{

class ShapeProcessorPrivate;

class ShapeProcessor {
    ShapeProcessorPrivate *_p;

public:
    explicit ShapeProcessor(const GameDescriptor &descriptor);
    ~ShapeProcessor();

    Correction getCorrectionFor(int status);
    QPainterPath getPuzzlePieceShape(int status);
    QPainterPath getPuzzlePieceStrokeShape(int status);
    MatchMode match(int status1, int status2);
};

void generatePuzzlePieceStatuses(unsigned rows, unsigned cols, int *statuses);
PuzzleItem *findPuzzleItem(QPointF p, const QList<PuzzleItem*> &puzzleItems);

}

#endif // PUZZLEPIECESHAPE_H
