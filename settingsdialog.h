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

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    static int rows();
    static int columns();
    static bool useAccelerometer();
    static bool useDropShadow();
    static QColor boardBackground();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void saveSettings();
    void on_btnClose_clicked();
};

#endif // SETTINGSDIALOG_H
