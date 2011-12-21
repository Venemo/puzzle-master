
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
#include <QTimer>
#include <QElapsedTimer>
#include <QApplication>

#include "puzzleboard.h"
#include "puzzleitem.h"

PuzzleBoard::PuzzleBoard(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
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
    if (pix.width() - 1 > width() || pix.width() + 1 < width())
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
    emit loadProgressChanged(0);
    deleteAllPieces();
    QApplication::instance()->processEvents();

    if (height() == 0 || height() == 0)
    {
        qDebug() << "The size of this PuzzleBoard item is not okay, not starting game.";
        return;
    }

    QElapsedTimer timer;
    timer.start();

    qDebug() << "trying to start game with" << imageUrl;
    QPixmap pixmap = processImage(imageUrl);

    if (pixmap.isNull())
    {
        qDebug() << "pixmap is null, not starting game.";
        return;
    }

    qDebug() << timer.elapsed() << "ms spent with processing the image";
    timer.restart();

    _allowMultitouch = allowMultitouch;
    _unit = QSize(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    int tabTolerance = 1, *statuses = new int[cols * rows], strokeThickness = 5, usabilityThickness = 6;
    qreal   w0 = (width() - pixmap.width()) / 2,
            h0 = (height() - pixmap.height()) / 2,
            tabSize = min<qreal>(_unit.width() / 6.0, _unit.height() / 6.0),
            tabOffset = tabSize * 2.0 / 3.0,
            tabFull = tabSize + tabOffset + tabTolerance;

    memset(statuses, 0, rows * cols * sizeof(int));

    QPainterPathStroker stroker;
    stroker.setJoinStyle(Qt::BevelJoin);

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            timer.restart();

            // Creating the shape of the piece
            QPainterPath rectClip;
            rectClip.addRect(tabFull - 1, tabFull - 1, _unit.width() + 1, _unit.height() + 1);
            QPainterPath clip = rectClip;

            int sxCorrection = 0, syCorrection = 0, xCorrection = 0, yCorrection = 0, widthCorrection = 0, heightCorrection = 0;

            // Left
            if (i > 0)
            {
                if (statuses[(i - 1) * rows + j] & PuzzleBoard::RightTab)
                {
                    QPainterPath leftBlank;
                    leftBlank.addEllipse(QPointF(tabFull + tabOffset, tabFull + _unit.height() / 2.0), tabSize, tabSize);
                    clip = clip.subtracted(leftBlank);
                    xCorrection -= tabFull;
                }
                else
                {
                    QPainterPath leftTab;
                    leftTab.addEllipse(QPointF(tabSize + tabTolerance, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(leftTab);
                    sxCorrection -= tabFull;
                    widthCorrection += tabFull;
                }
            }
            else
                xCorrection -= tabFull;

            // Top
            if (j > 0)
            {
                if (statuses[i * rows + j - 1] & PuzzleBoard::BottomTab)
                {
                    QPainterPath topBlank;
                    topBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + tabOffset), tabSize, tabSize);
                    clip = clip.subtracted(topBlank);
                    yCorrection -= tabFull;
                }
                else
                {
                    QPainterPath topTab;
                    topTab.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabSize + tabTolerance), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(topTab);
                    syCorrection -= tabFull;
                    heightCorrection += tabFull;
                }
            }
            else
                yCorrection -= tabFull;

            // Right
            if (i < cols - 1)
            {
                statuses[i * rows + j] |= randomInt(0, 1) * PuzzleBoard::RightTab;

                if (statuses[i * rows + j] & PuzzleBoard::RightTab)
                {
                    QPainterPath rightTab;
                    rightTab.addEllipse(QPointF(tabFull + _unit.width() + tabOffset, tabFull + _unit.height() / 2.0), tabSize + tabTolerance, tabSize + tabTolerance);
                    clip = clip.united(rightTab);
                    widthCorrection += tabFull;
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
                    heightCorrection += tabFull;
                }
                else
                {
                    QPainterPath bottomBlank;
                    bottomBlank.addEllipse(QPointF(tabFull + _unit.width() / 2.0, tabFull + _unit.height() - tabOffset), tabSize, tabSize);
                    clip = clip.subtracted(bottomBlank);
                }
            }

            clip = clip.translated(xCorrection, yCorrection).simplified();
            rectClip = rectClip.translated(xCorrection, yCorrection);

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

            QPointF supposed(w0 + (i * _unit.width()) + sxCorrection - usabilityThickness,
                             h0 + (j * _unit.height()) + syCorrection - usabilityThickness);
            stroker.setWidth(strokeThickness);
            QPainterPath stroke = stroker.createStroke(clip).united(clip).simplified();
            stroker.setWidth(usabilityThickness * 2);
            QPainterPath fakeShape = (stroker.createStroke(clip + rectClip) + clip + rectClip).simplified();

            // Creating the piece
            PuzzleItem *item = new PuzzleItem(px, this);
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(supposed);
            item->setPos(supposed);
            item->setPixmapOffset(QPoint(usabilityThickness, usabilityThickness));
            item->setStroke(stroke.translated(usabilityThickness, usabilityThickness));
            item->setFakeShape(fakeShape.translated(usabilityThickness, usabilityThickness));
            item->setWidth(px.width() + usabilityThickness * 2);
            item->setHeight(px.height() + usabilityThickness * 2);
            connect(item, SIGNAL(noNeighbours()), this, SLOT(assemble()));

            item->show();
            _puzzleItems.insert(item);

            qDebug() << timer.elapsed() << "ms spent with generating piece" << i * rows + j + 1 << item->puzzleCoordinates();
            emit loadProgressChanged(i * rows + j + 1);
            QApplication::instance()->processEvents();
        }
    }
    delete statuses;
    setNeighbours(cols, rows);
    emit loaded();
    QApplication::instance()->processEvents();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
}

void PuzzleBoard::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InElastic);

    foreach (PuzzleItem *item, _puzzleItems)
    {
        QPointF newPos(randomInt(0, width() - _unit.width()), randomInt(0, height() - _unit.width()));

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

    foreach (PuzzleItem *item, _puzzleItems)
    {
        item->setCanMerge(false);

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(item->supposedPosition());
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
    if (_puzzleItems.count() == 1)
        connect(group, SIGNAL(finished()), this, SIGNAL(gameWon()));
    else
        connect(group, SIGNAL(finished()), this, SIGNAL(assembleComplete()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void PuzzleBoard::accelerometerMovement(qreal x, qreal y, qreal z)
{
    Q_UNUSED(z);
    foreach (PuzzleItem *item, _puzzleItems)
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
}

void PuzzleBoard::removePuzzleItem(PuzzleItem *item)
{
    _puzzleItems.remove(item);
    item->deleteLater();
}
