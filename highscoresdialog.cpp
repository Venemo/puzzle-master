#include <QtGui>

#include "highscoresdialog.h"
#include "ui_highscoresdialog.h"

#define SETTING_MATCHTYPES "MatchTypes"
#define SETTING_RESULTS(type) QString("Results-") + QString(type)

HighScoresDialog::HighScoresDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::HighScoresDialog)
{
    ui->setupUi(this);

#if defined(Q_WS_MAEMO_5)
    setFixedWidth(parentWidget()->width());

    // This ensures the correct look on Maemo 5 too
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    ui->btnClose->setDefault(true);
    buttonBox->addButton(ui->btnClear, QDialogButtonBox::ActionRole);
    buttonBox->addButton(ui->btnClose, QDialogButtonBox::ActionRole);
    layout()->addWidget(buttonBox);
#endif
}

HighScoresDialog::~HighScoresDialog()
{
    delete ui;
}

void HighScoresDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    QStringList matchTypes = getMatchTypes();

    ui->cbScoreType->clear();
    ui->listWidget->clear();

    if (matchTypes.count() > 0)
    {
        ui->cbScoreType->addItems(matchTypes);
        ui->cbScoreType->setCurrentIndex(0);
    }
    else
    {
        QMessageBox::information(this, tr("No high scores"), tr("There are no high scores yet."));
    }
}

void HighScoresDialog::matchTypeIndexChanged(int i)
{
    if (i >= 0)
    {
        QStringList matchTypes = getMatchTypes();
        ui->listWidget->clear();
        ui->listWidget->addItems(getResults(matchTypes[i]));
    }
}

void HighScoresDialog::clearScores()
{
    QSettings s;
    QList<QString> matchTypes = getMatchTypes();

    foreach (QString matchType, matchTypes)
    {
        s.remove(SETTING_RESULTS(matchType));
    }
    s.remove(SETTING_MATCHTYPES);

    ui->cbScoreType->clear();
    ui->listWidget->clear();

    QMessageBox::information(this, tr("Clear successful!"), tr("High scores successfully deleted."));
    reject();
}

void HighScoresDialog::addMatchType(const QString &matchType)
{
    QSettings s;
    QList<QString> matchTypes = getMatchTypes();
    matchTypes.append(matchType);
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << matchTypes;
    s.setValue(SETTING_MATCHTYPES, arr);

    ui->cbScoreType->addItem(matchType);
}

void HighScoresDialog::addResult(const QString &matchType, const QString &result)
{
    QSettings s;
    QList<QString> matchTypes = getMatchTypes();
    if (!matchTypes.contains(matchType))
        addMatchType(matchType);
    QList<QString> results = getResults(matchType);
    results.append(result);
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << results;
    s.setValue(SETTING_RESULTS(matchType), arr);
}

QList<QString> HighScoresDialog::getMatchTypes()
{
    QSettings s;
    QList<QString> val;
    QByteArray arr = s.value(SETTING_MATCHTYPES).toByteArray();
    QDataStream stream(&arr, QIODevice::ReadOnly);
    stream >> val;
    return val;
}

QList<QString> HighScoresDialog::getResults(const QString &matchType)
{
    QSettings s;
    QList<QString> val;
    QByteArray arr = s.value(SETTING_RESULTS(matchType)).toByteArray();
    QDataStream stream(&arr, QIODevice::ReadOnly);
    stream >> val;
    return val;
}
