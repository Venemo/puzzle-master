
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

#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QDeclarativeItem>

#if defined(HAVE_QACCELEROMETER)
#include <QAccelerometer>
#endif

#include "util.h"

class QTimer;
class PuzzleItem;

class PuzzleBoard : public QDeclarativeItem
{
    Q_OBJECT
    GENPROPERTY_R(bool, _allowRotation, allowRotation)
    GENPROPERTY_R(int, _usabilityThickness, usabilityThickness)
    GENPROPERTY_R(int, _strokeThickness, strokeThickness)
    GENPROPERTY_R(QSize, _unit, unit)
    GENPROPERTY_R(qreal, _tabSizes, tabSizes)
    GENPROPERTY_F(int, _tolerance, tolerance, setTolerance, toleranceChanged)
    Q_PROPERTY(int tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)
    GENPROPERTY_F(int, _rotationTolerance, rotationTolerance, setRotationTolerance, rotationToleranceChanged)
    Q_PROPERTY(int rotationTolerance READ rotationTolerance WRITE setRotationTolerance NOTIFY rotationToleranceChanged)
    Q_PROPERTY(bool isAccelerometerActive READ isAccelerometerActive NOTIFY isAccelerometerActiveChanged)

    QTimer *_fixedFPSTimer;
    QSet<PuzzleItem*> _puzzleItems;
    QHash<PuzzleItem*, QPair<QPointF, int> > _restorablePositions;
#if defined(HAVE_QACCELEROMETER)
    QtMobility::QAccelerometer *accelerometer;
#endif

public:
    explicit PuzzleBoard(QDeclarativeItem *parent = 0);
    Q_INVOKABLE void startGame(const QString &imageUrl, unsigned rows, unsigned cols, bool allowRotation);
    void setNeighbours(int x, int y);
    PuzzleItem *find(const QPoint &puzzleCoordinates);
    void removePuzzleItem(PuzzleItem *item);
    bool isAccelerometerActive() const;
    QPixmap processImage(const QString &url);

protected:
    void accelerometerMovement(qreal x, qreal y, qreal z);

signals:
    void isAccelerometerActiveChanged();
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
    void accelerometerReadingChanged();

public slots:
    Q_INVOKABLE void disable();
    Q_INVOKABLE void enable();
    Q_INVOKABLE void enableAccelerometer();
    Q_INVOKABLE void disableAccelerometer();
    Q_INVOKABLE void enableFixedFPS();
    Q_INVOKABLE void disableFixedFPS();
    Q_INVOKABLE void shuffle();
    Q_INVOKABLE void assemble();
    Q_INVOKABLE void restore();
    Q_INVOKABLE void deleteAllPieces();
};

#endif // PUZZLEBOARD_H
