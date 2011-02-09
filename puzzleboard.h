#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QGraphicsScene>

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
#include <QAccelerometer>
#endif

class PuzzleItem;

class PuzzleBoard : public QGraphicsScene
{
    Q_OBJECT
    QSize _originalPixmapSize;
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    QtMobility::QAccelerometer *accelerometer;
#endif

public:
    explicit PuzzleBoard(QObject *parent = 0);
    virtual void startGame(const QPixmap &pixmap, unsigned rows, unsigned cols) = 0;
    const QSize &originalPixmapSize();
    void setNeighbours(int x, int y);
    void setOriginalPixmapSize(const QSize &size);
    PuzzleItem *find(QPoint puzzleCoordinates);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    bool isDropshadowActive();
    void enableDropshadow();
    void disableDropshadow();
#endif
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    bool isAccelerometerActive();
    void enableAccelerometer();
    void disableAccelerometer();
#endif

protected:
    virtual void accelerometerMovement(qreal x, qreal y, qreal z) = 0;

signals:
    void gameStarted();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);

private slots:
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    void accelerometerReadingChanged();
#endif

public slots:
    virtual void surrenderGame() = 0;

};

#endif // PUZZLEBOARD_H
