#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    ~SearchDialog();

signals:
    void search(QString text);

private slots:
    void on_pbSearch_clicked();

private:
    Ui::SearchDialog *ui;
};

#endif // SEARCHDIALOG_H
