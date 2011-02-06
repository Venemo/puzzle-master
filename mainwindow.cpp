#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"
#include "util.h"
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
    _isPlaying(false),
    _oldGraphicsViewSize(0, 0),
    _currentScaleRatio(1)
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
    board->setOriginalPixmapSize(ui->graphicsView->size());

    ui->graphicsView->setScene(board);
    ui->graphicsView->setViewport(new QGLWidget(this));

#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5AutoOrientation);

    ui->btnFullscreen->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->btnFullscreen->setIcon(QIcon::fromTheme("general_fullsize"));
#endif
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (event)
        QMainWindow::resizeEvent(event);

    // The first resize event will contain garbage
    if (_oldGraphicsViewSize == QSize(0, 0))
    {
        _oldGraphicsViewSize = QSize(0,1);
        return;
    }
    // Second resize event, don't scale just remember the size
    else if (_oldGraphicsViewSize == QSize(0, 1))
    {
        _oldGraphicsViewSize = ui->graphicsView->size();
        return;
    }

    // On every other resize event, actually scale the graphics view's content

    // Calculating scale ratio
    qreal oldScaleRatio = _currentScaleRatio;
    QSize s1 = board->originalPixmapSize();
    QSize s2 = board->originalPixmapSize();
    s1.scale(ui->graphicsView->size(), Qt::KeepAspectRatio);
    s2.scale(_oldGraphicsViewSize, Qt::KeepAspectRatio);
    _currentScaleRatio = min<qreal>((qreal)s1.width() / (qreal)s2.width(), (qreal)s1.height() / (qreal)s2.height());

    qDebug() << "old size" << _oldGraphicsViewSize << "new size" << ui->graphicsView->size() << "ratio" << _currentScaleRatio;

    // Setting scene rect and scale
    QSizeF olds = (_oldGraphicsViewSize / oldScaleRatio - board->originalPixmapSize()) / 2;
    if (intro.isNull())
    {
        board->setSceneRect(0, 0, ui->graphicsView->width() / _currentScaleRatio, ui->graphicsView->height() / _currentScaleRatio);
    }
    ui->graphicsView->scale(_currentScaleRatio, _currentScaleRatio);

    // Making sure every piece is visible and has a nice position
    QSizeF news = (board->sceneRect().size() - board->originalPixmapSize()) / 2;
    QSizeF p = news - olds;
    foreach (QGraphicsItem *item, board->items())
    {
        QPointF newPos(item->pos().x() + p.width(),
                       item->pos().y() + p.height());
        item->setPos(newPos);
        if (JigsawPuzzleItem *jpi = dynamic_cast<JigsawPuzzleItem*>(item))
        {
            jpi->verifyPosition();
        }
    }

    // Saving size
    _oldGraphicsViewSize = ui->graphicsView->size();
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

            ui->graphicsView->resetTransform();
            board = new JigsawPuzzleBoard(ui->graphicsView);
            board->setBackgroundBrush(QBrush(SettingsDialog::boardBackground()));
            connect(board, SIGNAL(loadProgressChanged(int)), progress, SLOT(setValue(int)));
            connect(board, SIGNAL(gameStarted()), progress, SLOT(deleteLater()));
            connect(board, SIGNAL(gameWon()), this, SLOT(onWon()));
            ui->graphicsView->setScene(board);
            board->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

            connect(board, SIGNAL(gameStarted()), this, SLOT(initializeGame()));
            if (SettingsDialog::useDropShadow())
                connect(board, SIGNAL(loaded()), board, SLOT(enableDropshadow()));

            board->startGame(pixmap, rows, cols);
            //_currentScaleRatio = 1;
            _oldGraphicsViewSize = ui->graphicsView->size();
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
    int timeout = width() > height() ? QMaemo5InformationBox::NoTimeout : 5000;
    QMaemo5InformationBox::information(0, "<b>You rock!</b><br />Congratulations!<br />You've successfully solved the given puzzle!", timeout);
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

    if (JigsawPuzzleBoard *jpb = qobject_cast<JigsawPuzzleBoard*>(board))
    {
        jpb->setToleranceForPieces(SettingsDialog::tolerance());
    }
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
    if (JigsawPuzzleBoard *jpb = qobject_cast<JigsawPuzzleBoard*>(board))
    {
        jpb->setToleranceForPieces(SettingsDialog::tolerance());
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
    QString str = "Elapsed " + QString::number(_secsElapsed) + " second";
    ui->lblTime->setText(_secsElapsed == 1 ? str : str + "s");
}

void MainWindow::on_btnFullscreen_clicked()
{
    if (isFullScreen())
    {
        ui->btnFullscreen->setText("+");
        showNormal();
    }
    else
    {
        ui->btnFullscreen->setText("-");
        showFullScreen();
    }
}
