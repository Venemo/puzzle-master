#ifndef JIGSAWPUZZLEITEM_H
#define JIGSAWPUZZLEITEM_H

#include <QtGui>
#include "puzzleitem.h"
#include "jigsawpuzzleboard.h"

class JigsawPuzzleBoard;

class JigsawPuzzleItem : public QObject, public PuzzleItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    QPointF _dragStart;
    bool _dragging;
    bool _canMerge;
    double _weight;

public:
    explicit JigsawPuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual bool merge(JigsawPuzzleItem *item);
    bool canMerge() const;
    void setMerge(bool canMerge);
    double weight();
    inline int tolerance();
    inline const QSize &unit();
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

inline const QSize &JigsawPuzzleItem::unit()
{
    return ((JigsawPuzzleBoard*)scene())->unit();
}

inline int JigsawPuzzleItem::tolerance()
{
    return ((JigsawPuzzleBoard*)scene())->tolerance();
}

#endif // JIGSAWPUZZLEITEM_H
