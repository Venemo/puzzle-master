#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"
#include "util.h"
#include <QtGui>

#if defined(Q_WS_MAEMO_5)
#include <QtMaemo5>
#include <hildon-extras-1/hildon-extras/qt-he-wrapper.h>
#endif

#if defined(HAVE_OPENGL)
#include <QGLWidget>
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
    _isPaused(false),
    _wasPaused(false),
    _currentScaleRatio(1)
{
    ui->setupUi(this);

    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    ui->btnPause->hide();
    ui->btnHelp->hide();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateElapsedTimeLabel()));
    setFocus();

    QColor bg = SettingsDialog::boardBackground();
    board->setBackgroundBrush(QBrush(bg));
    intro = new QGraphicsTextItem("Please press the new game button!");
    intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    board->addItem(intro);

    ui->graphicsView->setScene(board);
#if defined(HAVE_OPENGL)
    ui->graphicsView->setViewport(new QGLWidget(this));
#endif

#if defined(MOBILE)
    delete ui->lblTime;
#endif

#if defined(Q_WS_MAEMO_5)
    setFixedHeight(424);
    setAttribute(Qt::WA_Maemo5AutoOrientation);
    ui->btnFullscreen->setIcon(QIcon::fromTheme("general_fullsize"));
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    bool result = QMainWindow::event(event);
    if (_isPlaying && !_wasPaused && _isPaused && event->type() == QEvent::WindowActivate)
    {
        unpause();
    }
    else if (_isPlaying && event->type() == QEvent::WindowDeactivate)
    {
        // If the game was paused before deactivating, it will be paused after activating.
        _wasPaused = _isPaused;
        pause();
    }
    return result;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (event)
        QMainWindow::resizeEvent(event);

    if (intro != 0)
        return;

    // Scaling the graphics view's content

    // Calculating scale ratio
    qreal oldScaleRatio = _currentScaleRatio;
    QSize scaledPixmapSize = board->originalPixmapSize();
    scaledPixmapSize.scale(ui->graphicsView->size(), Qt::KeepAspectRatio);
    _currentScaleRatio = min<qreal>((qreal)scaledPixmapSize.width() / (qreal)board->originalPixmapSize().width(),
                                    (qreal)scaledPixmapSize.height() / (qreal)board->originalPixmapSize().height());

    //qDebug() << "new size" << ui->graphicsView->size() << "ratio" << _currentScaleRatio;

    // Setting scene rect and scale
    QSizeF oldscenesize = board->sceneRect().size();
    board->setSceneRect(0, 0, ui->graphicsView->width() / _currentScaleRatio, ui->graphicsView->height() / _currentScaleRatio);
    ui->graphicsView->scale(_currentScaleRatio / oldScaleRatio, _currentScaleRatio / oldScaleRatio);

    // Making sure every piece is visible and has a nice position
    QSizeF p = (board->sceneRect().size() - oldscenesize) / 2;
    foreach (QGraphicsItem *item, board->items())
    {
        QPointF newPos(item->pos().x() + p.width(),
                       item->pos().y() + p.height());
        item->setPos(newPos);
        if (JigsawPuzzleItem *jpi = dynamic_cast<JigsawPuzzleItem*>(item))
        {
            if (jpi->canMerge())
                jpi->verifyPosition();
        }
    }
    event->accept();
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
            ui->btnOpenImage->setIcon(QIcon(QPixmap(":/surrender.png")));
            ui->btnOpenImage->setText("Surrender");
            if (SettingsDialog::useAccelerometer())
                fixCurrentOrientation();
            _isPaused = false;

            QProgressDialog *progress = new QProgressDialog("Generating puzzle...", "Cancel", 0, rows * cols, this);
            progress->setWindowTitle("Generating puzzle...");
#ifndef MOBILE
            if (rows * cols < 20)
#endif
                progress->show();


            if (board != 0)
            {
                board->deleteLater();
            }

            ui->graphicsView->resetTransform();
            board = new JigsawPuzzleBoard(ui->graphicsView);
            board->setBackgroundBrush(QBrush(SettingsDialog::boardBackground()));
            ui->graphicsView->setScene(board);
            board->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());

            connect(board, SIGNAL(loadProgressChanged(int)), progress, SLOT(setValue(int)));
            connect(board, SIGNAL(gameStarted()), progress, SLOT(deleteLater()));
            connect(board, SIGNAL(gameStarted()), this, SLOT(initializeGame()));
            connect(board, SIGNAL(gameWon()), this, SLOT(onWon()));
            connect(board, SIGNAL(gameEnded()), this, SLOT(endGame()));

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
            if (SettingsDialog::useDropShadow())
                connect(board, SIGNAL(loaded()), board, SLOT(enableDropshadow()));
#endif

            board->startGame(pixmap, rows, cols);
            _currentScaleRatio = board->originalScaleRatio();
            ui->graphicsView->scale(_currentScaleRatio, _currentScaleRatio);
        }
        else
        {
            QMessageBox::warning(this, "Error", "Could not load the selected image.");
            _isPlaying = false;
        }
    }
    else if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Are you sure you want to abandon this game?", QMessageBox::Yes, QMessageBox::Cancel))
    {
        _isPlaying = false;
        timer->stop();
        ui->btnOpenImage->setIcon(QIcon(QPixmap(":/play.png")));
        ui->btnOpenImage->setText("New game");
        board->surrenderGame();
    }
}


void MainWindow::onWon()
{
    endGame();

    // Showing congratulations
#if defined(Q_WS_MAEMO_5)
    QMaemo5InformationBox::information(0, "<b>You rock!</b><br />Congratulations!<br />You've successfully solved the given puzzle!", 5000);
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
    _secsElapsed = 0;
    updateElapsedTimeLabel();
    ui->btnPause->show();

    // Snap tolerance
    if (JigsawPuzzleBoard *jpb = qobject_cast<JigsawPuzzleBoard*>(board))
        jpb->setTolerance(SettingsDialog::tolerance());

    // Accelerometer
    if (SettingsDialog::useAccelerometer())
        board->enableAccelerometer();
}

void MainWindow::endGame()
{
    _isPlaying = false;
    timer->stop();

    // Additional things
    ui->btnOpenImage->setIcon(QIcon(QPixmap(":/play.png")));
    ui->btnOpenImage->setText("New game...");
    ui->btnPause->hide();

    if (SettingsDialog::useAccelerometer())
    {
        board->disableAccelerometer();
        unfixCurrentOrientation();
    }
}

void MainWindow::showSettings()
{
    settings->exec();

    // Board background
    QColor bg = SettingsDialog::boardBackground();
    if (board->backgroundBrush().color() != bg)
    {
        board->setBackgroundBrush(QBrush(bg));
        if (!intro.isNull())
            intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    }

    // Snap tolerance
    if (_isPlaying)
        if (JigsawPuzzleBoard *jpb = qobject_cast<JigsawPuzzleBoard*>(board))
            jpb->setTolerance(SettingsDialog::tolerance());

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    // Drop shadows
    if (SettingsDialog::useDropShadow() && !board->isDropshadowActive() && intro.isNull())
        board->enableDropshadow();
    else if (!SettingsDialog::useDropShadow() && board->isDropshadowActive())
        board->disableDropshadow();
#endif
    // Accelerometer
    if (_isPlaying && SettingsDialog::useAccelerometer())
    {
        board->enableAccelerometer();
        fixCurrentOrientation();
    }
    else
    {
        board->disableAccelerometer();
        unfixCurrentOrientation();
    }
}

void MainWindow::about()
{
#if defined(Q_WS_MAEMO_5)
    QtHeWrapper::showHeAboutDialog(this,
                                   "Fun and addictive jigsaw puzzle game\nfor your mobile computer",
                                   "Licensed under the terms of EUPL 1.1 - 2011, Timur Kristóf",
                                   "http://gitorious.org/colorful-apps/pages/PuzzleMaster",
                                   "http://talk.maemo.org/showthread.php?t=67139",
                                   "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=venemo%40msn%2ecom&lc=US&item_name=to%20Timur%20Kristof%2c%20for%20Puzzle%20Master%20development&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted",
                                   "puzzle-master");
#else
    QMessageBox::information(this, "About", fetchAboutString(), QMessageBox::Ok);
#endif
}

void MainWindow::updateElapsedTimeLabel()
{
    if (_isPlaying)
    {
        _secsElapsed++;
    }
    else if (_isPlaying)
    {
        _secsElapsed = 0;
#if defined(MOBILE)
#else
        ui->lblTime->show();
#endif
    }
    QString str = "Elapsed " + QString::number(_secsElapsed) + " second";
    if (_secsElapsed > 1)
        str += "s";
#if defined(MOBILE)
    setWindowTitle(str);
#else
    ui->lblTime->setText(str);
#endif
}

void MainWindow::on_btnFullscreen_clicked()
{
    if (isFullScreen())
    {
#if defined(Q_OS_SYMBIAN)
        showMaximized();
#else
        showNormal();
#endif
    }
    else
    {
        showFullScreen();
    }
}

void MainWindow::pause()
{
    _isPaused = true;
    timer->stop();
    board->disable();
    ui->btnPause->setIcon(QIcon(QPixmap(":/unpause.png")));

    // Stopping accelerometer
    if (SettingsDialog::useAccelerometer())
        board->disableAccelerometer();
}

void MainWindow::unpause()
{
    _isPaused = false;
    timer->start();
    board->enable();
    ui->btnPause->setIcon(QIcon(QPixmap(":/pause.png")));

    // Starting accelerometer
    if (SettingsDialog::useAccelerometer())
        board->enableAccelerometer();
}

void MainWindow::togglePause()
{
    if (_isPlaying)
    {
        if (!_isPaused)
        {
            pause();
#if defined(Q_WS_MAEMO_5)
            QMaemo5InformationBox::information(this, "<b>Game paused!</b><br />You now can't move the pieces.", 2000);
#else
#endif
        }
        else
        {
            unpause();
#if defined(Q_WS_MAEMO_5)
            QMaemo5InformationBox::information(this, "<b>Game resumed!</b><br />Now you can move the pieces again.", 2000);
#else
#endif
        }
    }
}

void MainWindow::on_btnPause_clicked()
{
    togglePause();
}

void MainWindow::fixCurrentOrientation()
{
#if defined(Q_WS_MAEMO_5)
    if (width() > height())
        setAttribute(Qt::WA_Maemo5LandscapeOrientation);
    else
        setAttribute(Qt::WA_Maemo5PortraitOrientation);
#endif
}

void MainWindow::unfixCurrentOrientation()
{
#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5AutoOrientation);
#endif
}
