
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

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QTouchEvent>
#include <QMap>
#include <QGraphicsSceneMouseEvent>

#include "puzzleboard.h"
#include "helpers/shapeprocessor.h"
#include "helpers/imageprocessor.h"
#include "puzzle/puzzlepieceprimitive.h"
#include "puzzle/puzzlepiece.h"

inline static bool puzzleItemDescLessThan(PuzzlePiece *a, PuzzlePiece *b)
{
    // We want to order them in descending order by their z values
    return a->zValue() > b->zValue();
}
inline static bool puzzleItemAscLessThan(PuzzlePiece *a, PuzzlePiece *b)
{
    // We want to order them in ascending order by their z values
    return a->zValue() < b->zValue();
}

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    _allowRotation(true),
    _tolerance(5),
    _rotationTolerance(10)
{
#if !defined(MEEGO_EDITION_HARMATTAN) && !defined(Q_OS_SYMBIAN) && !defined(Q_OS_BLACKBERRY) && !defined(Q_OS_BLACKBERRY_TABLET)
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
#else
    setAcceptedMouseButtons(Qt::NoButton);
#endif
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    setAcceptTouchEvents(true);
    _mouseSubject = 0;
    _strokeThickness = 3;

    _autoRepainter = new QTimer();
    _autoRepainter->setInterval(20);
    connect(_autoRepainter, SIGNAL(timeout()), this, SLOT(updateItem()));
}

void PuzzleBoard::setNeighbours(int x, int y)
{
    foreach (PuzzlePiece *p, _puzzleItems)
    {
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzlePiece *PuzzleBoard::find(const QPoint &puzzleCoordinates)
{
    foreach (PuzzlePiece *p, _puzzleItems)
    {
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleBoard::startGame(const QString &imageUrl, int rows, int cols, bool allowRotation)
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
    PuzzleHelpers::ImageProcessor imageProcessor(imageUrl, QSize(width(), height()), rows, cols, _strokeThickness);

    if (!imageProcessor.isValid())
    {
        qDebug() << "pixmap is null, not starting game.";
        return false;
    }

    qDebug() << timer.elapsed() << "ms spent with processing the image";
    timer.restart();

    const PuzzleHelpers::GameDescriptor &desc = imageProcessor.descriptor();
    int *statuses = new int[cols * rows], tShape = 0, tPaint = 0;
    qreal   w0 = (width() - desc.cols * desc.unitSize.width()) / 2,
            h0 = (height() - desc.rows * desc.unitSize.height()) / 2;

    _allowRotation = allowRotation;
    _tabSizes = desc.tabOffset;
    _unit = desc.unitSize;

    memset(statuses, 0, rows * cols * sizeof(int));

    static int previousRows = rows, previousCols = cols, previousPixmapW = desc.pixmapSize.width(), previousPixmapH = desc.pixmapSize.height();
    static PuzzleHelpers::ShapeProcessor *shapeProcessor = new PuzzleHelpers::ShapeProcessor(desc);

    if (previousRows != rows || previousCols != cols || previousPixmapW != desc.pixmapSize.width() || previousPixmapH != desc.pixmapSize.height())
    {
        delete shapeProcessor;
        shapeProcessor = new PuzzleHelpers::ShapeProcessor(desc);
    }

    previousRows = rows, previousCols = cols, previousPixmapW = desc.pixmapSize.width(), previousPixmapH = desc.pixmapSize.height();
    shapeProcessor->resetPerfCounters();
    PuzzleHelpers::generatePuzzlePieceStatuses(rows, cols, statuses);

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            timer.restart();

            QElapsedTimer t;
            t.start();

            // Creating the shape of the piece

            PuzzleHelpers::Correction corr = shapeProcessor->getCorrectionFor(statuses[i * rows + j]);
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
            item->setZValue(i * rows + j + this->zValue() + 1);

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

void PuzzleBoard::shuffle()
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

        if (randomInt(0, 1))
            item->raise();
    }

    enableAutoRepaint();
    connect(group, SIGNAL(finished()), this, SLOT(disableAutoRepaint()));
    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::assemble()
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
        connect(group, SIGNAL(finished()), this, SIGNAL(gameWon()));
    else
        connect(group, SIGNAL(finished()), this, SIGNAL(assembleComplete()));

    enableAutoRepaint();
    connect(group, SIGNAL(finished()), this, SLOT(disableAutoRepaint()));
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

    enableAutoRepaint();
    connect(group, SIGNAL(finished()), this, SLOT(disableAutoRepaint()));
    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SIGNAL(restoreComplete()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::enable()
{
    setEnabled(true);
}

void PuzzleBoard::disable()
{
    setEnabled(false);
}

void PuzzleBoard::deleteAllPieces()
{
    qDeleteAll(_puzzleItems);
    _puzzleItems.clear();
    _restorablePositions.clear();
}

void PuzzleBoard::removePuzzleItem(PuzzlePiece *item)
{
    _puzzleItems.remove(item);
    item->deleteLater();
}

bool PuzzleBoard::sceneEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd)
    {
        QTouchEvent *te = static_cast<QTouchEvent*>(event);
        event->accept();
        touchEvent(te);
        return true;
    }

    return QDeclarativeItem::sceneEvent(event);
}

void PuzzleBoard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    QList<PuzzlePiece*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), puzzleItemAscLessThan);
    _mouseSubject = PuzzleHelpers::findPuzzleItem(event->pos(), puzzleItems);

    if (!_enabled || !_mouseSubject || _mouseSubject->_isDraggingWithTouch)
        return;

    if (event->button() == Qt::LeftButton)
    {
        _mouseSubject->startDrag(_mouseSubject->mapFromParent(event->pos()));
    }
    else if (event->button() == Qt::RightButton && allowRotation())
    {
        _mouseSubject->_isRightButtonPressed = true;
        _mouseSubject->setCompensatedTransformOriginPoint(_mouseSubject->centerPoint());
        _mouseSubject->startRotation(event->pos() - _mouseSubject->mapToParent(_mouseSubject->centerPoint()));
    }
}

void PuzzleBoard::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    if (!_mouseSubject || _mouseSubject->_isDraggingWithTouch)
        return;

    if (event->button() == Qt::LeftButton)
    {
        _mouseSubject->stopDrag();
    }
    else if (event->button() == Qt::RightButton)
    {
        _mouseSubject->_isRightButtonPressed = false;
        _mouseSubject->_dragStart = event->pos() - _mouseSubject->pos();
    }
}

void PuzzleBoard::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    if (!_enabled || !_mouseSubject || _mouseSubject->_isDraggingWithTouch)
        return;

    QPointF p = _mouseSubject->mapFromParent(event->pos());

    if (_mouseSubject->_isRightButtonPressed && this->allowRotation())
        _mouseSubject->handleRotation(event->pos() - _mouseSubject->mapToParent(_mouseSubject->centerPoint()));
    else
        _mouseSubject->doDrag(p);

    _mouseSubject->checkMergeableSiblings();
    update();
}

void PuzzleBoard::touchEvent(QTouchEvent *event)
{
    if (!_enabled)
        return;

    // Determine which touch point belongs to which puzzle item.

    QList<PuzzlePiece*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), puzzleItemDescLessThan);

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
                if (item->_grabbedTouchPointIds.contains(p.id()))
                {
                    item->_grabbedTouchPointIds.removeAll(p.id());
                    break;
                }
            }
        }
        else if (p.state() == Qt::TouchPointPressed)
        {
            //qDebug() << "pressed";
            PuzzlePiece *item = PuzzleHelpers::findPuzzleItem(p.pos(), puzzleItems);

            if (item)
            {
                item->_grabbedTouchPointIds.append(p.id());
                item->raise();
            }
        }
    }

    // For each item, handle the touch points it has grabbed

    foreach (PuzzlePiece *item, puzzleItems)
    {
        // Remove all non-existent touch points (they might exist on a glitchy touchscreen)
        foreach (int id, item->_grabbedTouchPointIds)
            if (!m.contains(id))
                item->_grabbedTouchPointIds.removeAll(id);

        // Examine the current touch point count and decide what to do
        int currentTouchPointCount = item->_grabbedTouchPointIds.count();
        if (currentTouchPointCount == 0)
        {
            if (item->_dragging)
                item->stopDrag();
            continue;
        }

        // Calculate the midpoint of the item
        QPointF midPoint;
        foreach (int id, item->_grabbedTouchPointIds)
            if (m.contains(id))
                midPoint += m[id]->pos();
        midPoint /= currentTouchPointCount;
        midPoint = item->mapFromParent(midPoint);

        // Set the transform origin point to the midpoint
        item->setCompensatedTransformOriginPoint(midPoint);

        // Perform dragging
        if (!item->_dragging)
        {
            item->_isDraggingWithTouch = true;
            item->startDrag(midPoint);
        }
        else
        {
            if (item->_previousTouchPointCount != currentTouchPointCount)
                item->_dragStart = mapToParent(midPoint) - pos();
            item->doDrag(midPoint);
        }

        // Perform rotation
        if (allowRotation() && currentTouchPointCount >= 2)
        {
            if (item->_previousTouchPointCount < 2)
                item->startRotation(m[item->_grabbedTouchPointIds[1]]->screenPos() - m[item->_grabbedTouchPointIds[0]]->screenPos());
            else
                item->handleRotation(m[item->_grabbedTouchPointIds[1]]->screenPos() - m[item->_grabbedTouchPointIds[0]]->screenPos());
        }

        // Save previous touch point count
        item->_previousTouchPointCount = item->_grabbedTouchPointIds.count();
        // Check mergeable neighbours of the piece
        item->checkMergeableSiblings();
    }

    // Schedule a repaint
    update();
}

void PuzzleBoard::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Save the original transform of the painter
    QTransform originalTransform = painter->transform();
    // Order the puzzle pieces by z value (ascending)
    QList<PuzzlePiece*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), puzzleItemAscLessThan);

    // Draw the pieces
    foreach (PuzzlePiece *piece, puzzleItems)
    {
        QPointF p = piece->mapToParent(QPointF(0, 0));
        QTransform transform = QTransform::fromTranslate(p.x(), p.y()).rotate(piece->rotation());
        painter->setTransform(transform);

        // Draw the strokes first
        foreach (PuzzlePiecePrimitive *p, piece->_primitives)
            painter->drawPixmap(p->strokeOffset(), p->stroke());

        // Draw the actual pixmaps
        foreach (PuzzlePiecePrimitive *p, piece->_primitives)
            painter->drawPixmap(p->pixmapOffset(), p->pixmap());
    }

    // Restore the original transform of the painter
    painter->setTransform(originalTransform);
}
