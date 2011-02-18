#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "imagechooser.h"
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
    ImageChooser *chooser;
    HighScoresDialog *highscores;
    PuzzleBoard *board;
    QPointer<QGraphicsTextItem> intro;
    bool _isPlaying;
    bool _isPaused;
    bool _wasPaused;
    int _secsElapsed;
    qreal _currentScaleRatio;

protected:
    bool event(QEvent *event);
    void resizeEvent(QResizeEvent *event);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionHigh_scores_triggered();
    void showSettings();
    void on_btnOpenImage_clicked();
    void onWon();
    void about();
    void pause();
    void unpause();
    void togglePause();
    void updateElapsedTimeLabel();
    void initializeGame();
    void endGame();
    void on_btnFullscreen_clicked();
    void on_btnPause_clicked();
};

#endif // MAINWINDOW_H
