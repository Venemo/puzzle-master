
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

#ifndef APPEVENTHANDLER_H
#define APPEVENTHANDLER_H

#include <QDeclarativeView>

class QTimer;

// This class handles platform differences and native events.
// It contains all the hacks necessary to make the app work on multiple platforms.

class AppEventHandler : public QObject
{
    Q_OBJECT

    QTimer *_fixedFPSTimer;

public:
    explicit AppEventHandler(QDeclarativeView *parent = 0);
    ~AppEventHandler();
    static bool nativeEventFilter(void *message);
    bool eventFilter(QObject *obj, QEvent *event);
    Q_INVOKABLE bool showAppSwitcherButton();
    Q_INVOKABLE void displayAppSwitcher();
    Q_INVOKABLE bool showPlatformFileDialog();
    Q_INVOKABLE void displayPlatformFileDialog();
    
signals:
    void windowActivated();
    void windowDeactivated();
    void platformFileDialogAccepted(QString fileUrl);
    
public slots:
    void adjustForPlaying();
    void adjustForUi();
    Q_INVOKABLE void enableFixedFPS();
    Q_INVOKABLE void disableFixedFPS();
    
};

#endif // APPEVENTHANDLER_H
