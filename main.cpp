#include <QtGui>
#include "mainwindow.h"
#include "util.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::addLibraryPath("./plugins");
    QApplication::setApplicationName("Puzzle Master");
    QApplication::setOrganizationName("Venemo");
    QApplication::setApplicationVersion(QString(APP_VERSION));

    qsrand((uint)QTime::currentTime().msec());

    MainWindow w;
#if defined(MOBILE)
    w.showMaximized();
#else
    w.show();
#endif

    return app.exec();
}
