
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

#ifndef PUZZLEBOARDITEM_H
#define PUZZLEBOARDITEM_H

#include <QDeclarativeItem>

#include "puzzle/puzzlegame.h"

class QTimer;
class QTouchEvent;
class QGraphicsSceneMouseEvent;

class PuzzlePiece;

class PuzzleBoardItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(PuzzleGame* game READ game NOTIFY gameChanged)

    QTimer *_autoRepainter;
    int _autoRepaintRequests;
    PuzzleGame *_game;

public:
    explicit PuzzleBoardItem(QDeclarativeItem *parent = 0);
    PuzzleGame *game() { return _game; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    bool sceneEvent(QEvent *);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

signals:
    void gameChanged();

private slots:
    void updateItem() { this->update(); }
    void updateGame();

public slots:
    void enableAutoRepaint();
    void disableAutoRepaint();
};

#endif // PUZZLEBOARDITEM_H
