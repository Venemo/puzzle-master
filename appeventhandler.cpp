
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

#include "appeventhandler.h"
#include <QDesktopWidget>
#include <QApplication>

#if defined(HAVE_SWIPELOCK) && !defined(Q_WS_X11)
#error What were you thinking? Swipe lock only works on MeeGo & X11
#endif

#if defined(HAVE_SWIPELOCK)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>
#endif

#if defined(HAVE_SWIPELOCK)
static Atom customRegionAtom = 0;
unsigned int customRegion[4];
unsigned int defaultRegion[4] = { 0, 0, 0, 0 };
#endif

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
