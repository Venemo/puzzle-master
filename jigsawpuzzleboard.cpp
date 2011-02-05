

#include "jigsawpuzzleboard.h"

JigsawPuzzleBoard::JigsawPuzzleBoard(QObject *parent) :
    PuzzleBoard(parent)
{
}

void JigsawPuzzleBoard::startGame(const QPixmap &image, unsigned rows, unsigned cols)
{
    QList<PuzzlePiece*> list1;
    QList<JigsawPuzzleItem*> list2;

    QPixmap pixmap = image.scaled(width(), height(), Qt::KeepAspectRatio);
    QSize unit(pixmap.width() / cols, pixmap.height() / rows);
    QPainter p;

    for (unsigned i = 0; i < cols; i++)
    {
        for (unsigned j = 0; j < rows; j++)
        {
            // creating the pixmap for the piece
//            QPixmap px(pixmap.width(), pixmap.height());
//            px.fill(Qt::transparent);
//            p.begin(&px);
//            p.setRenderHint(QPainter::Antialiasing);

//            QPainterPath clip;
//            clip.addRect(i * unit.width(), j * unit.height(), unit.width(), unit.height());
//            p.setClipPath(clip);
//            p.setClipping(true);

//            p.drawPixmap(0, 0, pixmap);
//            p.end();

            QPixmap px(unit.width(), unit.height());
            px.fill(Qt::transparent);
            p.begin(&px);
            p.setRenderHint(QPainter::Antialiasing);

            p.drawPixmap(0, 0, pixmap, i * unit.width(), j * unit.height(), unit.width(), unit.height());
            p.end();

            // creating the piece
            JigsawPuzzleItem *w = new JigsawPuzzleItem(px, unit, 0, 0);
            w->setPosition(QPoint(i, j));
            list1.append(w);
            list2.append(w);
            connect(w, SIGNAL(noNeighbours()), this, SIGNAL(gameWon()));
            addItem(w);
            w->show();
            w->setZValue(i * rows + j + 1);
            emit loadProgressChanged(i * rows + j + 1);
        }
    }
    PuzzlePiece::setNeighbours(&list1, cols, rows);
    emit gameStarted();

    JigsawPuzzleItem::shuffle(&list2, cols, rows, width() - unit.width() - 1, height() - unit.height() - 1);
}

void JigsawPuzzleBoard::surrenderGame()
{
    foreach (QGraphicsItem *item, items())
    {
        if (JigsawPuzzleItem *widget = dynamic_cast<JigsawPuzzleItem*>(item))
        {
            widget->setMerge(false);
        }
    }
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
