#ifndef IMAGECHOOSER_H
#define IMAGECHOOSER_H

#include <QtGui>

namespace Ui {
    class ImageChooser;
}

typedef struct
{
    QIcon icon;
    QString path;
    QString caption;
} ImageItem;

QDataStream &operator<<(const QDataStream &out, const ImageItem &item);
QDataStream &operator>>(const QDataStream &in, ImageItem &item);
bool operator==(const ImageItem &item1, const ImageItem &item2);

class ImageChooser : public QDialog
{
    Q_OBJECT
    QMap<QListWidgetItem*, QString> map;
    Ui::ImageChooser *ui;
    QList<ImageItem> items;

public:
    explicit ImageChooser(QWidget *parent = 0);
    ~ImageChooser();

    QString getPictureFile();
    void addItem(const QString &path, const QString &caption, bool select = true, bool save = true);
    void addItem(const ImageItem &item, bool select = true, bool save = true);

public slots:
    void saveItems();
    void recoverItems();

private slots:
    void on_btnOther_clicked();
    void on_btnOk_clicked();
};

#endif // IMAGECHOOSER_H
