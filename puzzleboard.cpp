
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
#include <QTouchEvent>
#include <QMap>
#include <QGraphicsSceneMouseEvent>

#include "puzzleboard.h"
#include "puzzleitem.h"
#include "helpers/shapeprocessor.h"
#include "helpers/imageprocessor.h"

inline static bool puzzleItemLessThan(PuzzleItem *a, PuzzleItem *b)
{
    // We want to order them in descending order by their z values
    return a->zValue() > b->zValue();
}

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    _allowRotation(true),
    _strokeThickness(3),
    _tolerance(5),
    _rotationTolerance(10)
{
#if !defined(MEEGO_EDITION_HARMATTAN) && !defined(Q_OS_SYMBIAN) && !defined(Q_OS_BLACKBERRY) && !defined(Q_OS_BLACKBERRY_TABLET)
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
#else
    setAcceptedMouseButtons(Qt::NoButton);
#endif

    setAcceptTouchEvents(true);
    _mouseSubject = 0;
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
    PuzzleHelpers::ImageProcessor imageProcessor(imageUrl, QSize(width(), height()), rows, cols);

    if (!imageProcessor.isValid())
    {
        qDebug() << "pixmap is null, not starting game.";
        return false;
    }

    qDebug() << timer.elapsed() << "ms spent with processing the image";
    timer.restart();

    const PuzzleHelpers::GameDescriptor &desc = imageProcessor.descriptor();
    int *statuses = new int[cols * rows], tShape = 0, tPaint = 0;
    qreal   w0 = (width() - desc.pixmapSize.width()) / 2,
            h0 = (height() - desc.pixmapSize.height()) / 2;

    _allowRotation = allowRotation;
    _tabSizes = desc.tabOffset;
    _unit = desc.unitSize;
    memset(statuses, 0, rows * cols * sizeof(int));

    PuzzleHelpers::ShapeProcessor shapeProcessor(desc.unitSize, desc.tabFull, desc.tabSize, desc.tabOffset, desc.tabTolerance, _strokeThickness);
    PuzzleHelpers::generatePuzzlePieceStatuses(rows, cols, statuses);

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            timer.restart();

            QElapsedTimer t;
            t.start();

            // Creating the shape of the piece

            PuzzleHelpers::Correction corr = shapeProcessor.getCorrectionFor(statuses[i * rows + j]);
            int &sxCorrection = corr.sxCorrection, &syCorrection = corr.syCorrection, &xCorrection = corr.xCorrection, &yCorrection = corr.yCorrection;

            QPainterPath clip = shapeProcessor.getPuzzlePieceShape(statuses[i * rows + j])
                    .translated(xCorrection, yCorrection);;
            QPainterPath strokePath = shapeProcessor.getPuzzlePieceStrokeShape(statuses[i * rows + j])
                    .translated(xCorrection, yCorrection);

            QPainterPath realShape;
            realShape.addRect(xCorrection + desc.tabFull - 10, yCorrection + desc.tabFull - 10, desc.unitSize.width() + 20, desc.unitSize.height() + 20);
            realShape += strokePath;

            QPainterPath fakeShape;
            fakeShape.addRect(desc.tabFull - 1, desc.tabFull - 1, desc.unitSize.width() + 1 + desc.usabilityThickness * 2, desc.unitSize.height() + 1 + desc.usabilityThickness * 2);
            fakeShape.translate(xCorrection - desc.usabilityThickness, yCorrection - desc.usabilityThickness);

            tShape += t.elapsed();
            t.restart();

            // Creating the pixmap for the piece

            QPixmap px = imageProcessor.drawPiece(i, j, clip, corr);

            // Creating the stroke for the piece

            QPainter p;
            QPixmap stroke(px.width() + _strokeThickness * 2, px.height() + _strokeThickness * 2);
            stroke.fill(Qt::transparent);
            p.begin(&stroke);
            p.setRenderHint(QPainter::SmoothPixmapTransform);
            p.setRenderHint(QPainter::Antialiasing);
            p.setRenderHint(QPainter::HighQualityAntialiasing);
            p.fillPath(strokePath, QBrush(QColor(255, 255, 255, 255)));
            p.end();

            tPaint += t.elapsed();
            t.restart();

            QPointF supposed(w0 + (i * desc.unitSize.width()) + sxCorrection - desc.usabilityThickness,
                             h0 + (j * desc.unitSize.height()) + syCorrection - desc.usabilityThickness);

            // Creating the piece
            PuzzleItem *item = new PuzzleItem(px, this);
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(supposed);
            item->setPos(supposed);
            item->setPixmapOffset(QPoint(desc.usabilityThickness + 1, desc.usabilityThickness + 1));
            item->setStrokeOffset(item->pixmapOffset() - QPoint(_strokeThickness, _strokeThickness));
            item->setStroke(stroke);
            item->setFakeShape(fakeShape.translated(desc.usabilityThickness + 1, desc.usabilityThickness + 1));
            item->setRealShape(realShape.translated(desc.usabilityThickness + 1, desc.usabilityThickness + 1));
            item->setWidth(px.width() + desc.usabilityThickness * 2 + 2);
            item->setHeight(px.height() + desc.usabilityThickness * 2 + 2);
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

    qDebug() << "time spent" << "creating shapes:" << tShape << "painting:" << tPaint;

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
    qDebug() << "assemble called, number of items:" << _puzzleItems.count();
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
    QList<PuzzleItem*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), puzzleItemLessThan);
    _mouseSubject = PuzzleHelpers::findPuzzleItem(event->pos(), puzzleItems);

    if (!_mouseSubject || _mouseSubject->_isDraggingWithTouch || !_mouseSubject->_canMerge)
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
    if (!_mouseSubject || _mouseSubject->_isDraggingWithTouch || !_mouseSubject->_canMerge)
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
    if (!_mouseSubject || _mouseSubject->_isDraggingWithTouch || !_mouseSubject->_canMerge)
        return;

    QPointF p = _mouseSubject->mapFromParent(event->pos());

    if (_mouseSubject->_isRightButtonPressed && this->allowRotation())
        _mouseSubject->handleRotation(event->pos() - _mouseSubject->mapToParent(_mouseSubject->centerPoint()));
    else
        _mouseSubject->doDrag(p);

    _mouseSubject->checkMergeableSiblings(p);
}

void PuzzleBoard::touchEvent(QTouchEvent *event)
{
    // Determine which touch point belongs to which puzzle item.

    QList<PuzzleItem*> puzzleItems = _puzzleItems.toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), puzzleItemLessThan);

    foreach (const QTouchEvent::TouchPoint &p, event->touchPoints())
    {
        if (p.state() == Qt::TouchPointReleased)
        {
            //qDebug() << "released";
            foreach (PuzzleItem *item, puzzleItems)
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
            PuzzleItem *item = PuzzleHelpers::findPuzzleItem(p.pos(), puzzleItems);

            if (item)
            {
                item->_grabbedTouchPointIds.append(p.id());
                item->raise();
            }
        }
    }

    // Map touch points to their IDs (so that we don't have to look them up all the time)

    QMap<int, const QTouchEvent::TouchPoint*> m;
    foreach (const QTouchEvent::TouchPoint &p, event->touchPoints())
        m.insert(p.id(), &p);

    // For each item, handle the touch points it has grabbed

    foreach (PuzzleItem *item, puzzleItems)
    {
        int currentTouchPointCount = item->_grabbedTouchPointIds.count();
        if (currentTouchPointCount == 0 || !item->_canMerge)
        {
            if (item->_dragging)
                item->stopDrag();
            continue;
        }

        QPointF midPoint;
        foreach (int id, item->_grabbedTouchPointIds)
            midPoint += m[id]->pos();
        midPoint /= currentTouchPointCount;
        midPoint = this->QGraphicsItem::mapToItem(item, midPoint);

        item->setCompensatedTransformOriginPoint(midPoint);

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

        if (allowRotation() && currentTouchPointCount >= 2)
        {
            if (item->_previousTouchPointCount < 2)
                item->startRotation(m[item->_grabbedTouchPointIds[1]]->screenPos() - m[item->_grabbedTouchPointIds[0]]->screenPos());
            else
                item->handleRotation(m[item->_grabbedTouchPointIds[1]]->screenPos() - m[item->_grabbedTouchPointIds[0]]->screenPos());
        }

        item->_previousTouchPointCount = item->_grabbedTouchPointIds.count();
        item->checkMergeableSiblings(midPoint);
    }
}
