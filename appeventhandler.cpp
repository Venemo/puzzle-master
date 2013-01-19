
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

#include <QDesktopWidget>
#include <QApplication>
#include <QEvent>
#include <QDebug>
#include <QFile>
#include <QAbstractEventDispatcher>

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

#if defined(Q_WS_MAEMO_5)
#include <QFileDialog>
#endif

#if defined(Q_OS_BLACKBERRY_TABLET)
#include <bps/event.h>
#include <bps/dialog.h>

static dialog_instance_t bbDialog = 0;
static const char *bbDialogFilters[] = { "*.jpg" };
#endif

#include "appeventhandler.h"

#if defined(HAVE_SWIPELOCK)
static Atom customRegionAtom = 0;
static unsigned int customRegion[4];
static unsigned int defaultRegion[4] = { 0, 0, 0, 0 };
#endif

static AppEventHandler *appEventHandler = 0;
static QAbstractEventDispatcher::EventFilter previousNativeEventFilter = 0;

AppEventHandler *AppEventHandler::instance(QWidget *parent)
{
    if (!appEventHandler)
        appEventHandler = new AppEventHandler(parent);

    return appEventHandler;
}

bool AppEventHandler::nativeEventFilter(void *message)
{
#if defined(Q_OS_BLACKBERRY_TABLET)
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
        emit appEventHandler->platformFileDialogAccepted(path);
    }
#endif
    return previousNativeEventFilter ? previousNativeEventFilter(message) : false;
}

AppEventHandler::AppEventHandler(QWidget *parent) :
    QObject(parent)
{
#if defined(HAVE_SWIPELOCK)
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
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    if (appUi)
    {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
    }
#endif

#if defined(Q_OS_BLACKBERRY_TABLET)
    dialog_request_events(0);
#endif

    parent->installEventFilter(this);
    previousNativeEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(AppEventHandler::nativeEventFilter);
}

AppEventHandler::~AppEventHandler()
{
#if defined(Q_OS_BLACKBERRY_TABLET)
    if (bbDialog)
        dialog_destroy(bbDialog);
#endif
}

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

void AppEventHandler::adjustForPlaying()
{
#if defined(HAVE_SWIPELOCK)
    XChangeProperty(QX11Info::display(), static_cast<QWidget*>(parent())->winId(), customRegionAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<unsigned char*>(customRegion), 4);
#endif
}

void AppEventHandler::adjustForUi()
{
#if defined(HAVE_SWIPELOCK)
    XChangeProperty(QX11Info::display(), static_cast<QWidget*>(parent())->winId(), customRegionAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<unsigned char*>(defaultRegion), 4);
#endif
}

bool AppEventHandler::showAppSwitcherButton()
{
#if defined(Q_WS_MAEMO_5)
    return true;
#else
    return false;
#endif
}

void AppEventHandler::displayAppSwitcher()
{
#if defined(Q_WS_MAEMO_5)
    QDBusConnection::sessionBus().send(QDBusMessage::createSignal("/", "com.nokia.hildon_desktop", "exit_app_view"));
#else
    qDebug() << "displayAppSwitcher is not implemented for the current platform";
#endif
}

bool AppEventHandler::showPlatformFileDialog()
{
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_BLACKBERRY_TABLET)
    return true;
#else
    return false;
#endif
}

void AppEventHandler::displayPlatformFileDialog()
{
#if defined(Q_WS_MAEMO_5)
    QString path = QFileDialog::getOpenFileName(static_cast<QWidget*>(parent()), QString(), "/home/user/MyDocs", "Images (*.png *.jpeg *.jpg *.gif *.bmp)");
    qDebug() << "selected path is" << path;
    emit this->platformFileDialogAccepted(path);
#elif defined(Q_OS_BLACKBERRY)
    if (bbDialog)
        dialog_destroy(bbDialog);
    dialog_create_filebrowse(&bbDialog);
    dialog_set_filebrowse_filter(bbDialog, bbDialogFilters, 1);
    dialog_show(bbDialog);
#else
    qDebug() << "displayPlatformFileDialog is not implemented for the current platform";
#endif
}
