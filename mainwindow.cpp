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
    newgame(new NewGameDialog(this)),
    highscores(new HighScoresDialog(this)),
    board(new JigsawPuzzleBoard(this)),
    _isPlaying(false),
    _isPaused(false),
    _wasPaused(false),
    _canPause(false),
    _currentScaleRatio(1)
{
    ui->setupUi(this);

    ui->btnPause->hide();
    ui->actionPause->setVisible(false);
    ui->btnSurrender->hide();
    ui->actionSurrender->setVisible(false);
    ui->btnHelp->hide();

    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateElapsedTimeLabel()));
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setFocus();

    QColor bg = SettingsDialog::boardBackground();
    board->setBackgroundBrush(QBrush(bg));
    intro = new QGraphicsTextItem("Please press the new game button!");
    intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    board->addItem(intro);

    ui->graphicsView->setScene(board);
#if defined(HAVE_OPENGL)
    ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer), this));
#endif

#if defined(MOBILE)
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    delete ui->lblTime;
    ui->horizontalLayout->setContentsMargins(10, 0, 10, 0);
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
    if (_canPause && _isPlaying && !_wasPaused && _isPaused && event->type() == QEvent::WindowActivate)
    {
        unpause();
    }
    else if (_canPause && _isPlaying && event->type() == QEvent::WindowDeactivate)
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

#if defined(MOBILE)
    if (isFullScreen())
    {
        ui->horizontalLayout->removeWidget(ui->btnFullscreen);
        ui->btnFullscreen->move(width() - ui->btnFullscreen->width(), height() - ui->btnFullscreen->height());
        ui->btnFullscreen->show();
    }
#endif

    if (intro != 0)
        return;

    // Scaling the graphics view's content

    QSizeF oldscenesize = board->sceneRect().size();

    if (SettingsDialog::enableScaling())
    {
        // Calculating scale ratio
        qreal oldScaleRatio = _currentScaleRatio;
        QSize scaledPixmapSize = board->originalPixmapSize();
        scaledPixmapSize.scale(ui->graphicsView->size(), Qt::KeepAspectRatio);
        _currentScaleRatio = min<qreal>((qreal)scaledPixmapSize.width() / (qreal)board->originalPixmapSize().width(),
                                        (qreal)scaledPixmapSize.height() / (qreal)board->originalPixmapSize().height());

        //qDebug() << "new size" << ui->graphicsView->size() << "ratio" << _currentScaleRatio;

        // Setting scene rect and scale
        board->setSceneRect(0, 0, ui->graphicsView->width() / _currentScaleRatio, ui->graphicsView->height() / _currentScaleRatio);
        if (0.98 < _currentScaleRatio && 1.02 > _currentScaleRatio)
            ui->graphicsView->resetTransform();
        else
            ui->graphicsView->scale(_currentScaleRatio / oldScaleRatio, _currentScaleRatio / oldScaleRatio);
    }
    else
    {
        ui->graphicsView->resetTransform();
        board->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
    }

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

    if (event)
        event->accept();
    //qDebug() << "current scale ratio is" << _currentScaleRatio;
}

void MainWindow::showHighScores()
{
    highscores->exec();
}

void MainWindow::newGame()
{
    if (!_isPlaying)
    {
        if (!QDialog::Accepted == newgame->exec())
            return;

        int rows = SettingsDialog::rows(), cols = SettingsDialog::columns();
        _isPlaying = true;
        _canPause = false;

        if ((SettingsDialog::startInFullscreen() && !isFullScreen()))// || !SettingsDialog::startInFullscreen() && isFullScreen())
            toggleFullscreen();

        applyViewportSettings();

        QPixmap pixmap;
        if (pixmap.load(newgame->getPictureFile()) && !pixmap.isNull())
        {
            ui->btnOpenImage->hide();
            ui->actionNew_game->setVisible(false);
            if (!isFullScreen())
            {
                ui->btnSurrender->show();
                ui->actionSurrender->setVisible(true);
            }

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
            board->setSceneRect(0, 0, ui->graphicsView->width(), ui->graphicsView->height());
            ui->graphicsView->setScene(board);

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
}

void MainWindow::surrender()
{
    if (_isPlaying && QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Are you sure you want to abandon this game?", QMessageBox::Yes, QMessageBox::Cancel))
    {
        endGame();

        _isPlaying = false;
        timer->stop();
        board->surrenderGame();
    }
}

void MainWindow::applyViewportSettings()
{
#if defined(HAVE_OPENGL)
        if (SettingsDialog::useOpenGl() && !ui->graphicsView->viewport()->inherits("QGLWidget"))
        {
            ui->graphicsView->viewport()->deleteLater();
            ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::DoubleBuffer), this));
        }
        else if (!SettingsDialog::useOpenGl() && ui->graphicsView->viewport()->inherits("QGLWidget"))
        {
            ui->graphicsView->viewport()->deleteLater();
            ui->graphicsView->setViewport(new QWidget(this));
        }
#endif
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
    _secsElapsed = 0;
    updateElapsedTimeLabel();
    _canPause = true;

    // This is to set the correct icon on the button and to start the timer
    unpause();

#if defined (MOBILE)
    if (!isFullScreen())
    {
        ui->btnPause->show();
        ui->actionPause->setVisible(true);
    }
#else
    ui->btnPause->show();
    ui->actionPause->setVisible(true);
#endif

    // Snap tolerance
    if (JigsawPuzzleBoard *jpb = qobject_cast<JigsawPuzzleBoard*>(board))
        jpb->setTolerance(SettingsDialog::tolerance());
}

void MainWindow::endGame()
{
    _isPlaying = false;
    timer->stop();

    // Additional things
#if defined(MOBILE)
    if (!isFullScreen())
#endif
        ui->btnOpenImage->show();

    ui->actionNew_game->setVisible(true);
    ui->btnSurrender->hide();
    ui->actionSurrender->setVisible(false);
    ui->btnPause->hide();
    ui->actionPause->setVisible(false);

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

    //Viewport
    applyViewportSettings();
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
#ifndef MOBILE
        ui->lblTime->setText("Elapsed 0 second");
        ui->lblTime->show();
#endif
    }

    QString str = "Elapsed " + QString::number(_secsElapsed) + " second";
    if (_secsElapsed > 1)
        str += "s";
#ifndef MOBILE
    if (_isPaused)
        str = "Game paused! " + str;
#endif

#if defined(MOBILE)
    setWindowTitle(str);
#else
    ui->lblTime->setText(str);
#endif
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen())
    {
#if defined(Q_OS_SYMBIAN)
        showMaximized();
#else
        showNormal();
#endif

#if defined(MOBILE)
        ui->horizontalLayout->addWidget(ui->btnFullscreen);
        ui->btnAbout->show();
        ui->btnFullscreen->show();
        //ui->btnHelp->show();
        if (!_isPlaying)
            ui->btnOpenImage->show();
        ui->btnPause->show();
        ui->btnSettings->show();
        if (_isPlaying)
            ui->btnSurrender->show();
#else
        ui->menuBar->show();
#endif
    }
    else
    {
        showFullScreen();

#if defined(MOBILE)
        ui->btnAbout->hide();
        ui->btnFullscreen->hide();
        //ui->btnHelp->hide();
        ui->btnOpenImage->hide();
        ui->btnPause->hide();
        ui->btnSettings->hide();
        ui->btnSurrender->hide();
#else
        ui->menuBar->hide();
#endif
    }

    // This is what it takes to fully update the appearance
    QApplication *app = (QApplication*) QApplication::instance();
    app->processEvents();
    update();
    app->processEvents();
    layout()->update();
    app->processEvents();
}

void MainWindow::pause()
{
    qDebug() << "game paused";
    _isPaused = true;
    timer->stop();
    board->disable();
    ui->btnPause->setIcon(QIcon(QPixmap(":/unpause.png")));
    ui->actionPause->setText("Resume");

    // Stopping accelerometer
    if (SettingsDialog::useAccelerometer())
        board->disableAccelerometer();
}

void MainWindow::unpause()
{
    qDebug() << "game resumed";
    _isPaused = false;
    timer->start();
    board->enable();
    ui->btnPause->setIcon(QIcon(QPixmap(":/pause.png")));
    ui->actionPause->setText("Pause");

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
#elif defined (MOBILE)
            QMessageBox::information(this, "Game paused!", "You now can't move the pieces.", QMessageBox::Ok);
#else
            ui->lblTime->setText("Game paused!");
#endif
        }
        else
        {
#if defined(Q_WS_MAEMO_5)
            QMaemo5InformationBox::information(this, "<b>Game resumed!</b><br />Now you can move the pieces again.", 2000);
#elif defined (MOBILE)
            QMessageBox::information(this, "Game resumed!", "Now you can move the pieces again.", QMessageBox::Ok);
#else
            updateElapsedTimeLabel();
#endif
            unpause();
        }
    }
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
