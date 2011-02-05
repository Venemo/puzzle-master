#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QtGui>

class PuzzleItem : public QGraphicsPixmapItem
{
private:
    QPoint _position;
    QList<PuzzleItem*> _neighbours;

public:
    explicit PuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

    const QPoint &position() const;
    const QList<PuzzleItem*> &neighbours() const;

    void setPosition(const QPoint &p);
    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);

    bool isNeighbourOf(const PuzzleItem *piece) const;
    virtual bool merge(PuzzleItem *piece);

    static void setNeighbours(QList<PuzzleItem*> *pieces, int x, int y);
    static PuzzleItem *find(QList<PuzzleItem*> *pieces, QPoint position);

};

#endif // PUZZLEPIECE_H
