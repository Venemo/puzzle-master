#include <QApplication>
#include <QtDeclarative>
#include <QSettings>
#include <QLibraryInfo>

#include "util.h"
#include "puzzleboard.h"
#include "puzzleitem.h"

QSettings *settings;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::addLibraryPath("./plugins");
    QApplication::setApplicationName("Puzzle Master");
    QApplication::setOrganizationName("Venemo");
    QApplication::setApplicationVersion(QString(APP_VERSION));

    QString langCode;
    langCode = getenv("LANG");
    if (langCode.isEmpty() || langCode == "C")
        langCode = QLocale::system().name();

    QTranslator tQt, tApp;
    tQt.load("qt_" + langCode, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    tApp.load("puzzle-master_" + langCode, ":/translations");
    QApplication::installTranslator(&tQt);
    QApplication::installTranslator(&tApp);

    qsrand((uint)QTime::currentTime().msec());
    settings = new QSettings();

    qmlRegisterType<PuzzleBoard>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    //qmlRegisterUncreatableType<PuzzleItem>("net.venemo.puzzlemaster", 2, 0, "PuzzleItem", "This item will be created progmatically");

    QDeclarativeView view;
    QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setSource(QUrl("qrc:/qml/meego/AppWindow.qml"));
    view.showFullScreen();

    return app.exec();
}
