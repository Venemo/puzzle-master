#ifndef JIGSAWPUZZLEBOARD_H
#define JIGSAWPUZZLEBOARD_H

#include "puzzleboard.h"

class JigsawPuzzleItem;

class JigsawPuzzleBoard : public PuzzleBoard
{
    Q_OBJECT
    QSize _unit;
    int _tolerance;
    bool _allowMultitouch;

public:
    explicit JigsawPuzzleBoard(QObject *parent = 0);
    void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols, bool allowMultitouch);
    inline int tolerance();
    inline void setTolerance(int tolerance);
    void assemble();
    inline const QSize &unit();

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

inline const QSize &JigsawPuzzleBoard::unit()
{
    return _unit;
}

inline int JigsawPuzzleBoard::tolerance()
{
    return _tolerance;
}

inline void JigsawPuzzleBoard::setTolerance(int tolerance)
{
    _tolerance = tolerance;
}

#endif // JIGSAWPUZZLEBOARD_H
