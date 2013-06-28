
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
#include <QElapsedTimer>
#include <QDebug>

#if defined(HAVE_OPENGL)
#include <QGLWidget>
#endif

#if defined(HAVE_APPLAUNCHERD)
#include <MDeclarativeCache>
#endif

#if defined (HAVE_DEVICEINFO) || defined (Q_OS_SYMBIAN)
#include <QSystemDeviceInfo>
#endif

#ifndef TEXT_SCALING_FACTOR
#define TEXT_SCALING_FACTOR 1
#endif
#ifndef UI_SCALING_FACTOR
#define UI_SCALING_FACTOR 1
#endif
#ifndef ENABLE_MOBILE_TWEAKS
#define ENABLE_MOBILE_TWEAKS false
#endif

#include "puzzleboarditem_qt4.h"
#include "helpers/appsettings.h"
#include "helpers/appeventhandler.h"
#include "puzzle/puzzlegame.h"

extern void loadTranslations();

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QElapsedTimer *timer = new QElapsedTimer();
    timer->start();

    // Some settings

#if Q_OS_BLACKBERRY
    QCoreApplication::addLibraryPath("app/native/lib");
    QCoreApplication::addLibraryPath("app/native/plugins");
#endif
    QCoreApplication::addLibraryPath("./plugins");
    QCoreApplication::setApplicationName("Puzzle Master");
    QCoreApplication::setOrganizationName("Venemo");
    QCoreApplication::setApplicationVersion(QString(APP_VERSION));

    // Printing app version

    qDebug() << "Welcome to Puzzle Master! App version is" << QString(APP_VERSION);

    // Initializing QApplication and QDeclarativeView

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

#if Q_OS_BLACKBERRY
    app->setStartDragTime(600);
    app->setStartDragDistance(20);
#endif

    // Some wireup

    AppEventHandler *appEventHandler = new AppEventHandler(view);
    QObject::connect(view->engine(), SIGNAL(quit()), app, SLOT(quit()));
    qsrand((uint)QTime::currentTime().msec());
    qmlRegisterType<PuzzleBoardItem>("net.venemo.puzzlemaster", 2, 0, "PuzzleBoard");
    qmlRegisterUncreatableType<PuzzleGame>("net.venemo.puzzlemaster", 2, 0, "PuzzleGame", "This type should not be used from QML.");
    qmlRegisterType<AppSettings>("net.venemo.puzzlemaster", 2, 0, "AppSettings");

    loadTranslations();

    // Checking for OpenGL support

#if defined(HAVE_OPENGL)
    qDebug() << "Puzzle Master is using a QGLWidget viewport";
    QGLWidget *glWidget = new QGLWidget(QGLFormat(QGL::DoubleBuffer), view);
    view->setViewport(glWidget);
#endif

    // Checking for rotation and touchscreen support

    bool allowRotation = true, allowScrollbars = true;

#if defined (HAVE_DEVICEINFO_CHECK)
    QtMobility::QSystemDeviceInfo *info = new QtMobility::QSystemDeviceInfo();
    qDebug() << "Puzzle Master is running on... Manufacturer:" << info->manufacturer() << "Model:" << info->model() << "Product name:" << info->productName();
    qDebug() << "Input method type is" << info->inputMethodType();
    allowRotation = (info->inputMethodType() & QtMobility::QSystemDeviceInfo::MultiTouch) || (info->inputMethodType() & QtMobility::QSystemDeviceInfo::Mouse);
    allowScrollbars = !(info->inputMethodType() & QtMobility::QSystemDeviceInfo::MultiTouch) && !(info->inputMethodType() & QtMobility::QSystemDeviceInfo::SingleTouch);
    delete info;
#endif

#if defined (Q_OS_SYMBIAN)
    QtMobility::QSystemDeviceInfo *info = new QtMobility::QSystemDeviceInfo();
    if (info->manufacturer() == "Nokia" && info->model() == "N8-00")
        allowRotation = false;
    else if (QSysInfo::symbianVersion() < QSysInfo::SV_SF_3)
        allowRotation = false;
    else
        allowRotation = true;
    delete info;
#endif

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

    QSize initialSize = QApplication::desktop()->screenGeometry(view).size();
#if defined(Q_OS_BLACKBERRY)
    if (initialSize.height() > initialSize.width())
        initialSize = QSize(initialSize.height(), initialSize.width());
#endif

    qDebug() << "initial size is" << initialSize;

    // Check scaling factors

    double uiScalingFactor = UI_SCALING_FACTOR;
    double textScalingFactor = TEXT_SCALING_FACTOR;
    bool enableMobileTweaks = ENABLE_MOBILE_TWEAKS;

#if defined(Q_OS_BLACKBERRY)
    // This is for the blackberry Q10 and Dev Alpha C
    if (initialSize.height() == initialSize.width())
    {
        uiScalingFactor = 1.3;
        textScalingFactor = 1.08;
        view->setAttribute(Qt::WA_LockPortraitOrientation);
    }
    else
    {
        view->setAttribute(Qt::WA_LockLandscapeOrientation);
    }
#endif

    qDebug() << "UI scaling factor:" << uiScalingFactor << "text scaling factor:" << textScalingFactor;

    // Setting up the view

    view->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    view->setOptimizationFlag(QGraphicsView::DontSavePainterState);
    view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    view->setRenderHint(QPainter::Antialiasing, false);
    view->setRenderHint(QPainter::HighQualityAntialiasing, false);
#if defined(Q_OS_BLACKBERRY_TABLET) || defined(Q_OS_BLACKBERRY)
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#else
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
#endif
    view->setAttribute(Qt::WA_OpaquePaintEvent);
    view->setAttribute(Qt::WA_NoSystemBackground);
    view->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view->viewport()->setAttribute(Qt::WA_NoSystemBackground);
    view->rootContext()->setContextProperty("initialSize", initialSize);
    view->rootContext()->setContextProperty("allowRotation", allowRotation);
    view->rootContext()->setContextProperty("allowScrollbars", allowScrollbars);
    view->rootContext()->setContextProperty("allowGalleryModel", allowGalleryModel);
    view->rootContext()->setContextProperty("appVersion", QString(APP_VERSION));
    view->rootContext()->setContextProperty("appEventHandler", appEventHandler);
    view->rootContext()->setContextProperty("uiScalingFactor", uiScalingFactor);
    view->rootContext()->setContextProperty("textScalingFactor", textScalingFactor);
    view->rootContext()->setContextProperty("enableMobileTweaks", enableMobileTweaks);

    qDebug() << Q_FUNC_INFO << "initialization took" << timer->elapsed() << "ms";
    timer->restart();

    view->setSource(QUrl("qrc:/qml/default-qt4/AppWindow.qml"));
    view->setWindowTitle(QObject::tr("Puzzle Master"));
    view->showFullScreen();
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    qDebug() << Q_FUNC_INFO << "setting the qml source took" << timer->elapsed() << "ms";
    delete timer;

    // Launching the app

    int result = app->exec();
    delete view;
    delete app;
    return result;
}
