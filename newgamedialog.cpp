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

#if defined(Q_WS_MAEMO_5)
    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->startButton->setDefault(true);
    buttonBox->addButton(ui->cancelButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->startButton, QDialogButtonBox::ActionRole);
    ui->verticalLayout->addWidget(buttonBox);
#endif

#if defined(HAVE_QACCELEROMETER)
#else
    // Hiding the relevant UI if we don't have an accelerometer
    ui->accelerometerBox->hide();
#endif

#if defined(Q_OS_SYMBIAN)
    QAction *closeAction = new QAction(tr("Cancel"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(reject()));
    addAction(closeAction);

    QAction *okAction = new QAction(tr("Start"), this);
    okAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(okAction, SIGNAL(triggered()), this, SLOT(accept()));
    addAction(okAction);

    ui->cancelButton->hide();
    ui->startButton->hide();
    ui->startInFullscreenBox->hide();
#endif

    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
    delete chooser;
}

void NewGameDialog::showEvent(QShowEvent *event)
{
    chooser->setCurrentPictureFile(SettingsDialog::lastImage());
    ui->selectImageButton->setIcon(chooser->getIconForCurrentPicture());

    ui->accelerometerBox->setChecked(SettingsDialog::useAccelerometer());
    ui->startInFullscreenBox->setChecked(SettingsDialog::startInFullscreen());

    int r = CLAMP(SettingsDialog::rows(), 2, 15), c = CLAMP(SettingsDialog::columns(), 2, 15);
    ui->cbRows->setCurrentIndex(r - 2);
    ui->cbColumns->setCurrentIndex(c - 2);

    QDialog::showEvent(event);
}

void NewGameDialog::hideEvent(QHideEvent *event)
{
    saveSettings();
    QDialog::hideEvent(event);
}

void NewGameDialog::saveSettings()
{
    SettingsDialog::setColumns(ui->cbColumns->currentIndex() + 2);
    SettingsDialog::setRows(ui->cbRows->currentIndex() + 2);
    SettingsDialog::setUseAccelerometer(ui->accelerometerBox->isChecked());
    SettingsDialog::setStartInFullscreen(ui->startInFullscreenBox->isChecked());
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
