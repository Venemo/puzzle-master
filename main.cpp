
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
#include <QDeclarativeContext>
#include <QSettings>
#include <QLibraryInfo>
#include <QDesktopWidget>
#include <QFile>
#include <QTranslator>
#include <QDesktopServices>

#if defined(HAVE_OPENGL)
#include <QGLWidget>
#endif

#if defined(HAVE_APPLAUNCHERD)
#include <MDeclarativeCache>
#endif

#if defined (HAVE_DEVICEINFO)
#include <QSystemDeviceInfo>
#endif

#include "util.h"
#include "puzzleboard.h"
#include "appsettings.h"
#include "appeventhandler.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("./plugins");
    QApplication::setApplicationName("Puzzle Master");
    QApplication::setOrganizationName("Venemo");
    QApplication::setApplicationVersion(QString(APP_VERSION));

    // Initiating QApplication and QDeclarativeView

    QApplication *app;
    QDeclarativeView *view;

    // Using applauncherd when available

#if defined(HAVE_APPLAUNCHERD)
    qDebug() << "Puzzle Master is using MDeclarativeCache";
    app = MDeclarativeCache::qApplication(argc, argv);
    view = MDeclarativeCache::qDeclarativeView();
#else
    app = new QApplication(argc, argv);
    view = new QDeclarativeView();
#endif

    // Some more initializations

    AppEventHandler *appEventHandler = new AppEventHandler(view);
    QObject::connect(view->engine(), SIGNAL(quit()), app, SLOT(quit()));
    qsrand((uint)QTime::currentTime().msec());
    qmlRegisterType<PuzzleBoard>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    qmlRegisterType<AppSettings>("net.venemo.puzzlemaster", 2, 0, "AppSettings");

    // Checking for translations

    QString langCode(getenv("LANG"));
    if (langCode.isEmpty() || langCode == "C")
        langCode = QLocale::system().name();
    if (langCode.contains('.'))
        langCode = langCode.mid(0, langCode.lastIndexOf('.'));

    qDebug() << "Puzzle Master's current language code is" << langCode;

    if (QFile::exists(":/translations/puzzle-master_" + langCode + ".qm"))
    {
        qDebug() << "A translation for the language code" << langCode << "exists, loading it";
        QTranslator *translator = new QTranslator(app);
        translator->load("puzzle-master_" + langCode, ":/translations");
        QApplication::installTranslator(translator);
    }

    // Checking for OpenGL support

#if defined(HAVE_OPENGL)
    qDebug() << "Puzzle Master is using a QGLWidget viewport";
    QGLWidget *glWidget = new QGLWidget(QGLFormat(QGL::DoubleBuffer), view);
    view->setViewport(glWidget);
#endif

    // Checking for rotation support

    bool allowRotation = true;

#if defined (HAVE_DEVICEINFO)
    QtMobility::QSystemDeviceInfo *info = new QtMobility::QSystemDeviceInfo();
    qDebug() << "Puzzle Master is running on... Manufacturer:" << info->manufacturer() << "Model:" << info->model() << "Product name:" << info->productName();
    qDebug() << "Input method type is" << info->inputMethodType();
    allowRotation = (info->inputMethodType() & QtMobility::QSystemDeviceInfo::MultiTouch) || (info->inputMethodType() & QtMobility::QSystemDeviceInfo::Mouse);
    if (info->manufacturer() == "Nokia" && info->model() == "N8-00")
        allowRotation = false;
    delete info;
#endif

#if defined(DISABLE_ROTATION)
    allowRotation = false;
#endif

    qDebug() << "Puzzle Master has" << (allowRotation ? "enabled" : "disabled") << "rotation support.";

    // Checking for QML gallery plugin support

    bool allowGalleryModel = true;

#if defined(DISABLE_QMLGALLERY)
    allowGalleryModel = false;
#endif

    // Setting up the view

    view->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    view->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setRenderHint(QPainter::HighQualityAntialiasing, false);
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view->rootContext()->setContextProperty("initialSize", QApplication::desktop()->geometry().size());
    view->rootContext()->setContextProperty("allowRotation", allowRotation);
    view->rootContext()->setContextProperty("allowGalleryModel", allowGalleryModel);
    view->rootContext()->setContextProperty("appVersion", QString(APP_VERSION));
    view->rootContext()->setContextProperty("appEventHandler", appEventHandler);
    view->rootContext()->setContextProperty("picturesFolder", QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
    view->setSource(QUrl("qrc:/qml/other/AppWindow.qml"));
    view->showFullScreen();

    // Launching the app

    int result = app->exec();
    delete view;
    delete app;
    return result;
}
