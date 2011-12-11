#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QSettings>
#include <QLibraryInfo>

#if defined(HAVE_OPENGL)
#include <QGLWidget>
#endif

#if defined(HAVE_APPLAUNCHERD)
#include <MDeclarativeCache>
#endif

#include "util.h"
#include "puzzleboard.h"
#include "appsettings.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");
    QApplication::setApplicationName("Puzzle Master");
    QApplication::setOrganizationName("Venemo");
    QApplication::setApplicationVersion(QString(APP_VERSION));

    QString langCode;
    langCode = getenv("LANG");
    if (langCode.isEmpty() || langCode == "C")
        langCode = QLocale::system().name();

    qDebug() << "current language code is" << langCode;

    QTranslator tQt, tApp;
    tQt.load("qt_" + langCode, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    tApp.load("puzzle-master_" + langCode, ":/translations");

    qsrand((uint)QTime::currentTime().msec());
    qmlRegisterType<PuzzleBoard>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    qmlRegisterType<AppSettings>("net.venemo.puzzlemaster", 2, 0, "AppSettings");

    QApplication *app;
    QDeclarativeView *view;

#if defined(HAVE_APPLAUNCHERD)
    app = MDeclarativeCache::qApplication(argc, argv);
    view = MDeclarativeCache::qDeclarativeView();
#else
    app = new QApplication(argc, argv);
    view = new QDeclarativeView();
#endif

    QApplication::installTranslator(&tQt);
    QApplication::installTranslator(&tApp);
    QObject::connect(view->engine(), SIGNAL(quit()), app, SLOT(quit()));

#if defined(HAVE_OPENGL)
    QGLWidget *glWidget = new QGLWidget();
    view->setViewport(glWidget);
#endif

    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->setSource(QUrl("qrc:/qml/other/AppWindow.qml"));
    view->showFullScreen();

    int result = app->exec();
    delete glWidget;
    delete view;
    delete app;
    return result;
}
