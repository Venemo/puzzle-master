
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

#include <QTouchEvent>
#include <QDebug>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QTimer>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

#include "puzzle/puzzlegame.h"
#include "puzzle/puzzlepiece.h"
#include "puzzlepieceprimitive.h"
#include "puzzle/creation/imageprocessor.h"
#include "puzzle/creation/shapeprocessor.h"

static QPointF defaultRotationGuideCoordinates(-100, -100);

static QPointF getBottomRight(const PuzzlePiece *piece, const PuzzleGame *game)
{
    qreal r = game->tabSize() * 2 - game->tabOffset();
    QPointF f(piece->tabStatus() & Puzzle::Creation::RightTab ? r : 0, piece->tabStatus() & Puzzle::Creation::BottomTab ? r : 0);
    QPointF p = piece->bottomRight() - f;
    return p;
}

static PuzzlePiece *findPuzzleItem(QPointF p, const QList<PuzzlePiece*> &puzzleItems)
{
    foreach (PuzzlePiece *item, puzzleItems)
    {
        QPointF tr = item->mapFromParent(p);
        bool enableUsabilityImprovement = item->grabbedTouchPointIds().count();

        foreach (const PuzzlePiecePrimitive *pr, item->primitives())
        {
            QPointF pt = tr - pr->pixmapOffset();

            if (!enableUsabilityImprovement && pr->realShape().contains(pt))
                return item;
            else if (enableUsabilityImprovement && pr->fakeShape().contains(pt))
                return item;
        }
    }

    return 0;
}

PuzzleGame::PuzzleGame(QObject *parent)
    : QObject(parent)
    , _allowRotation(true)
    , _tolerance(5)
    , _rotationTolerance(10)
{
    _mouseSubject = 0;
    _strokeThickness = 3;
    _enabled = false;
    setRotationGuideCoordinates(defaultRotationGuideCoordinates);
}

void PuzzleGame::setNeighbours(int x, int y)
{
    foreach (PuzzlePiece *p, _puzzleItems)
    {
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzlePiece *PuzzleGame::find(const QPoint &puzzleCoordinates)
{
    foreach (PuzzlePiece *p, _puzzleItems)
    {
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleGame::startGame(const QString &imageUrl, int rows, int cols, bool allowRotation)
{
    emit loadProgressChanged(0);
    deleteAllPieces();
    disable();
    QCoreApplication::instance()->processEvents();

    if (height() == 0 || height() == 0)
    {
        qDebug() << "The size of this PuzzleBoardItem item is not okay, not starting game.";
        return false;
    }

    QElapsedTimer timer;
    timer.start();

    qDebug() << "trying to start game with" << imageUrl;
    Puzzle::Creation::ImageProcessor imageProcessor(imageUrl, QSize(width(), height()), rows, cols, _strokeThickness);

    if (!imageProcessor.isValid())
    {
        qDebug() << "pixmap is null, not starting game.";
        return false;
    }

    qDebug() << timer.elapsed() << "ms spent with processing the image";
    timer.restart();

    const Puzzle::Creation::GameDescriptor &desc = imageProcessor.descriptor();
    int *statuses = new int[cols * rows], tShape = 0, tPaint = 0;
    qreal   w0 = (width() - desc.cols * desc.unitSize.width()) / 2,
            h0 = (height() - desc.rows * desc.unitSize.height()) / 2;

    _allowRotation = allowRotation;
    _tabSize = desc.tabSize;
    _tabOffset = desc.tabOffset;
    _unit = desc.unitSize;

    memset(statuses, 0, rows * cols * sizeof(int));

    static int previousRows = rows, previousCols = cols, previousPixmapW = desc.pixmapSize.width(), previousPixmapH = desc.pixmapSize.height();
    static Puzzle::Creation::ShapeProcessor *shapeProcessor = new Puzzle::Creation::ShapeProcessor(desc);

    if (previousRows != rows || previousCols != cols || previousPixmapW != desc.pixmapSize.width() || previousPixmapH != desc.pixmapSize.height())
    {
        delete shapeProcessor;
        shapeProcessor = new Puzzle::Creation::ShapeProcessor(desc);
    }

    previousRows = rows, previousCols = cols, previousPixmapW = desc.pixmapSize.width(), previousPixmapH = desc.pixmapSize.height();
    shapeProcessor->resetPerfCounters();
    Puzzle::Creation::generatePuzzlePieceStatuses(rows, cols, statuses);

    emit this->newGameStarting();
    QCoreApplication::instance()->processEvents();

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            timer.restart();

            QElapsedTimer t;
            t.start();

            // Creating the shape of the piece

            Puzzle::Creation::Correction corr = shapeProcessor->getCorrectionFor(statuses[i * rows + j]);
            int &sxCorrection = corr.sxCorrection, &syCorrection = corr.syCorrection, &xCorrection = corr.xCorrection, &yCorrection = corr.yCorrection;

            // Create shapes

            QPainterPath clip = shapeProcessor->getPuzzlePieceShape(statuses[i * rows + j])
                    .translated(xCorrection, yCorrection);;
            QPainterPath strokePath = shapeProcessor->getPuzzlePieceStrokeShape(statuses[i * rows + j])
                    .translated(xCorrection, yCorrection);

            QPainterPath realShape;
            realShape.addRect(xCorrection + desc.tabFull - 10, yCorrection + desc.tabFull - 10, desc.unitSize.width() + 20, desc.unitSize.height() + 20);
            realShape += strokePath;

            QPainterPath fakeShape;
            fakeShape.addRect(desc.tabFull - 1, desc.tabFull - 1, desc.unitSize.width() + 1 + desc.usabilityThickness * 2, desc.unitSize.height() + 1 + desc.usabilityThickness * 2);
            fakeShape.translate(xCorrection - desc.usabilityThickness, yCorrection - desc.usabilityThickness);

            tShape += t.elapsed();
            t.restart();

            // Paint pixmaps

            QPixmap px = imageProcessor.drawPiece(i, j, clip, corr);
            QPixmap stroke = imageProcessor.drawStroke(strokePath, px.size());

            tPaint += t.elapsed();
            t.restart();

            QPointF supposed(w0 + (i * desc.unitSize.width()) + sxCorrection,
                             h0 + (j * desc.unitSize.height()) + syCorrection);

            // Create the puzzle piece primitive
            PuzzlePiecePrimitive *primitive = new PuzzlePiecePrimitive();
            primitive->setPixmap(px);
            primitive->setStroke(stroke);
            primitive->setPixmapOffset(QPoint(0, 0));
            primitive->setStrokeOffset(primitive->pixmapOffset() - QPoint(_strokeThickness, _strokeThickness));
            primitive->setFakeShape(fakeShape);
            primitive->setRealShape(realShape);

            // Creating the piece item
            PuzzlePiece *item = new PuzzlePiece(this);
            item->addPrimitive(primitive, QPointF(0, 0));
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(supposed);
            item->setPos(supposed);
            item->setTabStatus(statuses[i * rows + j]);
            item->setZValue(i * rows + j + 1);

            item->setTransformOriginPoint(QPointF(randomInt(0, desc.unitSize.width()), randomInt(0, desc.unitSize.height())));

            connect(item, SIGNAL(noNeighbours()), this, SLOT(assemble()));
            _puzzleItems.insert(item);

            qDebug() << timer.elapsed() << "ms spent with generating piece" << i * rows + j + 1 << item->puzzleCoordinates();
            emit loadProgressChanged(i * rows + j + 1);
            QCoreApplication::instance()->processEvents();
        }
    }

    qDebug() << "time spent" << "creating shapes:" << tShape << "painting:" << tPaint;
    shapeProcessor->printPerfCounters();

    delete statuses;
    setNeighbours(cols, rows);
    emit loaded();
    QCoreApplication::instance()->processEvents();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
    return true;
}

void PuzzleGame::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutElastic);
    int maxExplosions = MIN(_puzzleItems.count() / 4, 6);
    int maxDuration = maxExplosions * 600;
    easingCurve.setPeriod(3);
    easingCurve.setAmplitude(2.2);

    foreach (PuzzlePiece *item, _puzzleItems)
    {
        int pauseDuration = randomInt(0, maxExplosions) * 350;

        QSequentialAnimationGroup *seq = new QSequentialAnimationGroup(group);
        group->addAnimation(seq);
        QParallelAnimationGroup *par = new QParallelAnimationGroup(seq);
        seq->addPause(pauseDuration);
        seq->addAnimation(par);

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setStartValue(item->pos());
        anim->setKeyValueAt(0.5, QPointF(randomInt(_unit.width(), width() - _unit.width()), randomInt(_unit.height(), height() - _unit.height())));
        anim->setEndValue(QPointF(randomInt(_unit.width(), width() - _unit.width()), randomInt(_unit.height(), height() - _unit.height())));
        anim->setDuration(maxDuration - pauseDuration);
        anim->setEasingCurve(easingCurve);
        par->addAnimation(anim);

        QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
        rotateAnimation->setStartValue(item->rotation());
        rotateAnimation->setEndValue(_allowRotation ? randomInt(0, 359) : 0);
        rotateAnimation->setKeyValueAt(0.5, randomInt(0, 359));
        rotateAnimation->setDuration(maxDuration - pauseDuration);
        rotateAnimation->setEasingCurve(easingCurve);
        par->addAnimation(rotateAnimation);

        if (randomInt(0, 2))
            item->raise();
    }

    emit this->animationStarting();
    connect(group, SIGNAL(finished()), this, SIGNAL(animationStopped()));
    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleGame::assemble()
{
    qDebug() << "assemble called, number of items:" << _puzzleItems.count();
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutExpo);
    _restorablePositions.clear();
    disable();

    foreach (PuzzlePiece *item, _puzzleItems)
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
    {
        emit this->gameAboutToBeWon();
        connect(group, SIGNAL(finished()), this, SIGNAL(gameWon()));
    }
    else
    {
        connect(group, SIGNAL(finished()), this, SIGNAL(assembleComplete()));
    }

    emit this->animationStarting();
    connect(group, SIGNAL(finished()), this, SIGNAL(animationStopped()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleGame::restore()
{
    if (_restorablePositions.count() != _puzzleItems.count())
    {
        qDebug() << "The puzzle can't be restored, because something is messed up.";
        return;
    }

    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InExpo);

    foreach (PuzzlePiece *item, _puzzleItems)
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

    emit this->animationStarting();
    connect(group, SIGNAL(finished()), this, SIGNAL(animationStopped()));
    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(restoreComplete()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleGame::enable()
{
    setEnabled(true);
}

void PuzzleGame::disable()
{
    setEnabled(false);
}

void PuzzleGame::deleteAllPieces()
{
    qDeleteAll(_puzzleItems);
    _puzzleItems.clear();
    _restorablePositions.clear();
}

void PuzzleGame::removePuzzleItem(PuzzlePiece *item)
{
    _puzzleItems.remove(item);
    item->deleteLater();
}

void PuzzleGame::handleMousePress(Qt::MouseButton button, QPointF pos)
{
    QList<PuzzlePiece*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), PuzzlePiece::puzzleItemDescLessThan);
    _mouseSubject = findPuzzleItem(pos, puzzleItems);

    if (!_enabled || !_mouseSubject || _mouseSubject->isDraggingWithTouch())
    {
        setRotationGuideCoordinates(defaultRotationGuideCoordinates);
        return;
    }

    if (button == Qt::LeftButton)
    {
        _mouseSubject->startDrag(_mouseSubject->mapFromParent(pos));
        setRotationGuideCoordinates(_mouseSubject->mapToParent(getBottomRight(_mouseSubject, this)));
    }
    else if (button == Qt::RightButton && allowRotation())
    {
        _mouseSubject->setIsRightButtonPressed(true);
        _mouseSubject->setTransformOriginPoint(_mouseSubject->centerPoint());
        _mouseSubject->startRotation(pos - _mouseSubject->mapToParent(_mouseSubject->centerPoint()));
    }
}

void PuzzleGame::handleMouseRelease(Qt::MouseButton button, QPointF pos)
{
    if (!_mouseSubject || _mouseSubject->isDraggingWithTouch())
        return;

    if (button == Qt::LeftButton)
    {
        _mouseSubject->stopDrag();
    }
    else if (button == Qt::RightButton)
    {
        _mouseSubject->setIsRightButtonPressed(false);

        if (_mouseSubject->dragging())
            _mouseSubject->startDrag(_mouseSubject->mapFromParent(pos));
        else
            _mouseSubject->stopDrag();
    }
}

void PuzzleGame::handleMouseMove(QPointF pos)
{
    if (!_enabled || !_mouseSubject || _mouseSubject->isDraggingWithTouch())
        return;

    QPointF p = _mouseSubject->mapFromParent(pos);

    if (_mouseSubject->isRightButtonPressed() && this->allowRotation())
        _mouseSubject->handleRotation(pos - _mouseSubject->mapToParent(_mouseSubject->centerPoint()));
    else
        _mouseSubject->doDrag(p);

    setRotationGuideCoordinates(_mouseSubject->mapToParent(getBottomRight(_mouseSubject, this)));
    _mouseSubject->checkMergeableSiblings();
}

void PuzzleGame::handleTouchEvent(QTouchEvent *event)
{
    if (!_enabled)
        return;

    // Determine which touch point belongs to which puzzle item.

    QList<PuzzlePiece*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), PuzzlePiece::puzzleItemDescLessThan);

    // Iterate through the touch points in the event and assign them to an item

    QMap<int, const QTouchEvent::TouchPoint*> m;
    foreach (const QTouchEvent::TouchPoint &p, event->touchPoints())
    {
        m.insert(p.id(), &p);
        if (p.state() == Qt::TouchPointReleased)
        {
            //qDebug() << "released";
            foreach (PuzzlePiece *item, puzzleItems)
            {
                item->ungrabTouchPoint(p.id());
            }
        }
        else if (p.state() == Qt::TouchPointPressed)
        {
            //qDebug() << "pressed";
            PuzzlePiece *item = findPuzzleItem(p.pos(), puzzleItems);

            if (item)
            {
                item->grabTouchPoint(p.id());
                item->raise();
            }
        }
    }

    // For each item, handle the touch points it has grabbed

    foreach (PuzzlePiece *item, puzzleItems)
    {
        // Remove all non-existent touch points (they might exist on a glitchy touchscreen)
        foreach (int id, item->grabbedTouchPointIds())
            if (!m.contains(id))
                item->ungrabTouchPoint(id);

        // Examine the current touch point count and decide what to do
        int currentTouchPointCount = item->grabbedTouchPointIds().count();
        if (currentTouchPointCount == 0)
        {
            if (item->dragging())
                item->stopDrag();
            continue;
        }

        // Calculate the midpoint of the item
        QPointF midPoint;
        foreach (int id, item->grabbedTouchPointIds())
            if (m.contains(id))
                midPoint += m[id]->pos();
        midPoint /= currentTouchPointCount;
        midPoint = item->mapFromParent(midPoint);

        // Set the transform origin point to the midpoint
        item->setTransformOriginPoint(midPoint);

        // Perform dragging
        if (!item->dragging())
        {
            item->startDrag(midPoint, true);
        }
        else
        {
            if (item->previousTouchPointCount() != currentTouchPointCount)
                item->startDrag(item->mapToParent(midPoint) - item->pos(), true);
            item->doDrag(midPoint);
        }

        // Perform rotation
        if (allowRotation() && currentTouchPointCount >= 2)
        {
            if (item->previousTouchPointCount() < 2 || item->previousTouchPointCount() != currentTouchPointCount)
                item->startRotation(m[*(++item->grabbedTouchPointIds().begin())]->screenPos() - m[*(item->grabbedTouchPointIds().begin())]->screenPos());
            else
                item->handleRotation(m[*(++item->grabbedTouchPointIds().begin())]->screenPos() - m[*(item->grabbedTouchPointIds().begin())]->screenPos());
        }

        if (currentTouchPointCount == 1 && event->touchPoints().count() == 1)
        {
            _mouseSubject = item;
            setRotationGuideCoordinates(_mouseSubject->mapToParent(getBottomRight(_mouseSubject, this)));
        }
        else
        {
            setRotationGuideCoordinates(defaultRotationGuideCoordinates);
        }

        // Save previous touch point count
        item->setPreviousTouchPointCount(item->grabbedTouchPointIds().count());
        // Check mergeable neighbours of the piece
        item->checkMergeableSiblings();
    }
}

void PuzzleGame::startRotateWithGuide(qreal x, qreal y)
{
    _mouseSubject->setTransformOriginPoint(_mouseSubject->centerPoint());
    QPointF rp = QPointF(x, y) - _mouseSubject->mapToParent(_mouseSubject->centerPoint());
    _mouseSubject->startRotation(rp);
}

void PuzzleGame::rotateWithGuide(qreal x, qreal y)
{
    QPointF rp = QPointF(x, y) - _mouseSubject->mapToParent(_mouseSubject->centerPoint());
    _mouseSubject->handleRotation(rp);
    setRotationGuideCoordinates(_mouseSubject->mapToParent(getBottomRight(_mouseSubject, this)));
}
