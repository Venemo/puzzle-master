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

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    static int rows();
    static int columns();
    static bool useAccelerometer();
#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    static bool useDropShadow();
#endif
    static QColor boardBackground();
    static int tolerance();

protected:
    void showEvent(QShowEvent *e);

private slots:
    void saveSettings();
    void on_btnClose_clicked();
    void on_btnBoardColor_clicked();
};

#endif // SETTINGSDIALOG_H
