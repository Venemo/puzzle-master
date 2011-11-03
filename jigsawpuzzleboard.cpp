
#include <QtGui>
#include <QtDeclarative>

#include "jigsawpuzzleboard.h"
#include "puzzleitem.h"
#include "util.h"

JigsawPuzzleBoard::JigsawPuzzleBoard(QObject *parent) :
    PuzzleBoard(parent),
    _tolerance(5),
    _rotationTolerance(10),
    _allowMultitouch(false)
{
    setItemIndexMethod(NoIndex);
}

void JigsawPuzzleBoard::startGame(const QPixmap &image, unsigned rows, unsigned cols, bool allowMultitouch)
{
    int w = min<int>(width(), image.width());
    int h = min<int>(height(), image.height());

    _allowMultitouch = allowMultitouch;

    QPixmap pixmap = image.scaled(w, h, Qt::KeepAspectRatio);
    setOriginalPixmapSize(pixmap.size());
    //setOriginalScaleRatio(min(width() / (qreal)pixmap.width(), height() / (qreal)pixmap.height()));
    setOriginalScaleRatio(1);
    setSceneRect(0, 0, width() / originalScaleRatio(), height() / originalScaleRatio());
    _unit = QSize(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    qreal w0 = (width() - pixmap.width()) / 2;
    qreal h0 = (height() - pixmap.height()) / 2;

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // creating the pixmap for the piece
            QPixmap px(_unit.width(), _unit.height());
            px.fill(Qt::transparent);
            p.begin(&px);
            p.setRenderHint(QPainter::HighQualityAntialiasing);
            p.drawPixmap(0, 0, pixmap, i * _unit.width(), j * _unit.height(), _unit.width(), _unit.height());
            p.end();

            // creating the piece
            PuzzleItem *item = new PuzzleItem(px);
            item->setPuzzleCoordinates(QPoint(i, j));
            item->setSupposedPosition(item->puzzleCoordinates() * _unit);
            connect(item, SIGNAL(noNeighbours()), this, SIGNAL(gameWon()));
            item->setZValue(i * rows + j + 1);

            QPointF oldPos(w0 + (i * _unit.width()),
                           h0 + (j * _unit.height()));
            item->setPos(oldPos);
            item->show();

            emit loadProgressChanged(i * rows + j + 1);
        }
    }
    setNeighbours(cols, rows);
    emit loaded();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
}

void JigsawPuzzleBoard::shuffle()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::InExpo);

    foreach (QGraphicsItem *gi, items())
    {
        PuzzleItem *item = (PuzzleItem*)gi;
        QPointF newPos(randomInt(0, originalPixmapSize().width() - _unit.width()), randomInt(0, originalPixmapSize().height() - _unit.width()));

        connect(group, SIGNAL(finished()), item, SLOT(enableMerge()));
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
        if (randomInt(0, 10) > 5)
            item->raise();
    }

    connect(group, SIGNAL(finished()), this, SLOT(disableFixedFPS()));
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    enableFixedFPS();
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void JigsawPuzzleBoard::assemble()
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    QEasingCurve easingCurve(QEasingCurve::OutExpo);

    foreach (QGraphicsItem *gi, items())
    {
        PuzzleItem *item = (PuzzleItem*)gi;
        item->disableMerge();
        QPointF newPos((item->scene()->width() - originalPixmapSize().width()) / 2 + (item->puzzleCoordinates().x() * _unit.width()),
                       (item->scene()->height() - originalPixmapSize().height()) / 2 + (item->puzzleCoordinates().y() * _unit.height()));

        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
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

void JigsawPuzzleBoard::surrenderGame()
{
    assemble();
}

void JigsawPuzzleBoard::accelerometerMovement(qreal x, qreal y, qreal z)
{
    Q_UNUSED(z);
    foreach (QGraphicsItem *item, items())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        if (widget->canMerge())
        {
            widget->setPos(widget->pos().x() - x * widget->weight() / 2, widget->pos().y() + y * widget->weight() / 2);
            widget->verifyPosition();
        }
    }
}

void JigsawPuzzleBoard::enable()
{
    foreach (QGraphicsItem *item, items())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        widget->enableMerge();
    }
}

void JigsawPuzzleBoard::disable()
{
    foreach (QGraphicsItem *item, items())
    {
        PuzzleItem *widget = (PuzzleItem*) item;
        widget->disableMerge();
    }
}
