#include "puzzleitem.h"

PuzzleItem::PuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsPixmapItem(pixmap, parent, scene)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

const QPoint &PuzzleItem::puzzleCoordinates() const
{
    return _puzzleCoordinates;
}

const QList<PuzzleItem*> &PuzzleItem::neighbours() const
{
    return _neighbours;
}

void PuzzleItem::setPuzzleCoordinates(const QPoint &p)
{
    _puzzleCoordinates = p;
}

void PuzzleItem::addNeighbour(PuzzleItem *piece)
{
    if (piece != this)
    {
        //qDebug() << puzzleCoordinates() << "added" << piece->puzzleCoordinates() << "as neighbour";
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
