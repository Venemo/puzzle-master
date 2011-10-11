
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"
#include "util.h"

JigsawPuzzleBoard::JigsawPuzzleBoard(QObject *parent) :
    PuzzleBoard(parent),
    _tolerance(5),
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
            JigsawPuzzleItem *item = new JigsawPuzzleItem(px, 0, this);
            item->setPuzzleCoordinates(QPoint(i, j));
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
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
#endif
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        QPointF newPos(randomInt(0, originalPixmapSize().width() - _unit.width()), randomInt(0, originalPixmapSize().height() - _unit.width()));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        connect(group, SIGNAL(finished()), item, SLOT(enableMerge()));
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(randomInt(0, 359));
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
            group->addAnimation(rotateAnimation);
        }
#else
        item->setPos(newPos);
        item->enableMerge();
#endif
        if (randomInt(0, 10) > 5)
            item->raise();
    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    connect(group, SIGNAL(finished()), this, SIGNAL(gameStarted()));
    connect(group, SIGNAL(finished()), group, SLOT(deleteLater()));
    group->start();
#else
    emit gameStarted();
#endif
}

void JigsawPuzzleBoard::assemble()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    connect(group, SIGNAL(finished()), this, SIGNAL(gameEnded()));
#endif
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        item->disableMerge();
        QPointF newPos((item->scene()->width() - originalPixmapSize().width()) / 2 + (item->puzzleCoordinates().x() * _unit.width()),
                       (item->scene()->height() - originalPixmapSize().height()) / 2 + (item->puzzleCoordinates().y() * _unit.height()));
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
        QPropertyAnimation *anim = new QPropertyAnimation(item, "pos", group);
        anim->setEndValue(newPos);
        anim->setDuration(2000);
        anim->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
        group->addAnimation(anim);

        if (_allowMultitouch)
        {
            QPropertyAnimation *rotateAnimation = new QPropertyAnimation(item, "rotation", group);
            rotateAnimation->setEndValue(0);
            rotateAnimation->setDuration(2000);
            rotateAnimation->setEasingCurve(QEasingCurve(QEasingCurve::OutElastic));
            group->addAnimation(rotateAnimation);
        }
#else
        item->setPos(newPos);
        item->setRotation(0);
        emit gameEnded();
#endif
    }
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
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
