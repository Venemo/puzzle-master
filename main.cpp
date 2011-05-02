#include <QtGui>
#include "mainwindow.h"
#include "util.h"

QSettings *settings;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::addLibraryPath("./plugins");
    QApplication::setApplicationName("Puzzle Master");
    QApplication::setOrganizationName("Venemo");
    QApplication::setApplicationVersion(QString(APP_VERSION));

    qsrand((uint)QTime::currentTime().msec());
    settings = new QSettings();

    MainWindow w;
#if defined(Q_OS_SYMBIAN)
    w.showFullScreen();
#elif defined(MOBILE)
    w.showMaximized();
#else
    w.show();
#endif

    return app.exec();
}
