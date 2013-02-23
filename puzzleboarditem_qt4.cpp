
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

#include <QPainter>
#include <QTimer>
#include <QTouchEvent>
#include <QMap>
#include <QGraphicsSceneMouseEvent>

#include "puzzleboarditem_qt4.h"
#include "puzzle/puzzlepieceprimitive.h"
#include "puzzle/puzzlepiece.h"
#include "puzzle/puzzlegame.h"

PuzzleBoardItem::PuzzleBoardItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
#if !defined(MEEGO_EDITION_HARMATTAN) && !defined(Q_OS_SYMBIAN) && !defined(Q_OS_BLACKBERRY) && !defined(Q_OS_BLACKBERRY_TABLET)
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
#else
    setAcceptedMouseButtons(Qt::NoButton);
#endif
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setAcceptTouchEvents(true);

    _game = new PuzzleGame(this);
    _autoRepaintRequests = 0;
    _autoRepainter = new QTimer();
    _autoRepainter->setInterval(20);

    connect(_autoRepainter, SIGNAL(timeout()), this, SLOT(updateItem()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(updateGame()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateGame()));
    connect(_game, SIGNAL(animationStarting()), this, SLOT(enableAutoRepaint()));
    connect(_game, SIGNAL(animationStopped()), this, SLOT(disableAutoRepaint()));
}

void PuzzleBoardItem::updateGame()
{
    _game->setWidth(this->width());
    _game->setHeight(this->height());
}

bool PuzzleBoardItem::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd)
    {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        event->accept();
        _game->handleTouchEvent(te);
        update();
        return true;
    }

    return QDeclarativeItem::sceneEvent(event);
}

void PuzzleBoardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    _game->handleMousePress(event->button(), event->pos());
    update();
}

void PuzzleBoardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    _game->handleMouseRelease(event->button(), event->pos());
}

void PuzzleBoardItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    _game->handleMouseMove(event->pos());
    update();
}

void PuzzleBoardItem::enableAutoRepaint()
{
    _autoRepaintRequests++;

    if (!_autoRepainter->isActive())
        _autoRepainter->start();
}

void PuzzleBoardItem::disableAutoRepaint()
{
    _autoRepaintRequests--;

    if (_autoRepaintRequests == 0 && _autoRepainter->isActive())
        QTimer::singleShot(2000, _autoRepainter, SLOT(stop()));
}

void PuzzleBoardItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Save the original transform of the painter
    QTransform originalTransform = painter->transform();
    // Order the puzzle pieces by z value (ascending)
    QList<PuzzlePiece*> puzzleItems = _game->puzzleItems().toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), PuzzlePiece::puzzleItemAscLessThan);

    // Draw the pieces
    foreach (PuzzlePiece *piece, puzzleItems)
    {
        QPointF p = piece->mapToParent(QPointF(0, 0));
        QTransform transform = QTransform::fromTranslate(p.x(), p.y()).rotate(piece->rotation());
        painter->setTransform(transform);

        // Draw the strokes first
        foreach (PuzzlePiecePrimitive *p, piece->primitives())
            painter->drawPixmap(p->strokeOffset(), p->stroke());

        // Draw the actual pixmaps
        foreach (PuzzlePiecePrimitive *p, piece->primitives())
            painter->drawPixmap(p->pixmapOffset(), p->pixmap());
    }

    // Restore the original transform of the painter
    painter->setTransform(originalTransform);
}
