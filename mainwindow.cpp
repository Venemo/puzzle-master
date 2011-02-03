#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "puzzlewidget.h"
#include  <QtGui>

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
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
        accelerometer(new QtMobility::QAccelerometer(this)),
#endif
        _isPlaying(false)
{
    ui->setupUi(this);
    ui->lblTime->hide();

    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(elapsedSecond()));
    setFocus();

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    connect(accelerometer, SIGNAL(readingChanged()), this, SLOT(accelerometerReadingChanged()));
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
        qDeleteAll(ui->frame->children());

        QProgressDialog *progress = new QProgressDialog(this);
        progress->setWindowTitle("Generating puzzle...");
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
#else
        progress->setFixedWidth(450);
#endif
        progress->setMaximum(rows * cols);
        progress->show();

        QList<PuzzlePiece*> list1;
        QList<PuzzleWidget*> list2;

        QPixmap pixmap;
        if (pixmap.load(chooser->getPictureFile()))
        {
            pixmap = pixmap.scaled(ui->frame->width(), ui->frame->height(), Qt::KeepAspectRatio);
            QSize unit(pixmap.width() / cols, pixmap.height() / rows);
            QPainter p;

            for (int i = 0; i < cols; i++)
            {
                for (int j = 0; j < rows; j++)
                {
                    QPixmap px(pixmap.width(), pixmap.height());
                    px.fill(Qt::transparent);
                    p.begin(&px);
                    p.setRenderHint(QPainter::Antialiasing);

                    QPainterPath clip;
                    clip.addRect(i * unit.width(), j * unit.height(), unit.width(), unit.height());
                    p.setClipPath(clip);
                    p.setClipping(true);

                    p.drawPixmap(0, 0, pixmap);
                    p.end();
                    PuzzleWidget *w = new PuzzleWidget(px, unit, ui->frame);
                    w->setPosition(QPoint(i, j));
                    list1.append(w);
                    list2.append(w);
                    connect(w, SIGNAL(noNeighbours()), this, SLOT(onWon()));
                    w->show();
                    progress->setValue(i * rows + j + 1);
                }
            }
            PuzzlePiece::setNeighbours(&list1, cols, rows);
            PuzzleWidget::shuffle(&list2, cols, rows, ui->frame->width() - unit.width() - 1, ui->frame->height() - unit.height() - 1);

            initializeGame();
        }
        else
        {
            QMessageBox::warning(this, "Error", "Could not load the selected image.");
            _isPlaying = false;
        }

        progress->deleteLater();

    }
    else if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Are you sure you want to abandon this game?", QMessageBox::Yes, QMessageBox::Cancel))
    {
        ui->lblTime->hide();
        qDeleteAll(ui->frame->children());
        endGame();
    }
}

void MainWindow::accelerometerReadingChanged()
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    QtMobility::QAccelerometerReading *reading = accelerometer->reading();
    //qDebug() << reading->x() << reading->y() << reading->z();
    for (int i = 0; i < ui->frame->children().count(); i++)
    {
        PuzzleWidget *widget = (PuzzleWidget*) ui->frame->children().operator [](i);
        if (widget->canMerge())
            widget->move(widget->pos().x() - reading->x() * widget->weight() / 2, widget->pos().y() + reading->y() * widget->weight() / 2);
    }
#endif
}

void MainWindow::onWon()
{
    // stopping the widget from moving
    PuzzleWidget *widget = (PuzzleWidget*) ui->frame->children().operator [](0);
    widget->setMerge(false);

    // ending the game
    endGame();

    // moving the widget to 0,0
    QPropertyAnimation *anim = new QPropertyAnimation(widget, "pos", this);
    anim->setEndValue(QPoint(0, 0));
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

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

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    if (SettingsDialog::useAccelerometer())
    {
        accelerometer->connectToBackend();
        accelerometer->start();
    }
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
    accelerometer->stop();
#endif
}

void MainWindow::on_actionSettings_triggered()
{
    settings->exec();
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
    if (SettingsDialog::useAccelerometer() && !accelerometer->isActive())
        accelerometer->start();
    else if (!SettingsDialog::useAccelerometer() && accelerometer->isActive())
        accelerometer->stop();
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
