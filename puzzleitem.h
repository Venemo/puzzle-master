#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QDeclarativeItem>
#include <QPixmap>
#include "puzzleboard.h"
#include "util.h"

#define DROPSHADOW_COLOR_DEFAULT QColor(0, 0, 0, 200)
#define DROPSHADOW_COLOR_SELECTED QColor(0, 0, 0, 255)

#define DROPSHADOW_RADIUS_DEFAULT 20
#define DROPSHADOW_RADIUS_SELECTED 25

class PuzzleItem : public QDeclarativeItem
{
private:
    Q_OBJECT
    GENPROPERTY(QPoint, _puzzleCoordinates, puzzleCoordinates, setPuzzleCoordinates, puzzleCoordinatesChanged)
    Q_PROPERTY(QPoint puzzleCoordinates READ puzzleCoordinates WRITE setPuzzleCoordinates NOTIFY puzzleCoordinatesChanged)
    GENPROPERTY(QPointF, _supposedPosition, supposedPosition, setSupposedPosition, supposedPositionChanged)
    Q_PROPERTY(QPointF supposedPosition READ supposedPosition WRITE setSupposedPosition NOTIFY supposedPositionChanged)
    GENPROPERTY_R(QList<PuzzleItem*>, _neighbours, neighbours)
    Q_PROPERTY(QList<PuzzleItem*> neighbours READ neighbours)
    GENPROPERTY_R(QPixmap, _pixmap, pixmap)
    Q_PROPERTY(QPixmap pixmap READ pixmap)
    GENPROPERTY(bool, _canMerge, canMerge, setCanMerge, canMergeChanged)
    Q_PROPERTY(bool canMerge READ canMerge WRITE setCanMerge NOTIFY canMergeChanged)
    GENPROPERTY(qreal, _weight, weight, setWeight, weightChanged)
    Q_PROPERTY(qreal weight READ weight WRITE setWeight NOTIFY weightChanged)

    QPointF _dragStart;
    QPointF _rotationStartVector;
    bool _dragging;
    bool _isDraggingWithTouch;
    double _previousRotationValue;
    int _previousTouchPointCount;

public:
    explicit PuzzleItem(const QPixmap &pixmap, QDeclarativeItem *parent = 0);

    bool merge(PuzzleItem *item);
    inline int tolerance() const;
    inline qreal rotationTolerance() const;
    inline const QSize &unit() const;
    void raise();
    void verifyPosition();
    void verifyCoveredSiblings();

    void addNeighbour(PuzzleItem *piece);
    void removeNeighbour(PuzzleItem *piece);
    bool isNeighbourOf(const PuzzleItem *piece) const;public slots:
    void enableMerge();
    void disableMerge();

signals:
    void puzzleCoordinatesChanged();
    void supposedPositionChanged();
    void canMergeChanged();
    void weightChanged();

    void noNeighbours();

protected:
    void startDrag(const QPointF &pos);
    void stopDrag();
    void doDrag(const QPointF &pos);
    void handleRotation(const QPointF &vector);
    void setCompensatedTransformOriginPoint(const QPointF &point);
    void emphasise();
    void deEmphasise();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *ev);
    virtual bool sceneEvent(QEvent *event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

inline const QSize &PuzzleItem::unit() const
{
    return ((PuzzleBoard*)scene())->unit();
}

inline int PuzzleItem::tolerance() const
{
    return ((PuzzleBoard*)scene())->tolerance();
}

inline qreal PuzzleItem::rotationTolerance() const
{
    return ((PuzzleBoard*)scene())->rotationTolerance();
}

#endif // PUZZLEPIECE_H
