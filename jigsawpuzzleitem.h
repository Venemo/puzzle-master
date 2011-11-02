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
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

    QPointF _dragStart;
    QPointF _rotationStartVector;
    bool _dragging;
    bool _canMerge;
    bool _isDraggingWithTouch;
    double _weight;
    double _previousRotationValue;
    int _previousTouchPointCount;

public:
    explicit JigsawPuzzleItem(const QPixmap &pixmap, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual bool merge(JigsawPuzzleItem *item);
    bool canMerge() const;
    void setMerge(bool canMerge);
    qreal weight() const;
    inline int tolerance() const;
    inline qreal rotationTolerance() const;
    inline const QSize &unit() const;
    void raise();
    void verifyPosition();
    void verifyCoveredSiblings();

public slots:
    void enableMerge();
    void disableMerge();

signals:
    void noNeighbours();

protected:
    void startDrag(const QPointF &pos);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void handleRotation(const QPointF &vector);
    void setCompensatedTransformOriginPoint(const QPointF &point);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);
    virtual bool sceneEvent(QEvent *event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

inline const QSize &JigsawPuzzleItem::unit() const
{
    return ((JigsawPuzzleBoard*)scene())->unit();
}

inline int JigsawPuzzleItem::tolerance() const
{
    return ((JigsawPuzzleBoard*)scene())->tolerance();
}

inline qreal JigsawPuzzleItem::rotationTolerance() const
{
    return ((JigsawPuzzleBoard*)scene())->rotationTolerance();
}

#endif // JIGSAWPUZZLEITEM_H
