#ifndef IMAGECHOOSER_H
#define IMAGECHOOSER_H

#include <QtGui>

namespace Ui {
    class ImageChooser;
}

class ImageChooser : public QDialog
{
    Q_OBJECT
    QMap<QListWidgetItem*, QString> map;

public:
    explicit ImageChooser(QWidget *parent = 0);
    ~ImageChooser();

    QString getPictureFile();

private:
    Ui::ImageChooser *ui;

private slots:
    void on_btnOther_clicked();
};

#endif // IMAGECHOOSER_H
