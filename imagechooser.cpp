#include <QtGui>
#include "imagechooser.h"
#include "ui_imagechooser.h"

#define SETTING_IMAGE_LIST "AdditionalImageList"

QDataStream &operator<<(QDataStream &out, const ImageItem &item)
{
    out << item.icon << item.path << item.caption;
    return out;
}

QDataStream &operator>>(QDataStream &in, ImageItem &item)
{
    in >> item.icon >> item.path >> item.caption;
    return in;
}

ImageChooser::ImageChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageChooser)
{
    ui->setupUi(this);
    recoverItems();

#if defined(Q_WS_MAEMO_5)
    setFixedHeight(250);
    setFixedWidth(parentWidget()->width());

    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnOk->setDefault(true);
    buttonBox->addButton(ui->btnOther, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->btnCancel, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->btnOk, QDialogButtonBox::ActionRole);
    layout()->addWidget(buttonBox);
#endif
}

ImageChooser::~ImageChooser()
{
    saveItems();
    delete ui;
}

QString ImageChooser::getPictureFile()
{
    if (ui->listWidget->selectedItems().count() > 0)
        return map[ui->listWidget->selectedItems().at(0)];
    return ":/image1.jpg";
}

void ImageChooser::on_btnOther_clicked()
{
    QString str = QFileDialog::getOpenFileName(this);

    if (str.isEmpty())
        return;

    addItem(str, str, true);
}

void ImageChooser::addItem(const QString &path, const QString &caption, bool select, bool save)
{
    QPixmap pix(path);

    if (!pix.isNull())
    {
        ImageItem item;
        item.icon = QIcon(pix.scaled(64, 64, Qt::KeepAspectRatio));
        item.path = path;
        item.caption = caption;

        addItem(item, select, save);
    }
    else
    {
        QMessageBox::warning(this,
                             "Error",
                             "This file is not a valid image file or not supported by your Qt installation.",
                     #if defined(Q_WS_MAEMO_5)
                             QMessageBox::Cancel
                     #else
                             QMessageBox::Ok
                     #endif
                             );
    }
}

void ImageChooser::addItem(const ImageItem &item, bool select, bool save)
{
    if (save)
        items.append(item);

    QListWidgetItem *widgetItem = new QListWidgetItem(item.icon, item.caption, ui->listWidget);
    map[widgetItem] = item.path;

    if (select)
        ui->listWidget->setCurrentItem(widgetItem);
}

void ImageChooser::saveItems()
{
    // serializing to QSettings
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream << items;
    QSettings s;
    s.setValue(SETTING_IMAGE_LIST, array);
}

void ImageChooser::recoverItems()
{
    QSettings s;
    if (s.contains(SETTING_IMAGE_LIST))
    {
        // deserializing from QSettings
        QByteArray array = s.value(SETTING_IMAGE_LIST).toByteArray();
        QDataStream stream(&array, QIODevice::ReadOnly);
        stream >> items;

        foreach (ImageItem item, items)
        {
            // only showing the items but not saving them again
            addItem(item, false, false);
        }
    }
    else
    {
        // adding the default images
        addItem(":/image1.jpg", "Bamboo Zen by MissNysha", false);
        addItem(":/image2.jpg", "Blue Dock by dimage", false);
        addItem(":/image3.jpg", "Solar by the Fedora project", false);
        addItem(":/image4.jpg", "The beach huts by Greg Roberts", false);
        addItem(":/image5.jpg", QString::fromUtf8("Squirrel by Gábor Bányász"), false);
    }
}
