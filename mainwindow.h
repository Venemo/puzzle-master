#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "newgamedialog.h"
#include "highscoresdialog.h"
#include "puzzleboard.h"



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Ui::MainWindow *ui;
    QTimer *timer;
    SettingsDialog *settings;
    NewGameDialog *newgame;
    HighScoresDialog *highscores;
    PuzzleBoard *board;
    QPointer<QGraphicsTextItem> intro;
    bool _isPlaying;
    bool _isPaused;
    bool _wasPaused;
    bool _canPause;
    int _secsElapsed;
    qreal _currentScaleRatio;
    void fixCurrentOrientation();
    void unfixCurrentOrientation();

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showHighScores();
    void showSettings();
    void newGame();
    void surrender();
    void togglePause();
    void toggleFullscreen();

private slots:
    void onWon();
    void about();
    void pause();
    void unpause();
    void updateElapsedTimeLabel();
    void initializeGame();
    void endGame();
};

#endif // MAINWINDOW_H
