#ifndef BORROWBOOK_H
#define BORROWBOOK_H

#include <QDialog>
#include<QSqlDatabase>
namespace Ui {
class borrowbook;
}

class borrowbook : public QDialog
{
    Q_OBJECT

public:
    explicit borrowbook(QString &xuehao,QWidget *parent = nullptr);
    ~borrowbook();
signals:
    void returntomode();//->closeButton
    void bookBorrowed();
private slots:
    void on_backButton_2_clicked();

    void on_refreshButton_clicked();

    void on_searchButton_clicked();

    void on_borrowButton_clicked();
        void checkInput();

private:
    Ui::borrowbook *ui;
    QString m_xuehao;
    QSqlDatabase db;
    void initDatabase();
    void loadBooks();

};

#endif // BORROWBOOK_H
