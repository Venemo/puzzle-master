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
    static void setRows(int value);
    static int columns();
    static void setColumns(int value);
    static bool useAccelerometer();
    static void setUseAccelerometer(bool value);
    static bool useOpenGl();
    static void setUseOpenGl(bool value);
    static bool startInFullscreen();
    static void setStartInFullscreen(bool value);
    static bool useDropShadow();
    static void setUseDropShadow(bool value);
    static QColor boardBackground();
    static void setBoardBackground(QColor value);
    static int tolerance();
    static void setTolerance(int value);
    static bool enableScaling();
    static void setEnableScaling(bool value);
    static QString lastImage();
    static void setLastImage(const QString &value);

protected:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private slots:
    void saveSettings();
    void on_boardColorButton_clicked();
};

#endif // SETTINGSDIALOG_H
