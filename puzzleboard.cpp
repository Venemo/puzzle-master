
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

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>

#include "puzzleboard.h"
#include "puzzleitem.h"
#include "puzzlepieceshape.h"

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    _allowRotation(true),
    _strokeThickness(3),
    _tolerance(5),
    _rotationTolerance(10)
{
#if Q_OS_BLACKBERRY_TABLET
    _usabilityThickness = 25;
#else
    _usabilityThickness = 12;
#endif
}

void PuzzleBoard::setNeighbours(int x, int y)
{
    foreach (PuzzleItem *p, _puzzleItems)
    {
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzleItem *PuzzleBoard::find(const QPoint &puzzleCoordinates)
{
    foreach (PuzzleItem *p, _puzzleItems)
    {
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleBoard::startGame(const QString &imageUrl, unsigned rows, unsigned cols, bool allowRotation)
{
    emit loadProgressChanged(0);
    deleteAllPieces();
    QCoreApplication::instance()->processEvents();

    if (height() == 0 || height() == 0)
    {
        qDebug() << "The size of this PuzzleBoard item is not okay, not starting game.";
        return false;
    }

    QElapsedTimer timer;
    timer.start();

    qDebug() << "trying to start game with" << imageUrl;
    QPixmap pixmap = PuzzlePieceShape::processImage(imageUrl, width(), height());

    if (pixmap.isNull())
    {
        qDebug() << "pixmap is null, not starting game.";
        return false;
    }

    qDebug() << timer.elapsed() << "ms spent with processing the image";
    timer.restart();

    _allowRotation = allowRotation;
    _unit = QSize(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    int tabTolerance = 1, *statuses = new int[cols * rows];
    qreal   w0 = (width() - pixmap.width()) / 2,
            h0 = (height() - pixmap.height()) / 2,
            tabSize = myMin<qreal>(_unit.width() / 6.0, _unit.height() / 6.0),
            tabOffset = tabSize * 2.0 / 3.0,
            tabFull = tabSize + tabOffset + tabTolerance;

    _tabSizes = tabOffset;
    memset(statuses, 0, rows * cols * sizeof(int));

    QPainterPathStroker stroker;
    stroker.setJoinStyle(Qt::BevelJoin);

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            timer.restart();

            // Creating the shape of the piece
            PuzzlePieceShape::PuzzlePieceShapeDescriptor descriptor = PuzzlePieceShape::createPuzzlePieceShape(i, j, rows, cols, _unit, statuses, tabFull, tabSize, tabOffset, tabTolerance);
            QPainterPath &clip = descriptor.shape;
            int &sxCorrection = descriptor.sxCorrection, &syCorrection = descriptor.syCorrection, &xCorrection = descriptor.xCorrection, &yCorrection = descriptor.yCorrection, &widthCorrection = descriptor.widthCorrection, &heightCorrection = descriptor.heightCorrection;

            // Creating the pixmap for the piece
            QPixmap px(_unit.width() + widthCorrection + 1, _unit.height() + heightCorrection + 1);
            px.fill(Qt::transparent);

            // Painting the pixmap
            p.begin(&px);
            p.setRenderHint(QPainter::SmoothPixmapTransform);
            p.setRenderHint(QPainter::Antialiasing);
            p.setRenderHint(QPainter::HighQualityAntialiasing);
            p.setClipping(true);
            p.setClipPath(clip);
            p.drawPixmap(tabFull + xCorrection + sxCorrection, tabFull + yCorrection + syCorrection, pixmap, i * _unit.width() + sxCorrection, j * _unit.height() + syCorrection, _unit.width() * 2, _unit.height() * 2);
            p.end();

            QPointF supposed(w0 + (i * _unit.width()) + sxCorrection - _usabilityThickness,
                             h0 + (j * _unit.height()) + syCorrection - _usabilityThickness);
            stroker.setWidth(_strokeThickness * 2);
            QPainterPath strokePath = stroker.createStroke(clip).united(clip).simplified();

            QPainterPath bigRectClip;
            bigRectClip.addRect(tabFull - 1, tabFull - 1, _unit.width() + 1 + _usabilityThickness * 2, _unit.height() + 1 + _usabilityThickness * 2);
            bigRectClip = bigRectClip.translated(xCorrection - _usabilityThickness, yCorrection - _usabilityThickness);
            QPainterPath fakeShape = (bigRectClip + clip).simplified();

            QPixmap stroke(px.width() + _strokeThickness * 2, px.height() + _strokeThickness * 2);
            stroke.fill(Qt::transparent);
            p.begin(&stroke);
            p.fillPath(strokePath.translated(_strokeThickness, _strokeThickness), QBrush(QColor(75, 75, 75, 255)));
            p.end();

            // Creating the piece
            PuzzleItem *item = new PuzzleItem(px, this);
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(supposed);
            item->setPos(supposed);
            item->setPixmapOffset(QPoint(_usabilityThickness + 1, _usabilityThickness + 1));
            item->setStrokeOffset(item->pixmapOffset() - QPoint(_strokeThickness, _strokeThickness));
            item->setStroke(stroke);
            item->setFakeShape(fakeShape.translated(_usabilityThickness + 1, _usabilityThickness + 1));
            item->setWidth(px.width() + _usabilityThickness * 2 + 2);
            item->setHeight(px.height() + _usabilityThickness * 2 + 2);
            item->setTabStatus(statuses[i * rows + j]);
            item->setZValue(i * rows + j + this->zValue() + 1);

            connect(item, SIGNAL(noNeighbours()), this, SLOT(assemble()));

            item->show();
            _puzzleItems.insert(item);

            qDebug() << timer.elapsed() << "ms spent with generating piece" << i * rows + j + 1 << item->puzzleCoordinates();
            emit loadProgressChanged(i * rows + j + 1);
            QCoreApplication::instance()->processEvents();
        }
    }
    delete statuses;
    setNeighbours(cols, rows);
    emit loaded();
    QCoreApplication::instance()->processEvents();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
    return true;
}

void PuzzleBoard::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutElastic);
    easingCurve.setPeriod(3);
    easingCurve.setAmplitude(2.2);

    foreach (PuzzleItem *item, _puzzleItems)
    {
        int pauseDuration = randomInt(0, CLAMP(_puzzleItems.count() / 3, 2, 6)) * 300;

        QSequentialAnimationGroup *seq = new QSequentialAnimationGroup(group);
        group->addAnimation(seq);
        QParallelAnimationGroup *par = new QParallelAnimationGroup(seq);
        seq->addPause(pauseDuration);
        seq->addAnimation(par);

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setStartValue(item->pos());
        anim->setKeyValueAt(0.5, QPointF(randomInt(0, width() - _unit.width()), randomInt(0, height() - _unit.width())));
        anim->setEndValue(QPointF(randomInt(0, width() - _unit.width()), randomInt(0, height() - _unit.width())));
        anim->setDuration(3000 - pauseDuration);
        anim->setEasingCurve(easingCurve);
        par->addAnimation(anim);

        QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
        rotateAnimation->setStartValue(item->rotation());
        rotateAnimation->setEndValue(_allowRotation ? randomInt(0, 359) : 0);
        rotateAnimation->setKeyValueAt(0.5, randomInt(0, 359));
        rotateAnimation->setDuration(3000 - pauseDuration);
        rotateAnimation->setEasingCurve(easingCurve);
        par->addAnimation(rotateAnimation);

        if (randomInt(0, 1))
            item->raise();
    }

    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::assemble()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutExpo);
    _restorablePositions.clear();
    disable();

    foreach (PuzzleItem *item, _puzzleItems)
    {
        _restorablePositions[item] = QPair<QPointF, int>(item->pos(), item->rotation());

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setStartValue(item->pos());
        anim->setEndValue(item->supposedPosition());
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
        rotateAnimation->setStartValue(item->rotation());
        rotateAnimation->setEndValue(0);
        rotateAnimation->setDuration(2000);
        rotateAnimation->setEasingCurve(easingCurve);
        group->addAnimation(rotateAnimation);
    }

    if (_puzzleItems.count() == 1)
        connect(group, SIGNAL(finished()), this, SIGNAL(gameWon()));
    else
        connect(group, SIGNAL(finished()), this, SIGNAL(assembleComplete()));

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::restore()
{
    if (_restorablePositions.count() != _puzzleItems.count())
    {
        qDebug() << "The puzzle can't be restored, because something is messed up.";
        return;
    }

    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InExpo);

    foreach (PuzzleItem *item, _puzzleItems)
    {
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setStartValue(item->pos());
        anim->setEndValue(_restorablePositions[item].first);
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
        rotateAnimation->setStartValue(item->rotation());
        rotateAnimation->setEndValue(_restorablePositions[item].second);
        rotateAnimation->setDuration(2000);
        rotateAnimation->setEasingCurve(easingCurve);
        group->addAnimation(rotateAnimation);
    }

    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(restoreComplete()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::enable()
{
    foreach (PuzzleItem *item, _puzzleItems)
    {
        item->enableMerge();
    }
}

void PuzzleBoard::disable()
{
    foreach (PuzzleItem *item, _puzzleItems)
    {
        item->disableMerge();
    }
}

void PuzzleBoard::deleteAllPieces()
{
    qDeleteAll(_puzzleItems);
    _puzzleItems.clear();
    _restorablePositions.clear();
}

void PuzzleBoard::removePuzzleItem(PuzzleItem *item)
{
    _puzzleItems.remove(item);
    item->deleteLater();
}
