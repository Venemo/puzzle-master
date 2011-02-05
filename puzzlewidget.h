#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QtGui>
#include "puzzlepiece.h"

QPointF operator*(const QPoint &point, const QSize &size);

class PuzzleWidget : public QObject, public QGraphicsPixmapItem, public PuzzlePiece
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
    explicit PuzzleWidget(const QPixmap &pixmap,  const QSize &unitSize, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    virtual bool merge(PuzzlePiece *piece);
    bool canMerge() const;
    void setMerge(bool canMerge);
    double weight();
    static void shuffle(QList<PuzzleWidget*> *list, int x, int y, int width, int height);
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

#endif // PUZZLEWIDGET_H
