#include "newgamedialog.h"
#include "ui_newgamedialog.h"

#include "settingsdialog.h"
#include "util.h"

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewGameDialog),
    chooser(new ImageChooser(this))
{
    ui->setupUi(this);

#if defined(HAVE_QACCELEROMETER)
#else
    // Hiding the relevant UI if we don't have an accelerometer
    ui->accelerometerBox->hide();
#endif

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(this, SIGNAL(rejected()), this, SLOT(saveSettings()));
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
}

void NewGameDialog::showEvent(QShowEvent *event)
{
    chooser->setCurrentPictureFile(SettingsDialog::lastImage());
    ui->selectImageButton->setIcon(chooser->getIconForCurrentPicture());

    ui->accelerometerBox->setChecked(SettingsDialog::useAccelerometer());
    ui->useOpenGlBox->setChecked(SettingsDialog::useOpenGl());

    int r = CLAMP(SettingsDialog::rows(), 2, 15), c = CLAMP(SettingsDialog::columns(), 2, 15);
    ui->cbRows->setCurrentIndex(r - 2);
    ui->cbColumns->setCurrentIndex(c - 2);

    QDialog::showEvent(event);
}

void NewGameDialog::saveSettings()
{
    SettingsDialog::setColumns(ui->cbColumns->currentIndex() + 2);
    SettingsDialog::setRows(ui->cbRows->currentIndex() + 2);
    SettingsDialog::setUseAccelerometer(ui->accelerometerBox->isChecked());
    SettingsDialog::setUseOpenGl(ui->useOpenGlBox->isChecked());
    SettingsDialog::setLastImage(chooser->getPictureFile());
}

QString NewGameDialog::getPictureFile()
{
    return chooser->getPictureFile();
}

void NewGameDialog::on_selectImageButton_clicked()
{
    chooser->exec();
    ui->selectImageButton->setIcon(chooser->getIconForCurrentPicture());
}