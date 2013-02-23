
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

#include "puzzleboarditem.h"

PuzzleBoardItem::PuzzleBoardItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    _game = new PuzzleGame(this);

    connect(this, SIGNAL(widthChanged()), this, SLOT(updateGame()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateGame()));
}

void PuzzleBoardItem::updateGame()
{
    _game->setWidth(this->width());
    _game->setHeight(this->height());
}

void PuzzleBoardItem::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMousePress(event->button(), event->pos());
    update();
}

void PuzzleBoardItem::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMouseRelease(event->button(), event->pos());
}

void PuzzleBoardItem::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMouseMove(event->pos());
    update();
}

void PuzzleBoardItem::touchEvent(QTouchEvent *event)
{
    event->accept();
    _game->handleTouchEvent(event);
    update();
}

QSGNode *PuzzleBoardItem::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
{
    return 0;
}
