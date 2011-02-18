
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"
#include "util.h"

JigsawPuzzleBoard::JigsawPuzzleBoard(QObject *parent) :
    PuzzleBoard(parent)
{
}

void JigsawPuzzleBoard::startGame(const QPixmap &image, unsigned rows, unsigned cols)
{
    QList<PuzzleItem*> list1;
    QList<JigsawPuzzleItem*> list2;

    int w = min<int>(QApplication::desktop()->width(), image.width());
    int h = min<int>(QApplication::desktop()->height(), image.height());

    QPixmap pixmap = image.scaled(w, h, Qt::KeepAspectRatio);
    setOriginalPixmapSize(pixmap.size());
    setOriginalScaleRatio(min(width() / (qreal)pixmap.width(), height() / (qreal)pixmap.height()));
    QSize unit(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // creating the pixmap for the piece
            QPixmap px(unit.width(), unit.height());
            px.fill(Qt::transparent);
            p.begin(&px);
            p.setRenderHint(QPainter::Antialiasing);

            p.drawPixmap(0, 0, pixmap, i * unit.width(), j * unit.height(), unit.width(), unit.height());
            p.end();

            // creating the piece
            JigsawPuzzleItem *item = new JigsawPuzzleItem(px, unit, 0, 0);
            item->setPuzzleCoordinates(QPoint(i, j));
            list1.append(item);
            list2.append(item);
            connect(item, SIGNAL(noNeighbours()), this, SIGNAL(gameWon()));
            addItem(item);
            item->setZValue(i * rows + j + 1);

            QPointF oldPos((width() - pixmap.width()) / 2 + (item->puzzleCoordinates().x() * unit.width()),
                           (height() - pixmap.height()) / 2 + (item->puzzleCoordinates().y() * unit.height()));
            item->setPos(oldPos);
            item->show();

            emit loadProgressChanged(i * rows + j + 1);
        }
    }
    setNeighbours(cols, rows);
    connect(this, SIGNAL(shuffleComplete()), this, SIGNAL(gameStarted()));
    emit loaded();

    QTimer::singleShot(1000, this, SLOT(shuffle()));
}

void JigsawPuzzleBoard::shuffle()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
#endif
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        QPointF newPos(randomInt(0, originalPixmapSize().width() - item->unit().width()), randomInt(0, originalPixmapSize().height() - item->unit().width()));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        connect(group, SIGNAL(finished()), item, SLOT(enableMerge()));
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);
#else
        item->setPos(newPos);
        item->enableMerge();
#endif
        if (randomInt(0, 10) > 5)
            item->raise();
    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    connect(group, SIGNAL(finished()), this, SIGNAL(shuffleComplete()));
    connect(group, SIGNAL(finished()), group, SLOT(deleteLater()));
    group->start();
#else
    emit shuffleComplete();
#endif
}

void JigsawPuzzleBoard::assemble()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    connect(group, SIGNAL(finished()), this, SIGNAL(assembleComplete()));
#endif
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        item->disableMerge();
        QPointF newPos((item->scene()->width() - originalPixmapSize().width()) / 2 + (item->puzzleCoordinates().x() * item->unit().width()),
                       (item->scene()->height() - originalPixmapSize().height()) / 2 + (item->puzzleCoordinates().y() * item->unit().height()));
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);
#else
        item->setPos(newPos);
        emit assembleComplete();
#endif
    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void JigsawPuzzleBoard::setToleranceForPieces(int tolerance)
{
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        item->setTolerance(tolerance);
    }
}

void JigsawPuzzleBoard::surrenderGame()
{
    connect(this, SIGNAL(assembleComplete()), this, SIGNAL(gameEnded()));
    assemble();
}

void JigsawPuzzleBoard::accelerometerMovement(qreal x, qreal y, qreal z)
{
    Q_UNUSED(z);
    foreach (QGraphicsItem *item, items())
    {
        JigsawPuzzleItem *widget = (JigsawPuzzleItem*) item;
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
        JigsawPuzzleItem *widget = (JigsawPuzzleItem*) item;
        widget->enableMerge();
    }
}

void JigsawPuzzleBoard::disable()
{
    foreach (QGraphicsItem *item, items())
    {
        JigsawPuzzleItem *widget = (JigsawPuzzleItem*) item;
        widget->disableMerge();
    }
}
