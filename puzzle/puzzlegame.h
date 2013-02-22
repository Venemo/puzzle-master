
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

#ifndef PUZZLEGAME_H
#define PUZZLEGAME_H

#include <QObject>
#include <QSize>
#include <QPoint>
#include <QPointF>
#include <QSet>

#include "util.h"

class QTouchEvent;
class PuzzlePiece;

class PuzzleGame : public QObject
{
    Q_OBJECT
    GENPROPERTY_S(bool, _enabled, enabled, setEnabled)
    GENPROPERTY_R(bool, _allowRotation, allowRotation)
    GENPROPERTY_R(int, _strokeThickness, strokeThickness)
    GENPROPERTY_S(int, _width, width, setWidth)
    GENPROPERTY_S(int, _height, height, setHeight)
    GENPROPERTY_R(QSize, _unit, unit)
    GENPROPERTY_R(qreal, _tabSizes, tabSizes)
    GENPROPERTY_F(int, _tolerance, tolerance, setTolerance, toleranceChanged)
    Q_PROPERTY(int tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)
    GENPROPERTY_F(int, _rotationTolerance, rotationTolerance, setRotationTolerance, rotationToleranceChanged)
    Q_PROPERTY(int rotationTolerance READ rotationTolerance WRITE setRotationTolerance NOTIFY rotationToleranceChanged)
    GENPROPERTY_R(QSet<PuzzlePiece*>, _puzzleItems, puzzleItems)

    QHash<PuzzlePiece*, QPair<QPointF, int> > _restorablePositions;
    PuzzlePiece *_mouseSubject;

public:
    explicit PuzzleGame(QObject *parent = 0);
    Q_INVOKABLE bool startGame(const QString &imageUrl, int rows, int cols, bool allowRotation);
    void setNeighbours(int x, int y);
    PuzzlePiece *find(const QPoint &puzzleCoordinates);
    void removePuzzleItem(PuzzlePiece *item);

    void handleMousePress(Qt::MouseButton button, QPointF pos);
    void handleMouseRelease(Qt::MouseButton button, QPointF pos);
    void handleMouseMove(QPointF pos);
    void handleTouchEvent(QTouchEvent *event);
    
signals:
    void toleranceChanged();
    void rotationToleranceChanged();

    void animationStarting();
    void animationStopped();
    void gameStarted();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);
    void shuffleComplete();
    void assembleComplete();
    void restoreComplete();
    
public slots:
    Q_INVOKABLE void disable();
    Q_INVOKABLE void enable();
    Q_INVOKABLE void shuffle();
    Q_INVOKABLE void assemble();
    Q_INVOKABLE void restore();
    Q_INVOKABLE void deleteAllPieces();

};

#endif // PUZZLEGAME_H
