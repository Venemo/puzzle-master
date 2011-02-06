#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jigsawpuzzleboard.h"
#include <QtGui>
#include <QGLWidget>

#if defined(Q_WS_MAEMO_5)
#include <QtMaemo5>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer(this)),
    settings(new SettingsDialog(this)),
    chooser(new ImageChooser(this)),
    highscores(new HighScoresDialog(this)),
    board(new JigsawPuzzleBoard(this)),
    _isPlaying(false)
{
    ui->setupUi(this);
    ui->lblTime->hide();

    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(elapsedSecond()));
    setFocus();

    QColor bg = SettingsDialog::boardBackground();
    board->setBackgroundBrush(QBrush(bg));
    intro = new QGraphicsTextItem("To start playing, please press the 'New game' button!");
    intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    board->addItem(intro);
    ui->graphicsView->setScene(board);
    ui->graphicsView->setViewport(new QGLWidget(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::focusInEvent(QFocusEvent *event)
{
    if (_isPlaying)
        timer->start();
    QMainWindow::focusInEvent(event);
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    if (timer->isActive())
        timer->stop();
    QMainWindow::focusOutEvent(event);
}

void MainWindow::on_actionHigh_scores_triggered()
{
    highscores->exec();
}

void MainWindow::on_btnOpenImage_clicked()
{
    if (!_isPlaying)
    {
        int rows = SettingsDialog::rows();
        int cols = SettingsDialog::columns();

        if (!QDialog::Accepted == chooser->exec())
            return;

        _isPlaying = true;

        QPixmap pixmap;
        if (pixmap.load(chooser->getPictureFile()) && !pixmap.isNull())
        {
            QProgressDialog *progress = new QProgressDialog("Generating puzzle...", "Cancel", 0, rows * cols, this);
            progress->setWindowTitle("Generating puzzle...");
            progress->show();

            if (board != 0)
            {
                board->deleteLater();
            }

            board = new JigsawPuzzleBoard(ui->graphicsView);
            board->setBackgroundBrush(QBrush(SettingsDialog::boardBackground()));
            connect(board, SIGNAL(loadProgressChanged(int)), progress, SLOT(setValue(int)));
            connect(board, SIGNAL(gameStarted()), progress, SLOT(deleteLater()));
            connect(board, SIGNAL(gameWon()), this, SLOT(onWon()));
            ui->graphicsView->setScene(board);
            board->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
            board->startGame(pixmap, rows, cols);

            initializeGame();
        }
        else
        {
            QMessageBox::warning(this, "Error", "Could not load the selected image.");
            _isPlaying = false;
        }
    }
    else if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Are you sure you want to abandon this game?", QMessageBox::Yes, QMessageBox::Cancel))
    {
        board->surrenderGame();
        endGame();
    }
}


void MainWindow::onWon()
{
    // ending the game
    endGame();

    // Showing congratulations
#if defined(Q_WS_MAEMO_5)
    QMaemo5InformationBox::information(0, "<b>You rock!</b><br />Congratulations!<br />You've successfully solved the given puzzle!", QMaemo5InformationBox::NoTimeout);
#else
    QMessageBox::information(this, "You rock!", "Congratulations!\nYou've successfully solved the given puzzle!");
#endif

    // Adding to high scores
    highscores->addResult(QString::number(SettingsDialog::columns()) + QString::fromUtf8("×") + QString::number(SettingsDialog::rows()),
                          QDateTime::currentDateTime().toString(Qt::SystemLocaleLongDate) + " - " + QString::number(_secsElapsed) + " seconds");
}

void MainWindow::initializeGame()
{
    timer->start();
    ui->lblTime->show();
    _secsElapsed = 0;

    if (SettingsDialog::useDropShadow())
        board->enableDropshadow();
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    if (SettingsDialog::useAccelerometer())
        board->enableAccelerometer();
#endif
    _isPlaying = true;
    ui->btnOpenImage->setText("Surrender");
}

void MainWindow::endGame()
{
    timer->stop();

    // Additional things
    ui->btnOpenImage->setText("New game...");
    _isPlaying = false;
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    board->disableAccelerometer();
#endif
}

void MainWindow::on_actionSettings_triggered()
{
    settings->exec();
    QColor bg = SettingsDialog::boardBackground();
    if (board->backgroundBrush().color() != bg)
    {
        board->setBackgroundBrush(QBrush(bg));
        if (!intro.isNull())
            intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    }
    if (SettingsDialog::useDropShadow() && !board->isDropshadowActive())
        board->enableDropshadow();
    else if (!SettingsDialog::useDropShadow() && board->isDropshadowActive())
        board->disableDropshadow();
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    if (SettingsDialog::useAccelerometer() && !board->isAccelerometerActive())
        board->enableAccelerometer();
    else if (!SettingsDialog::useAccelerometer() && board->isAccelerometerActive())
        board->disableAccelerometer();
#endif
}

void MainWindow::about()
{
    bool wasActive = timer->isActive();
    timer->stop();
    QFile file(":/about.txt");
    file.open(QIODevice::ReadOnly);
    QString str = QString::fromUtf8(file.readAll().constData());
    file.close();
    QMessageBox::information(this, "About", str, QMessageBox::Ok
                         #if defined(Q_WS_MAEMO_5)
                             , QMessageBox::Cancel // So the user can close it by tapping on the blurred area
                         #endif
                             );
    if (wasActive)
        timer->start();
}

void MainWindow::elapsedSecond()
{
    _secsElapsed++;
    QString str = "Elapsed time: " + QString::number(_secsElapsed) + " second";
    ui->lblTime->setText(_secsElapsed == 1 ? str : str + "s");
}
