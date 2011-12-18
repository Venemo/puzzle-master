
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

#include <QtGui>
#include <QtDeclarative>

#include "puzzleboard.h"
#include "puzzleitem.h"

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    _originalScaleRatio(1),
    _tolerance(5),
    _rotationTolerance(10),
    _allowMultitouch(false),
    _fixedFPSTimer(0)
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer = new QtMobility::QAccelerometer(this);
    connect(accelerometer, SIGNAL(readingChanged()), this, SLOT(accelerometerReadingChanged()));
#endif
}

void PuzzleBoard::setNeighbours(int x, int y)
{
    foreach (PuzzleItem *p, puzzleItems())
    {
        if (p->puzzleCoordinates().x() != x - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(1, 0)));

        if (p->puzzleCoordinates().y() != y - 1)
            p->addNeighbour(find(p->puzzleCoordinates() + QPoint(0, 1)));
    }
}

PuzzleItem *PuzzleBoard::find(const QPoint &puzzleCoordinates)
{
    foreach (PuzzleItem *p, puzzleItems())
    {
        if (p->puzzleCoordinates() == puzzleCoordinates)
            return p;
    }
    return 0;
}

bool PuzzleBoard::isAccelerometerActive() const
{
#if defined(HAVE_QACCELEROMETER)
    return accelerometer->isActive();
#else
    return false;
#endif
}

void PuzzleBoard::accelerometerReadingChanged()
{
#if defined(HAVE_QACCELEROMETER)
    QtMobility::QAccelerometerReading *reading = accelerometer->reading();
#if defined(Q_WS_MAEMO_5)
    accelerometerMovement(reading->y(), - reading->x(), reading->z());
#elif defined(Q_OS_SYMBIAN)
    accelerometerMovement(- reading->y(), reading->x(), reading->z());
#elif defined(MEEGO_EDITION_HARMATTAN)
    accelerometerMovement(- reading->y(), reading->x(), reading->z());
#else
    accelerometerMovement(reading->x(), reading->y(), reading->z());
#endif
#endif
}

void PuzzleBoard::enableAccelerometer()
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer->connectToBackend();
    accelerometer->start();
#endif
}

void PuzzleBoard::disableAccelerometer()
{
#if defined(HAVE_QACCELEROMETER)
    accelerometer->stop();
#endif
}

void PuzzleBoard::enableFixedFPS()
{
    if (_fixedFPSTimer)
    {
        _fixedFPSTimer->stop();
        _fixedFPSTimer->deleteLater();
    }

    _fixedFPSTimer = new QTimer(this);
    _fixedFPSTimer->setInterval(20);

    foreach (QGraphicsView *view, scene()->views())
    {
        view->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
        connect(_fixedFPSTimer, SIGNAL(timeout()), view->viewport(), SLOT(update()));
    }

    _fixedFPSTimer->start();
}

void PuzzleBoard::disableFixedFPS()
{
    if (_fixedFPSTimer)
    {
        _fixedFPSTimer->stop();
        _fixedFPSTimer->deleteLater();
        _fixedFPSTimer = 0;
    }

    foreach (QGraphicsView *view, scene()->views())
    {
        view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    }
}

QPixmap PuzzleBoard::processImage(const QString &url)
{
    QString url2(url);
    url2.remove("file://");
    QPixmap pix(url2);

    if (pix.isNull())
        return pix;

    // If the image is better displayed in "portrait mode", rotate it.
    if ((pix.width() < pix.height() && width() >= height()) || (pix.width() >= pix.height() && width() < height()))
    {
        pix = pix.scaledToHeight(width());
        QPixmap pix2(pix.height(), pix.width());
        QPainter p;
        p.begin(&pix2);
        p.rotate(-90);
        p.translate(- pix2.height(), 0);
        p.drawPixmap(0, 0, pix);
        p.end();
        pix = pix2;
    }

    // Scale it to our width
    pix = pix.scaledToWidth(width());

    // If still not good enough, just crop it
    if (pix.height() > height())
    {
        QPixmap pix2(width(), height());
        QPainter p;
        p.begin(&pix2);
        p.drawPixmap(0, 0, pix, 0, (pix.height() - height()) / 2, pix.width(), pix.height());
        p.end();
        pix = pix2;
    }

    return pix;
}

void PuzzleBoard::startGame(const QString &imageUrl, unsigned rows, unsigned cols, bool allowMultitouch)
{
    if (height() == 0 || height() == 0)
    {
        qDebug() << "The size of this PuzzleBoard item is not okay, not starting game.";
        return;
    }

    qDebug() << "trying to start game with" << imageUrl;
    QPixmap pixmap = processImage(imageUrl);

    if (pixmap.isNull())
    {
        qDebug() << "pixmap is null, not starting game.";
        return;
    }

    qDeleteAll(puzzleItems());
    _puzzleItems.clear();
    _allowMultitouch = allowMultitouch;
    _originalPixmapSize = pixmap.size();
    _originalScaleRatio = 1;
    _unit = QSize(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    int tabTolerance = 2,
            *statuses = new int[cols * rows];
    qreal w0 = (width() - pixmap.width()) / 2,
            h0 = (height() - pixmap.height()) / 2,
            tabSize = min<qreal>(_unit.width() / 6.0, _unit.height() / 6.0),
            tabOffset = tabSize * 2.0 / 3.0,
            tabFull = tabSize + tabOffset + tabTolerance;

    memset(statuses, 0, rows * cols * sizeof(int));

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // Creating the pixmap for the piece
            QPixmap px(_unit.width() + tabFull * 2, _unit.height() + tabFull * 2);
            px.fill(Qt::transparent);

            // Creating the shape of the piece
            QPainterPath clip;
            clip.addRect(tabFull, tabFull, _unit.width(), _unit.height());

            int sxCorrection = 0, syCorrection = 0;

            // Left
            if (i > 0)
            {
                if (statuses[(i - 1) * rows + j] & PuzzleBoard::RightTab)
                {
                    QPainterPath leftBlank;
                    leftBlank.addEllipse(QPointF(tabFull + tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
                    clip = clip.subtracted(leftBlank);
                }
                else
                {
                    QPainterPath leftTab;
                    leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(leftTab);
                    sxCorrection -= tabFull;
                }
            }

            // Top
            if (j > 0)
            {
                if (statuses[i * rows + j - 1] & PuzzleBoard::BottomTab)
                {
                    QPainterPath topBlank;
                    topBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + tabOffset), tabSize, tabSize);
                    clip = clip.subtracted(topBlank);
                }
                else
                {
                    QPainterPath topTab;
                    topTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(topTab);
                    syCorrection -= tabFull;
                }
            }

            // Right
            if (i < cols - 1)
            {
                statuses[i * rows + j] |= randomInt(0, 1) * PuzzleBoard::RightTab;

                if (statuses[i * rows + j] & PuzzleBoard::RightTab)
                {
                    QPainterPath rightTab;
                    rightTab.addEllipse(QPointF(tabFull + _unit.width() + tabOffset, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(rightTab);
                }
                else
                {
                    QPainterPath rightBlank;
                    rightBlank.addEllipse(QPointF(tabFull + _unit.width() - tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
                    clip = clip.subtracted(rightBlank);
                }
            }

            // Bottom
            if (j < rows - 1)
            {
                statuses[i * rows + j] |= randomInt(0, 1) * PuzzleBoard::BottomTab;

                if (statuses[i * rows + j] & PuzzleBoard::BottomTab)
                {
                    QPainterPath bottomTab;
                    bottomTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() + tabOffset), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(bottomTab);
                }
                else
                {
                    QPainterPath bottomBlank;
                    bottomBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() - tabOffset), tabSize, tabSize);
                    clip = clip.subtracted(bottomBlank);
                }
            }

            clip = clip.simplified();

            p.begin(&px);
            p.setRenderHint(QPainter::SmoothPixmapTransform);
            p.setRenderHint(QPainter::Antialiasing);
            p.setRenderHint(QPainter::HighQualityAntialiasing);
            p.setClipping(true);
            p.setClipPath(clip);
            p.drawPixmap(tabFull + sxCorrection, tabFull + syCorrection, pixmap, i * _unit.width() + sxCorrection, j * _unit.height() + syCorrection, _unit.width() * 2, _unit.height() * 2);
            p.end();

            // Creating the piece
            PuzzleItem *item = new PuzzleItem(px, this);
            item->setWidth(_unit.width() + tabFull * 2);
            item->setHeight(_unit.height() + tabFull * 2);
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(QPointF(item->puzzleCoordinates().x() * _unit.width(), item->puzzleCoordinates().y() * _unit.height()));
            item->setShape(clip);
            connect(item, SIGNAL(noNeighbours()), this, SIGNAL(gameWon()));

            QPointF oldPos(w0 + (i * _unit.width()) - tabFull,
                           h0 + (j * _unit.height()) - tabFull);
            item->setPos(oldPos);
            item->show();
            _puzzleItems.append(item);

            if (i == 0 && j == 0)
                _initial00PiecePosition = oldPos;

            emit loadProgressChanged(i * rows + j + 1);
        }
    }
    delete statuses;
    setNeighbours(cols, rows);
    emit loaded();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
}

void PuzzleBoard::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InExpo);

    foreach (PuzzleItem *item, puzzleItems())
    {
        QPointF newPos(randomInt(0, originalPixmapSize().width() - _unit.width()), randomInt(0, originalPixmapSize().height() - _unit.width()));

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(randomInt(0, 359));
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(easingCurve);
            group->addAnimation(rotateAnimation);
        }
        if (randomInt(0, 1))
            item->raise();
    }

    connect(group, SIGNAL(finished()), this, SLOT(enable()));
    connect(group, SIGNAL(finished()), this, SLOT(disableFixedFPS()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::assemble()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutExpo);

    foreach (PuzzleItem *item, puzzleItems())
    {
        item->disableMerge();

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(initial00PiecePosition() + item->supposedPosition());
        anim->setDuration(2000);
        anim->setEasingCurve(easingCurve);
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(0);
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(easingCurve);
            group->addAnimation(rotateAnimation);
        }
    }

    connect(group, SIGNAL(finished()), this, SLOT(disableFixedFPS()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameEnded()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::surrenderGame()
{
    assemble();
}

void PuzzleBoard::accelerometerMovement(qreal x, qreal y, qreal z)
{
    Q_UNUSED(z);
    foreach (PuzzleItem *item, puzzleItems())
    {
        if (item->canMerge())
        {
            item->setPos(item->pos().x() - x * item->weight() / 2, item->pos().y() + y * item->weight() / 2);
            item->verifyPosition();
        }
    }
}

void PuzzleBoard::enable()
{
    foreach (PuzzleItem *item, puzzleItems())
    {
        item->enableMerge();
    }
}

void PuzzleBoard::disable()
{
    foreach (PuzzleItem *item, puzzleItems())
    {
        item->disableMerge();
    }
}

QList<PuzzleItem*> PuzzleBoard::puzzleItems()
{
    QList<PuzzleItem*> result;
    foreach (const QPointer<PuzzleItem> &ptr, _puzzleItems)
    {
        if (ptr.isNull())
            _puzzleItems.removeAll(ptr);
        else
            result.append(ptr.data());
    }
    return result;
}
