#ifndef JIGSAWPUZZLEBOARD_H
#define JIGSAWPUZZLEBOARD_H

#include "puzzleboard.h"
#include "puzzlewidget.h"

class JigsawPuzzleBoard : public PuzzleBoard
{
    Q_OBJECT

public:
    explicit JigsawPuzzleBoard(QObject *parent = 0);
    void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols);

protected:
    void accelerometerMovement(qreal x, qreal y, qreal z);

signals:
    void gameStarted();
    void gameWon();
    void loadProgressChanged(int progress);

private slots:

public slots:
    void surrenderGame();

};

#endif // JIGSAWPUZZLEBOARD_H
