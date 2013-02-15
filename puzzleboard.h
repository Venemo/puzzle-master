
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

#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QDeclarativeItem>
#include <QTimer>

#include "util.h"

class QTimer;
class QTouchEvent;
class QGraphicsSceneMouseEvent;
class PuzzlePiece;

class PuzzleBoard : public QDeclarativeItem
{
    Q_OBJECT
    GENPROPERTY_S(bool, _enabled, enabled, setEnabled)
    GENPROPERTY_R(bool, _allowRotation, allowRotation)
    GENPROPERTY_R(int, _strokeThickness, strokeThickness)
    GENPROPERTY_R(QSize, _unit, unit)
    GENPROPERTY_R(qreal, _tabSizes, tabSizes)
    GENPROPERTY_F(int, _tolerance, tolerance, setTolerance, toleranceChanged)
    Q_PROPERTY(int tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)
    GENPROPERTY_F(int, _rotationTolerance, rotationTolerance, setRotationTolerance, rotationToleranceChanged)
    Q_PROPERTY(int rotationTolerance READ rotationTolerance WRITE setRotationTolerance NOTIFY rotationToleranceChanged)
    GENPROPERTY_R(QSet<PuzzlePiece*>, _puzzleItems, puzzleItems)

    QHash<PuzzlePiece*, QPair<QPointF, int> > _restorablePositions;
    PuzzlePiece *_mouseSubject;
    QTimer *_autoRepainter;

public:
    explicit PuzzleBoard(QDeclarativeItem *parent = 0);
    Q_INVOKABLE bool startGame(const QString &imageUrl, int rows, int cols, bool allowRotation);
    void setNeighbours(int x, int y);
    PuzzlePiece *find(const QPoint &puzzleCoordinates);
    void removePuzzleItem(PuzzlePiece *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e);
    bool sceneEvent(QEvent *);
    void touchEvent(QTouchEvent*);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

signals:
    void toleranceChanged();
    void rotationToleranceChanged();

    void gameStarted();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);
    void shuffleComplete();
    void assembleComplete();
    void restoreComplete();

private slots:
    void enableAutoRepaint() { _autoRepainter->start(); }
    void disableAutoRepaint() { _autoRepainter->stop(); }
    void updateItem() { this->update(); }

public slots:
    Q_INVOKABLE void disable();
    Q_INVOKABLE void enable();
    Q_INVOKABLE void shuffle();
    Q_INVOKABLE void assemble();
    Q_INVOKABLE void restore();
    Q_INVOKABLE void deleteAllPieces();
};

#endif // PUZZLEBOARD_H
