
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"

JigsawPuzzleBoard::JigsawPuzzleBoard(QObject *parent) :
    PuzzleBoard(parent)
{
}

void JigsawPuzzleBoard::startGame(const QPixmap &image, unsigned rows, unsigned cols)
{
    QList<PuzzleItem*> list1;
    QList<JigsawPuzzleItem*> list2;

    QPixmap pixmap = image.scaled(width(), height(), Qt::KeepAspectRatio);
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
    PuzzleItem::setNeighbours(&list1, cols, rows);
    QTimer::singleShot(3000, this, SIGNAL(gameStarted()));
    emit loaded();
    _pixmapSize = QSize(pixmap.width(), pixmap.height());

    JigsawPuzzleItem::shuffle(&list2, pixmap.width(), pixmap.height());
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
    QList<JigsawPuzzleItem*> list2;
    foreach (QGraphicsItem *gi, items())
    {
        JigsawPuzzleItem *item = (JigsawPuzzleItem*)gi;
        item->setMerge(false);
        list2.append(item);
    }
    JigsawPuzzleItem::assemble(&list2, _pixmapSize.width(), _pixmapSize.height());
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
