#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QtGui>
#include "imagechooser.h"

namespace Ui {
    class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT
    Ui::NewGameDialog *ui;
    ImageChooser *chooser;

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();
    QString getPictureFile();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void on_selectImageButton_clicked();
    void saveSettings();

};

#endif // NEWGAMEDIALOG_H
