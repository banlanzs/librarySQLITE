/*
#ifndef RETURNBOOK_H
#define RETURNBOOK_H

#include <QDialog>
#include"DatabaseManager.h"
#include<QSqlDatabase>
namespace Ui {
class returnbook;
}

class returnbook : public QDialog
{
    Q_OBJECT

public:
    explicit returnbook(QWidget *parent = nullptr);
    ~returnbook();
signals:
    void returntomode();//->closeButton
private slots:
    void on_returnButton_clicked();

    void on_backButton_2_clicked();

    void checkShuhaoFormat();//立即检查书号格式

private:
    Ui::returnbook *ui;
    QSqlDatabase db;
    void initDatabase();//声明操作数据库函数
    void saveToDatabase();//保存数据函数
    void setupConnections();
};

#endif // RETURNBOOK_H
*/


#ifndef RETURNBOOK_H
#define RETURNBOOK_H

#include <QDialog>
#include"DatabaseManager.h"
#include<QSqlDatabase>
namespace Ui {
class returnbook;
}

class returnbook : public QDialog
{
    Q_OBJECT

public:
    explicit returnbook(QString &xuehao,QWidget *parent = nullptr);
    ~returnbook();
signals:
    void returntomode();//->closeButton
private slots:
    void on_returnButton_clicked();

    void on_backButton_2_clicked();

    //void checkShuhaoFormat();//立即检查书号格式

    void on_searchButton_clicked();

    void on_refreshButton_clicked();

private:
    Ui::returnbook *ui;
    QSqlDatabase db;
    void initDatabase();//声明操作数据库函数
    QString m_xuehao;
    void saveToDatabase();//保存数据函数
    //void setupConnections();
    void loadBooks();
};

#endif // RETURNBOOK_H
