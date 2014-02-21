
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

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QString>
#include <QPixmap>
#include "helpertypes.h"

namespace Puzzle
{
namespace Creation
{

class ImageProcessorPrivate;

class ImageProcessor
{
    ImageProcessorPrivate *_p;

public:
    explicit ImageProcessor(const QString &url, const QSize &viewportSize, int rows, int cols, int strokeThickness);
    ~ImageProcessor();

    bool isValid();
    const GameDescriptor &descriptor();
    QPixmap drawPiece(int i, int j, const QPainterPath &shape, const Puzzle::Creation::Correction &corr);
    QPixmap drawStroke(const QPainterPath &strokeShape, const QSize &pxSize);

};

}
}

#endif // IMAGEPROCESSOR_H
