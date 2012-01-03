
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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QByteArray>
#include <QStringList>

#include "util.h"

// Setting property
#define SETTINGPROPERTY(type, name, settername, signalname, settingKey, defaultValue) \
    inline type name() const { return _backend.value(settingKey, defaultValue).value<type>(); } \
    inline void settername (const type &value) { _backend.setValue(settingKey, value); emit signalname (); }

class QSettings;

class AppSettings : public QObject
{
    Q_OBJECT
    GENPROPERTY_R(bool, _areSettingsDeleted, areSettingsDeleted)
    Q_PROPERTY(bool areSettingsDeleted READ areSettingsDeleted NOTIFY areSettingsDeletedChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(int snapDifficulty READ snapDifficulty WRITE setSnapDifficulty NOTIFY snapDifficultyChanged)

    QSettings _backend;

public:
    explicit AppSettings(QObject *parent = 0);

    SETTINGPROPERTY(int, rows, setRows, rowsChanged, "rows", 3)
    SETTINGPROPERTY(int, columns, setColumns, columnsChanged, "columns", 4)
    SETTINGPROPERTY(int, snapDifficulty, setSnapDifficulty, snapDifficultyChanged, "snapDifficulty", 1)
    SETTINGPROPERTY(QByteArray, customImageListData, setCustomImageListData, customImageListDataChanged, "customImageListData", QByteArray())

    Q_INVOKABLE QStringList loadCustomImages();
    Q_INVOKABLE bool addCustomImage(const QString &url);
    Q_INVOKABLE bool removeCustomImage(const QString &url);

signals:
    void areSettingsDeletedChanged();
    void rowsChanged();
    void columnsChanged();
    void snapDifficultyChanged();
    void customImageListDataChanged();
    void customImageAlreadyAdded(const QString &url);

};

#endif // APPSETTINGS_H
