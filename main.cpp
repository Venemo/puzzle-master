
// This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010-2011, Timur Kristóf <venemo@fedoraproject.org>

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

    qDebug() << "Puzzle Master's current language code is" << langCode;

    QTranslator tQt, tApp;
    tQt.load("qt_" + langCode, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    tApp.load("puzzle-master_" + langCode, ":/translations");

    qsrand((uint)QTime::currentTime().msec());
    qmlRegisterType<PuzzleBoard>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    qmlRegisterType<AppSettings>("net.venemo.puzzlemaster", 2, 0, "AppSettings");

    QApplication *app;
    QDeclarativeView *view;

#if defined(HAVE_APPLAUNCHERD)
    qDebug() << "Puzzle Master is using MDeclarativeCache";
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
    qDebug() << "Puzzle Master is using QGLWidget viewport";
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
#if defined(HAVE_OPENGL)
    delete glWidget;
#endif
    delete view;
    delete app;
    return result;
}
