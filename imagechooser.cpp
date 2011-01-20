#include <QtGui>
#include "imagechooser.h"
#include "ui_imagechooser.h"

ImageChooser::ImageChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageChooser)
{
    ui->setupUi(this);

    QPixmap p1(":/image1.jpg");
    QPixmap p2(":/image2.jpg");
    QPixmap p3(":/image3.jpg");
    QPixmap p4(":/image4.jpg");
    QPixmap p5(":/image5.jpg");
    QListWidgetItem *item1 = new QListWidgetItem(QIcon(p1.scaled(64, 64, Qt::KeepAspectRatio)), "Bamboo Zen by MissNysha", ui->listWidget);
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(p2.scaled(64, 64, Qt::KeepAspectRatio)), "Blue Dock by dimage", ui->listWidget);
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(p3.scaled(64, 64, Qt::KeepAspectRatio)), "Solar by the Fedora project", ui->listWidget);
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(p4.scaled(64, 64, Qt::KeepAspectRatio)), "The beach huts by Greg Roberts", ui->listWidget);
    QListWidgetItem *item5 = new QListWidgetItem(QIcon(p5.scaled(64, 64, Qt::KeepAspectRatio)), QString::fromUtf8("Squirrel by Gábor Bányász"), ui->listWidget);
    ui->listWidget->setCurrentItem(item1);

    map[item1] = ":/image1.jpg";
    map[item2] = ":/image2.jpg";
    map[item3] = ":/image3.jpg";
    map[item4] = ":/image4.jpg";
    map[item5] = ":/image5.jpg";

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

    QPixmap p(str);
    QListWidgetItem *item = new QListWidgetItem(QIcon(p.scaled(64, 64, Qt::KeepAspectRatio)), str, ui->listWidget);
    map[item] = str;
}
