#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "jigsawpuzzleboard.h"
#include "jigsawpuzzleitem.h"
#include "util.h"
#include <QtGui>
#include <QGLWidget>

#if defined(Q_WS_MAEMO_5)
#include <QtMaemo5>
#include <hildon-extras-1/hildon-extras/qt-he-wrapper.h>
#endif

static QString *aboutString = 0;

static void fetchAboutString()
{
    QFile file(":/about.txt");
    file.open(QIODevice::ReadOnly);
    *aboutString = QString::fromUtf8(file.readAll().constData());
    file.close();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(new QTimer(this)),
    settings(new SettingsDialog(this)),
    chooser(new ImageChooser(this)),
    highscores(new HighScoresDialog(this)),
    board(new JigsawPuzzleBoard(this)),
    _isPlaying(false),
    _currentScaleRatio(1)
{
    ui->setupUi(this);
    ui->lblTime->hide();

    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(elapsedSecond()));
    setFocus();

    QColor bg = SettingsDialog::boardBackground();
    board->setBackgroundBrush(QBrush(bg));
    intro = new QGraphicsTextItem("Please press the 'New game' button!");
    intro->setDefaultTextColor(QColor(0xFFFFFF - bg.rgb()));
    board->addItem(intro);
    board->setOriginalPixmapSize(QSize(intro->boundingRect().size().width(), intro->boundingRect().size().height()));

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

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60) || defined(Q_WS_WINCE)
void MainWindow::enterEvent(QEvent *event)
{
    if (_isPlaying)
        timer->start();
    QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
    if (timer->isActive())
        timer->stop();
    QMainWindow::leaveEvent(event);
}
#endif

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
            _currentScaleRatio = 1;
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
#if defined(Q_WS_MAEMO_5)
    QtHeWrapper::showHeAboutDialog(this,
                                   "Fun and addictive jigsaw puzzle game\nfor your mobile computer",
                                   "Licensed under the terms of EUPL 1.1 - 2011, Timur Kristóf",
                                   "http://gitorious.org/colorful-apps/pages/PuzzleMaster",
                                   "http://talk.maemo.org/showthread.php?t=67139",
                                   "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=venemo%40msn%2ecom&lc=US&item_name=to%20Timur%20Kristof%2c%20for%20Puzzle%20Master%20development&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted",
                                   "puzzle-master");
#else
    if (aboutString == 0)
        fetchAboutString();

    QMessageBox::information(this, "About", *aboutString, QMessageBox::Ok);
#endif

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
