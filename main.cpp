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

    QTranslator tQt, tApp;
    tQt.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    tApp.load("puzzle-master_" + QLocale::system().name(), ":/translations");

    //qDebug() << QLocale::system().name();

    QApplication::installTranslator(&tQt);
    QApplication::installTranslator(&tApp);

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
