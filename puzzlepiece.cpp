#include "puzzlepiece.h"

PuzzlePiece::PuzzlePiece()
{
}

const QPoint &PuzzlePiece::position() const
{
    return _position;
}

const QList<PuzzlePiece*> &PuzzlePiece::neighbours() const
{
    return _neighbours;
}

void PuzzlePiece::setPosition(const QPoint &p)
{
    _position = p;
}

void PuzzlePiece::addNeighbour(PuzzlePiece *piece)
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

void PuzzlePiece::removeNeighbour(PuzzlePiece *piece)
{
    if (piece != this)
    {
        if (_neighbours.contains(piece))
            _neighbours.removeAll(piece);
        if (piece->_neighbours.contains(this))
            piece->_neighbours.removeAll(this);
    }
}

bool PuzzlePiece::isNeighbourOf(const PuzzlePiece *piece) const
{
    if (piece->neighbours().contains((PuzzlePiece*)this) && this->neighbours().contains((PuzzlePiece*)piece))
        return true;
    return false;
}

bool PuzzlePiece::merge(PuzzlePiece *piece)
{
    if (isNeighbourOf(piece))
    {
        foreach (PuzzlePiece *n, piece->_neighbours)
        {
            piece->removeNeighbour(n);
            this->addNeighbour(n);
        }
        //qDebug() << "merged" << position() << "and" << piece->position();
        return true;
    }
    return false;
}

void PuzzlePiece::setNeighbours(QList<PuzzlePiece *> *list, int x, int y)
{
    if (list->count() != x * y)
    {
        qDebug() << "The Puzzle piece list was inconsistent with x and y";
        return;
    }

    foreach(PuzzlePiece *p, *list)
    {
        if (p->position().x() != x - 1)
            p->addNeighbour(find(list, p->position() + QPoint(1, 0)));

        if (p->position().y() != y - 1)
            p->addNeighbour(find(list, p->position() + QPoint(0, 1)));
    }
}

PuzzlePiece *PuzzlePiece::find(QList<PuzzlePiece *> *pieces, QPoint position)
{
    foreach(PuzzlePiece *p, *pieces)
    {
        if (p->position() == position)
            return p;
    }
    return 0;
}
