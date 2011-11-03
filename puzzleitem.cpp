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

const QPointF &PuzzleItem::supposedPosition() const
{
    return _supposedPosition;
}

const QList<PuzzleItem*> &PuzzleItem::neighbours() const
{
    return _neighbours;
}

void PuzzleItem::setPuzzleCoordinates(const QPoint &p)
{
    _puzzleCoordinates = p;
}

void PuzzleItem::setSupposedPosition(const QPointF &p)
{
    _supposedPosition = p;
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

void PuzzleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mousePressEvent(event);
    if (QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect*>(this->graphicsEffect()))
    {
        effect->setColor(DROPSHADOW_COLOR_SELECTED);
        effect->setBlurRadius(DROPSHADOW_RADIUS_SELECTED);
    }
}

void PuzzleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseReleaseEvent(event);

    if (QGraphicsDropShadowEffect *effect = qobject_cast<QGraphicsDropShadowEffect*>(this->graphicsEffect()))
    {
        effect->setColor(DROPSHADOW_COLOR_DEFAULT);
        effect->setBlurRadius(DROPSHADOW_RADIUS_DEFAULT);
    }
}
