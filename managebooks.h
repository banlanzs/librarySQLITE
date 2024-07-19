#ifndef MANAGEBOOKS_H
#define MANAGEBOOKS_H
//zhoushuang
#include <QDialog>
#include<QSqlDatabase>
#include<QDebug>
namespace Ui {
class managebooks;
}

class managebooks : public QDialog
{
    Q_OBJECT

public:
    explicit managebooks(QWidget *parent = nullptr);
    ~managebooks();
public slots:
    void on_refreshButton_clicked();
private slots:
    void on_closeButton_clicked();

//    void on_refreshButton_clicked();
    void on_cleanButton_clicked();

signals:
    void returntomode();//->closeButton

private:
    Ui::managebooks *ui;
    QSqlDatabase db;
            void initDatabase();
            void loadBooks();
};

#endif // MANAGEBOOKS_H
