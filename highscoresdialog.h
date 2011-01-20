#ifndef HIGHSCORESDIALOG_H
#define HIGHSCORESDIALOG_H

#include <QDialog>

namespace Ui {
    class HighScoresDialog;
}

class HighScoresDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HighScoresDialog(QWidget *parent = 0);
    ~HighScoresDialog();

    static QList<QString> getMatchTypes();
    static QList<QString> getResults(const QString &matchType);
    void addMatchType(const QString &matchType);
    void addResult(const QString &matchType, const QString &result);

protected:
    void showEvent(QShowEvent *ev);

private:
    Ui::HighScoresDialog *ui;

public slots:
    void clearScores();
    void matchTypeIndexChanged(int i);
};

#endif // HIGHSCORESDIALOG_H
