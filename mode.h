#ifndef MODE_H
#define MODE_H

#include <QDialog>
#include"manageusers.h"
#include"bookin.h"
#include"managebooks.h"
#include<QDebug>
namespace Ui {
class Mode;
}

class Mode : public QDialog
{
    Q_OBJECT

public:
    explicit Mode(QWidget *parent = nullptr);
    ~Mode();

private slots:
    void on_BackButton_clicked();//退出
    void on_ManageUsersButton_clicked();//管理用户
    void on_BookInButton_clicked();//书籍入库
    void on_managebooksButton_clicked();//管理书籍

    void on_returnFromManageUsers();  // 槽函数处理从"管理用户"返回
    void on_returnFromBookIn();//从“书籍入库”返回
    void on_returnFromManagebooks();//从“管理书籍”返回

private:
    Ui::Mode *ui;
    ManageUsers*manageUsersWindow;
    BookIn*bookinWindow;
    managebooks*managebooksWindow;

};

#endif // MODE_H
