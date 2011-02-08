#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QtGui>

class PuzzleItem : public QGraphicsPixmapItem
{
private:
    QPoint _puzzleCoordinates;
    QList<PuzzleItem*> _neighbours;

public:
    explicit PuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    const QPoint &puzzleCoordinates() const;
    const QList<PuzzleItem*> &neighbours() const;

    void setPuzzleCoordinates(const QPoint &p);
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);

    bool isNeighbourOf(const PuzzleItem *piece) const;
};

#endif // PUZZLEPIECE_H
