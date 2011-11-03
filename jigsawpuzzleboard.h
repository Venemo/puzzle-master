#ifndef JIGSAWPUZZLEBOARD_H
#define JIGSAWPUZZLEBOARD_H

#include "puzzleboard.h"

class JigsawPuzzleItem;

class JigsawPuzzleBoard : public PuzzleBoard
{
    Q_OBJECT
    QSize _unit;
    int _tolerance;
    qreal _rotationTolerance;
    bool _allowMultitouch;

public:
    explicit JigsawPuzzleBoard(QObject *parent = 0);
    void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols, bool allowMultitouch);
    inline int tolerance() const;
    inline void setTolerance(int tolerance);
    inline qreal rotationTolerance() const;
    inline void setRotationTolerance(qreal i);
    void assemble();
    inline const QSize &unit() const;

protected:
    void accelerometerMovement(qreal x, qreal y, qreal z);

signals:
    void gameStarted();
    void gameWon();
    void shuffleComplete();
    void assembleComplete();

private slots:

public slots:
    void surrenderGame();
    void shuffle();
    void disable();
    void enable();

};

inline const QSize &JigsawPuzzleBoard::unit() const
{
    return _unit;
}

inline int JigsawPuzzleBoard::tolerance() const
{
    return _tolerance;
}

inline void JigsawPuzzleBoard::setTolerance(int tolerance)
{
    _tolerance = tolerance;
}

inline qreal JigsawPuzzleBoard::rotationTolerance() const
{
    return _rotationTolerance;
}

inline void JigsawPuzzleBoard::setRotationTolerance(qreal i)
{
    _rotationTolerance = i;
}

#endif // JIGSAWPUZZLEBOARD_H
