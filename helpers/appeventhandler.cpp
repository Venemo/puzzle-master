
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

#include <QEvent>
#include <QDebug>
#include <QFile>
#include <QAbstractEventDispatcher>
#include <QTimer>

#if QT_VERSION < 0x050000
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#elif QT_VERSION > 0x050000
#include <QQuickView>
#include <QWindow>
#endif

#if FORCE_PLATFORM_FILE_DIALOG
#include <QWidget>
#endif

#if defined(HAVE_SWIPELOCK) && !defined(Q_WS_X11)
#error What were you thinking? Swipe lock only works on MeeGo & X11
#endif

#if defined(HAVE_SWIPELOCK)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>
#endif

#if defined(Q_OS_SYMBIAN)
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif

#if defined(Q_WS_MAEMO_5)
#include <QDBusConnection>
#include <QDBusMessage>
#endif

#if defined(FORCE_PLATFORM_FILE_DIALOG) || defined(Q_WS_MAEMO_5)
#include <QFileDialog>
#endif

#if defined(Q_OS_BLACKBERRY) && !defined(Q_OS_BLACKBERRY_TABLET)
#include <bb/cascades/pickers/FilePicker>
using namespace bb::cascades::pickers;
#endif

#if defined(Q_OS_BLACKBERRY_TABLET)
#include <bps/event.h>
#include <bps/dialog.h>

static dialog_instance_t bbDialog = 0;
static const char *bbDialogFilters[] = { "*.jpg" };
#endif

#include "../helpers/appeventhandler.h"

#if defined(HAVE_SWIPELOCK)
static Atom customRegionAtom = 0;
static unsigned int customRegion[4];
static unsigned int defaultRegion[4] = { 0, 0, 0, 0 };
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
static QAbstractEventDispatcher::EventFilter previousNativeEventFilter = 0;
#endif

static QList<AppEventHandler*> appEventHandlers;

bool AppEventHandler::nativeEventFilter(void *message)
{
#if defined(Q_OS_BLACKBERRY_TABLET)
    // On BlackBerry, we get the file selector dialog results here

    bps_event_t * const event = static_cast<bps_event_t *>(message);

    if (event && bps_event_get_domain(event) == dialog_get_domain() && dialog_event_get_selected_index(event) == 1)
    {
        char **filePaths = 0;
        int pathCount;
        if (dialog_event_get_filebrowse_filepaths(event, &filePaths, &pathCount) != BPS_SUCCESS)
        {
            qWarning() << "Could not get paths from native file dialog";
            return false;
        }
        QString path = QFile::decodeName(filePaths[0]);
        qDebug() << path << "selected!";
        bps_free(filePaths);
        foreach (AppEventHandler *appEventHandler, appEventHandlers)
            emit appEventHandler->platformFileDialogAccepted(path);
    }
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    return previousNativeEventFilter ? previousNativeEventFilter(message) : false;
#else
    Q_UNUSED(message)
    return false;
#endif
}

AppEventHandler::AppEventHandler(QObject *parent)
    : QObject(parent)
{
    // Tell the static funtion to care about this instance
    appEventHandlers.append(this);

#if defined(HAVE_SWIPELOCK)
    // On Harmattan, these are needed to enable the swipe lock

    if (customRegionAtom == 0)
    {
        customRegionAtom = XInternAtom(QX11Info::display(), "_MEEGOTOUCH_CUSTOM_REGION", False);
        customRegion[0] = 0;
        customRegion[1] = 0;
        customRegion[2] = QApplication::desktop()->geometry().width();
        customRegion[3] = QApplication::desktop()->geometry().height();
    }
#endif

#if defined(Q_OS_SYMBIAN)
    // On Symbian, this is the way to disable automatic rotation

    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    if (appUi)
    {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    }
#endif

#if defined(Q_OS_BLACKBERRY_TABLET)
    // On BlackBerry, this tells the platform to send us dialog events
    dialog_request_events(0);
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Install the Qt event filter
    parent->installEventFilter(this);
    // Install the native event filter
    previousNativeEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(AppEventHandler::nativeEventFilter);
#endif
}

AppEventHandler::~AppEventHandler()
{
    appEventHandlers.removeAll(this);

#if defined(Q_OS_BLACKBERRY_TABLET)
    if (bbDialog)
        dialog_destroy(bbDialog);
#endif
}

// This method emits some events so that the QML UI can react to them
bool AppEventHandler::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::WindowActivate)
    {
        qDebug() << "Window activated";
        emit windowActivated();
    }
    else if (event->type() == QEvent::WindowDeactivate)
    {
        qDebug() << "Window deactivated";
        emit windowDeactivated();
    }
    return false;
}

// Adjusts the window for playing the game
void AppEventHandler::adjustForPlaying()
{
#if defined(HAVE_SWIPELOCK)
    XChangeProperty(QX11Info::display(), static_cast<QWidget*>(parent())->winId(), customRegionAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<unsigned char*>(customRegion), 4);
#elif defined(PUZZLE_MASTER_SAILFISH)
    QQuickView *view = static_cast<QQuickView*>(this->parent());
    view->setFlags(view->flags() | Qt::WindowOverridesSystemGestures);
#endif
}

// Adjusts the window for just displaying the user interface
void AppEventHandler::adjustForUi()
{
#if defined(HAVE_SWIPELOCK)
    XChangeProperty(QX11Info::display(), static_cast<QWidget*>(parent())->winId(), customRegionAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<unsigned char*>(defaultRegion), 4);
#elif defined(PUZZLE_MASTER_SAILFISH)
    QQuickView *view = static_cast<QQuickView*>(this->parent());
    view->setFlags(view->flags() & ~(Qt::WindowOverridesSystemGestures));
#endif
}

// Whether or not an app switcher button is necessary to display
bool AppEventHandler::showAppSwitcherButton()
{
#if defined(Q_WS_MAEMO_5)
    return true;
#else
    return false;
#endif
}

// When implemented, focuses out of the app and goes to the platform app switcher
void AppEventHandler::displayAppSwitcher()
{
#if defined(Q_WS_MAEMO_5)
    QDBusConnection::sessionBus().send(QDBusMessage::createSignal("/", "com.nokia.hildon_desktop", "exit_app_view"));
#else
    qDebug() << "displayAppSwitcher is not implemented for the current platform";
#endif
}

// Determines whether a platform specific file dialog should be used or the platform supports one of the QML models
bool AppEventHandler::showPlatformFileDialog()
{
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_BLACKBERRY) || defined(FORCE_PLATFORM_FILE_DIALOG)
    return true;
#else
    return false;
#endif
}

void AppEventHandler::filesSelected(QStringList files)
{
    foreach (const QString &s, files)
    {
        emit this->platformFileDialogAccepted(s);
    }
}

// Displays the platform file selector dialog when available
void AppEventHandler::displayPlatformFileDialog()
{
#if defined(Q_OS_BLACKBERRY_TABLET)
    // On BlackBerry, we must use a specific API to display the platform file dialog
    if (bbDialog)
        dialog_destroy(bbDialog);
    dialog_create_filebrowse(&bbDialog);
    dialog_set_filebrowse_filter(bbDialog, bbDialogFilters, 1);
    dialog_show(bbDialog);
#elif defined(Q_OS_BLACKBERRY) && !defined(Q_OS_BLACKBERRY_TABLET)
    FilePicker* filePicker = new FilePicker();
    filePicker->setType(FileType::Picture);
    filePicker->setMode(FilePickerMode::Picker);
    connect(filePicker, SIGNAL(fileSelected(QStringList)), this, SLOT(filesSelected(QStringList)));
    connect(filePicker, SIGNAL(pickerClosed()), filePicker, SLOT(deleteLater()));
    filePicker->open();
#elif defined(FORCE_PLATFORM_FILE_DIALOG)
    // Where there is no platform-specific dialog to use, fallback to good old QFileDialog
    QString path = QFileDialog::getOpenFileName(static_cast<QWidget*>(parent()), QString(), "/home/user/MyDocs", "Images (*.png *.jpeg *.jpg *.gif *.bmp)");
    qDebug() << "selected path is" << path;
    emit this->platformFileDialogAccepted(path);
#else
    qDebug() << "displayPlatformFileDialog is not implemented for the current platform";
#endif
}

void AppEventHandler::activateAppWindow()
{
#if QT_VERSION > 0x050000
    qDebug() << "activating app window";
    QWindow *window = static_cast<QWindow*>(this->parent());
    if (!window->isVisible())
    {
        window->showFullScreen();
    }
    window->raise();
    window->requestActivate();
#else
    qDebug() << "activating app window is not implemented, sorry";
#endif
}
