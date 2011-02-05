#ifndef JIGSAWPUZZLEITEM_H
#define JIGSAWPUZZLEITEM_H

#include <QtGui>
#include "puzzleitem.h"

QPointF operator*(const QPoint &point, const QSize &size);

class JigsawPuzzleItem : public QObject, public PuzzleItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    QPointF _dragStart;
    QSize _unit;
    bool _dragging;
    bool _canMerge;
    int _tolerance;
    double _weight;

public:
    explicit JigsawPuzzleItem(const QPixmap &pixmap, const QSize &unitSize, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual bool merge(PuzzleItem *piece);
    bool canMerge() const;
    void setMerge(bool canMerge);
    double weight();
    static void shuffle(QList<JigsawPuzzleItem*> *list, int x, int y, int width, int height);
    void raise();
    void verifyPosition();

public slots:
    void enableMerge();
    void disableMerge();

signals:
    void noNeighbours();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);

};

#endif // JIGSAWPUZZLEITEM_H
