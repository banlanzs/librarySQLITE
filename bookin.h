#ifndef BOOKIN_H
#define BOOKIN_H

#include <QDialog>
#include"DatabaseManager.h"
#include<QSqlDatabase>
#include<QDebug>
namespace Ui {
class BookIn;
}

class BookIn : public QDialog
{
    Q_OBJECT

public:
    explicit BookIn(QWidget *parent = nullptr);
    ~BookIn();
signals:
    void returntomode();//->closeButton
private slots:
    void on_BackButton_clicked();//返回
    void on_OKpushButton_clicked();//确认入库
    //void checkShuhaoFormat();//立即检查书号格式
    void checkBookExists();

private:
    Ui::BookIn *ui;
    QSqlDatabase db;
    void initDatabase();//声明操作数据库函数
    void saveToDatabase();//保存数据函数
    void setupConnections();
};

#endif // BOOKIN_H
