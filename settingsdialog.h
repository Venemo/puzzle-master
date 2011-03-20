#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    Ui::SettingsDialog *ui;
    QColor _boardBackground;
    QSettings settings;

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    static int rows();
    static void setRows(int value);
    static int columns();
    static void setColumns(int value);
    static bool useAccelerometer();
    static void setUseAccelerometer(bool value);
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    static bool useDropShadow();
    static void setUseDropShadow(bool value);
#endif
    static QColor boardBackground();
    static void setBoardBackground(QColor value);
    static int tolerance();
    static void setTolerance(int value);
    static bool enableScaling();
    static void setEnableScaling(bool value);

protected:
    void showEvent(QShowEvent *e);

private slots:
    void saveSettings();
    void on_btnClose_clicked();
    void on_btnBoardColor_clicked();
};

#endif // SETTINGSDIALOG_H
