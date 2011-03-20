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

bool operator==(const ImageItem &item1, const ImageItem &item2)
{
    return item1.path == item2.path;
}

ImageChooser::ImageChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageChooser)
{
    ui->setupUi(this);
    recoverItems();

#if defined(Q_WS_MAEMO_5)
    setFixedHeight(420);
    setFixedWidth(parentWidget()->width());

    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnOk->setDefault(true);
    buttonBox->addButton(ui->btnOther, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->btnRemove, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->btnClear, QDialogButtonBox::ActionRole);
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

const QIcon &ImageChooser::getIconForCurrentPicture()
{
    foreach (ImageItem &item, items)
    {
        if (item.path == getPictureFile())
            return item.icon;
    }
    return QIcon();
}

void ImageChooser::on_btnOther_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);

    if (fileName.isEmpty())
        return;

    QString caption = QInputDialog::getText(this, "Name the image", "Please give a name to this image.");

    if (caption.isEmpty() || caption.length() == 0)
        return;

    addItem(fileName, caption, true);
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
        QMessageBox::warning(this, "Error", "This file is not a valid image file or not supported by your Qt installation.");
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
    settings.setValue(SETTING_IMAGE_LIST, array);
}

void ImageChooser::recoverItems()
{
    if (settings.contains(SETTING_IMAGE_LIST))
    {
        // deserializing from QSettings
        QByteArray array = settings.value(SETTING_IMAGE_LIST).toByteArray();
        QDataStream stream(&array, QIODevice::ReadOnly);
        stream >> items;

        foreach (ImageItem item, items)
        {
            if (QFile::exists(item.path))
            {
                // only showing the items but not saving them again
                addItem(item, false, false);
            }
            else
            {
                items.removeAll(item);
            }
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

const QList<ImageItem> &ImageChooser::imageItems()
{
    return items;
}

void ImageChooser::on_btnOk_clicked()
{
    if (ui->listWidget->selectedItems().count() > 0)
    {
        if (QFile::exists(map[ui->listWidget->selectedItems().at(0)]))
            accept();
        else
            QMessageBox::warning(this, "Error", "This file doesn't exist. Please select a file that exists.");
    }
    else
    {
        QMessageBox::warning(this, "Error", "Please select an image.");
    }
}

void ImageChooser::on_btnClear_clicked()
{
    if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Clear all custom images?", QMessageBox::Yes, QMessageBox::Cancel))
    {
        ui->listWidget->clear();
        items.clear();
        map.clear();
        settings.remove(SETTING_IMAGE_LIST);

        recoverItems();
    }
}

void ImageChooser::on_btnRemove_clicked()
{
    if (ui->listWidget->selectedItems().count() > 0)
    {
        if (QMessageBox::Yes == QMessageBox::question(this, "Are you sure?", "Remove selected image?", QMessageBox::Yes, QMessageBox::Cancel))
        {
            QListWidgetItem *lwi = ui->listWidget->selectedItems().at(0);
            ui->listWidget->removeItemWidget(lwi);
            map.remove(lwi);

            foreach (ImageItem item, items)
            {
                if (item.caption == lwi->text())
                {
                    qDebug() << lwi->text();
                    items.removeAll(item);
                }
            }

            delete lwi;
            saveItems();
        }
    }
    else
    {
        QMessageBox::information(this, "Error", "Select an image if you want to remove it.");
    }
}
