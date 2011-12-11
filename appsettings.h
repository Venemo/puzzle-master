#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>

#include "util.h"

// Setting property
#define SETTINGPROPERTY(type, name, settername, signalname, settingKey, defaultValue) \
    inline type name() const { return _backend.value(settingKey, defaultValue).value<type>(); } \
    inline void settername (const type &value) { _backend.setValue(settingKey, value); emit signalname (); }

// Increase this when anything changes in the settings
#define APPSETTINGS_VERSION 1

class QSettings;

class AppSettings : public QObject
{
    Q_OBJECT
    GENPROPERTY_R(bool, _areSettingsDeleted, areSettingsDeleted)
    Q_PROPERTY(bool areSettingsDeleted READ areSettingsDeleted NOTIFY areSettingsDeletedChanged)

    QSettings _backend;

public:
    explicit AppSettings(QObject *parent = 0);

    SETTINGPROPERTY(int, rows, setRows, rowsChanged, "rows", 3)
    SETTINGPROPERTY(int, columns, setColumns, columnsChanged, "columns", 4)

signals:
    void areSettingsDeletedChanged();
    void rowsChanged();
    void columnsChanged();

};

#endif // APPSETTINGS_H
