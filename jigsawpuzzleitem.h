#ifndef JIGSAWPUZZLEITEM_H
#define JIGSAWPUZZLEITEM_H

#include <QtGui>
#include "puzzleitem.h"

class JigsawPuzzleBoard;

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
    virtual bool merge(JigsawPuzzleItem *item);
    bool canMerge() const;
    void setMerge(bool canMerge);
    double weight();
    void setTolerance(int t);
    int tolerance();
    const QSize &unit();
    void raise();
    void verifyPosition();
    void verifyCoveredSiblings();

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
