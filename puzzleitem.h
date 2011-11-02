#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QtGui>

#define DROPSHADOW_COLOR_DEFAULT QColor(0, 0, 0, 200)
#define DROPSHADOW_COLOR_SELECTED QColor(0, 0, 0, 255)

#define DROPSHADOW_RADIUS_DEFAULT 20
#define DROPSHADOW_RADIUS_SELECTED 25

class PuzzleItem : public QGraphicsPixmapItem
{
private:
    QPoint _puzzleCoordinates;
    QPointF _supposedPosition;
    QList<PuzzleItem*> _neighbours;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
    explicit PuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    const QPoint &puzzleCoordinates() const;
    const QPointF &supposedPosition() const;
    const QList<PuzzleItem*> &neighbours() const;

    void setPuzzleCoordinates(const QPoint &p);
    void setSupposedPosition(const QPointF &p);
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);

    bool isNeighbourOf(const PuzzleItem *piece) const;
};

#endif // PUZZLEPIECE_H
