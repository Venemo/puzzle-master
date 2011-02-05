#include "puzzleitem.h"

PuzzleItem::PuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPixmapItem(pixmap, parent, scene)
{
}

const QPoint &PuzzleItem::position() const
{
    return _position;
}

const QList<PuzzleItem*> &PuzzleItem::neighbours() const
{
    return _neighbours;
}

void PuzzleItem::setPosition(const QPoint &p)
{
    _position = p;
}

void PuzzleItem::addNeighbour(PuzzleItem *piece)
{
    if (piece != this)
    {
        //qDebug() << position() << "added" << piece->position() << "as neighbour";
        if (!_neighbours.contains(piece))
            _neighbours.append(piece);
        if (!piece->_neighbours.contains(this))
            piece->_neighbours.append(this);
    }
}

void PuzzleItem::removeNeighbour(PuzzleItem *piece)
{
    if (piece != this)
    {
        if (_neighbours.contains(piece))
            _neighbours.removeAll(piece);
        if (piece->_neighbours.contains(this))
            piece->_neighbours.removeAll(this);
    }
}

bool PuzzleItem::isNeighbourOf(const PuzzleItem *piece) const
{
    if (piece->neighbours().contains((PuzzleItem*)this) && this->neighbours().contains((PuzzleItem*)piece))
        return true;
    return false;
}

bool PuzzleItem::merge(PuzzleItem *piece)
{
    if (isNeighbourOf(piece))
    {
        foreach (PuzzleItem *n, piece->_neighbours)
        {
            piece->removeNeighbour(n);
            this->addNeighbour(n);
        }
        //qDebug() << "merged" << position() << "and" << piece->position();
        return true;
    }
    return false;
}

void PuzzleItem::setNeighbours(QList<PuzzleItem *> *list, int x, int y)
{
    if (list->count() != x * y)
    {
        qDebug() << "The Puzzle piece list was inconsistent with x and y";
        return;
    }

    foreach(PuzzleItem *p, *list)
    {
        if (p->position().x() != x - 1)
            p->addNeighbour(find(list, p->position() + QPoint(1, 0)));

        if (p->position().y() != y - 1)
            p->addNeighbour(find(list, p->position() + QPoint(0, 1)));
    }
}

PuzzleItem *PuzzleItem::find(QList<PuzzleItem *> *pieces, QPoint position)
{
    foreach(PuzzleItem *p, *pieces)
    {
        if (p->position() == position)
            return p;
    }
    return 0;
}
