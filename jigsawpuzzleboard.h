#ifndef JIGSAWPUZZLEBOARD_H
#define JIGSAWPUZZLEBOARD_H

#include "puzzleboard.h"
#include "util.h"

class JigsawPuzzleItem;

class JigsawPuzzleBoard : public PuzzleBoard
{
    Q_OBJECT
    GENPROPERTY_R(QSize, _unit, unit)
    Q_PROPERTY(QSize unit READ unit NOTIFY unitChanged)
    GENPROPERTY(int, _tolerance, tolerance, setTolerance, toleranceChanged)
    Q_PROPERTY(int tolerance READ tolerance WRITE setTolerance NOTIFY toleranceChanged)
    GENPROPERTY(int, _rotationTolerance, rotationTolerance, setRotationTolerance, rotationToleranceChanged)
    Q_PROPERTY(int rotationTolerance READ rotationTolerance WRITE setRotationTolerance NOTIFY rotationToleranceChanged)
    GENPROPERTY_R(bool, _allowMultitouch, allowMultitouch)
    Q_PROPERTY(bool allowMultitouch READ allowMultitouch NOTIFY allowMultitouchChanged)

public:
    explicit JigsawPuzzleBoard(QObject *parent = 0);
    void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols, bool allowMultitouch);
    void assemble();

protected:
    void accelerometerMovement(qreal x, qreal y, qreal z);

signals:
    void unitChanged();
    void toleranceChanged();
    void rotationToleranceChanged();
    void allowMultitouchChanged();

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

#endif // JIGSAWPUZZLEBOARD_H
