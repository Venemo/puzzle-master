
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
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

#include <QSettings>
#include <QLibraryInfo>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QQuickView>
#include <QScreen>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <qqml.h>

#include "puzzleboarditem.h"
#include "helpers/appsettings.h"
#include "helpers/appeventhandler.h"
#include "puzzle/puzzlegame.h"

extern void loadTranslations();

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();

    // Some settings

    QCoreApplication::addLibraryPath("./plugins");
    QCoreApplication::setApplicationName("Puzzle Master");
    QCoreApplication::setOrganizationName("Venemo");
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));

    // Printing app version

    qDebug() << "Welcome to Puzzle Master! App version is" << QString(APP_VERSION);

    // Initializing app and view

    QGuiApplication *app = new QGuiApplication(argc, argv);
    QQuickView *view = new QQuickView();

    // Some wireup

    AppEventHandler *appEventHandler = new AppEventHandler(view);
    QObject::connect(view->engine(), SIGNAL(quit()), app, SLOT(quit()));
    qsrand((uint)QTime::currentTime().msec());
    qmlRegisterType<PuzzleBoardItem>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    qmlRegisterUncreatableType<PuzzleGame>("net.venemo.puzzlemaster", 2, 0, "PuzzleGame", "This type should not be used from QML.");
    qmlRegisterType<AppSettings>("net.venemo.puzzlemaster", 2, 0, "AppSettings");

    loadTranslations();

    // Checking for rotation and touchscreen support

    bool allowRotation = true, allowScrollbars = true;

#if defined(DISABLE_ROTATION)
    allowRotation = false;
#endif

#if defined(DISABLE_SCROLLBARS)
    allowScrollbars = false;
#endif

    qDebug() << "Puzzle Master has" << (allowRotation ? "enabled" : "disabled") << "rotation support.";
    qDebug() << "Scroll bars will" << (allowScrollbars ? "appear" : "not appear");

    // Checking for QML gallery plugin support

    bool allowGalleryModel = true;

#if defined(DISABLE_QMLGALLERY)
    allowGalleryModel = false;
#endif

    // Checking the size

    QSize initialSize = view->screen()->size();
    qDebug() << "initial size is" << initialSize;

    // Setting up the view

    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->rootContext()->setContextProperty("initialSize", initialSize);
    view->rootContext()->setContextProperty("allowRotation", allowRotation);
    view->rootContext()->setContextProperty("allowScrollbars", allowScrollbars);
    view->rootContext()->setContextProperty("allowGalleryModel", allowGalleryModel);
    view->rootContext()->setContextProperty("appVersion", QString(APP_VERSION));
    view->rootContext()->setContextProperty("appEventHandler", appEventHandler);

    qDebug() << Q_FUNC_INFO << "initialization took" << timer->elapsed() << "ms";
    timer->restart();

    view->setSource(QUrl("qrc:/qml/default/AppWindow.qml"));
    view->setTitle(QObject::tr("Puzzle Master"));
    view->showFullScreen();

    qDebug() << Q_FUNC_INFO << "setting the qml source took" << timer->elapsed() << "ms";
    delete timer;

    // Launching the app

    int result = app->exec();
    delete view;
    delete app;
    return result;
}
