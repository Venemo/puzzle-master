#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QLabel>
#include "puzzlepiece.h"

class PuzzleWidget : public QLabel, public PuzzlePiece
{
    Q_OBJECT
    QPoint _dragStart;
    QSize _unit;
    bool _dragging;
    bool _canMerge;
    int _tolerance;
    double _weight;

public:
    explicit PuzzleWidget(const QPixmap &pixmap, const QSize &unitSize, QWidget *parent = 0);
    virtual bool merge(PuzzlePiece *piece);
    bool canMerge() const;
    void setMerge(bool canMerge);
    double weight();
    static void shuffle(QList<PuzzleWidget*> *list, int x, int y, int width, int height);

public slots:
    void enableMerge();

signals:
    void noNeighbours();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void moveEvent(QMoveEvent *ev);

};

#endif // PUZZLEWIDGET_H
