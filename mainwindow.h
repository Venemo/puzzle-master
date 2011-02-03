#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "imagechooser.h"
#include "highscoresdialog.h"

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
#include <QAccelerometer>
#endif

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    QTimer *timer;
    SettingsDialog *settings;
    ImageChooser *chooser;
    HighScoresDialog *highscores;
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    QtMobility::QAccelerometer *accelerometer;
#endif
    bool _isPlaying;
    int _secsElapsed;

    void initializeGame();
    void endGame();

protected:
    void focusOutEvent(QFocusEvent *event);
    void focusInEvent(QFocusEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionHigh_scores_triggered();
    void on_actionSettings_triggered();
    void on_btnOpenImage_clicked();
    void onWon();
    void accelerometerReadingChanged();
    void about();
    void elapsedSecond();
};

#endif // MAINWINDOW_H
