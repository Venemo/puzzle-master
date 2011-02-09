#ifndef PUZZLEBOARD_H
#define PUZZLEBOARD_H

#include <QGraphicsScene>

#if defined(HAVE_QACCELEROMETER)
#include <QAccelerometer>
#endif

class PuzzleItem;

class PuzzleBoard : public QGraphicsScene
{
    Q_OBJECT
    QSize _originalPixmapSize;
#if defined(HAVE_QACCELEROMETER)
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
#endif
#if defined(HAVE_QACCELEROMETER)
    bool isAccelerometerActive();
#endif

protected:
    virtual void accelerometerMovement(qreal x, qreal y, qreal z) = 0;

signals:
    void gameStarted();
    void gameWon();
    void loaded();
    void loadProgressChanged(int progress);

private slots:
#if defined(HAVE_QACCELEROMETER)
    void accelerometerReadingChanged();
#endif

public slots:
    virtual void surrenderGame() = 0;
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    void enableDropshadow();
    void disableDropshadow();
#endif
#if defined(HAVE_QACCELEROMETER)
    void enableAccelerometer();
    void disableAccelerometer();
#endif

};

#endif // PUZZLEBOARD_H
