#include "mode.h"
#include "ui_mode.h"
#include"mainwindow.h"
#include"manageusers.h"
#include"bookin.h"
#include<QTimer>
Mode::Mode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Mode),
    manageUsersWindow(new ManageUsers),
    bookinWindow(new BookIn),
    managebooksWindow(new managebooks)
{
    ui->setupUi(this);
    //使用信号和槽机制来切换窗口。以下是一个示例，演示如何使用信号和槽机制返回到原来的 Mode 界面，而不是创建新的实例
    //每次调用 new Mode() 都会创建一个新的 Mode 对象，如果不适当地管理这些对象，可能会导致内存泄漏。
    //频繁地创建和销毁对象可能会降低程序的性能。
    //如果 Mode 界面有一些状态需要保留，例如登录信息、当前选中的项目等，每次创建新的 Mode 对象都需要重新初始化这些状态。
    //更好的方法是隐藏和显示现有的窗口实例，而不是频繁地创建新的实例
    // Connect the return signal from ManageReaders to the slot in Mode
        connect(manageUsersWindow, &ManageUsers::returntomode, this, &Mode::on_returnFromManageUsers);
        connect(bookinWindow,&BookIn::returntomode,this,&Mode::on_returnFromBookIn);
        connect(managebooksWindow,&managebooks::returntomode,this,&Mode::on_returnFromManagebooks);
}

Mode::~Mode()
{
    delete ui;
    delete manageUsersWindow;
    delete bookinWindow;
    delete managebooksWindow;
}

//退出
void Mode::on_BackButton_clicked()
{
    MainWindow*m=new MainWindow();
    m->show();
    this->close();
}

//进入"管理用户"
void Mode::on_ManageUsersButton_clicked()
{
//    ManageUsers*m=new ManageUsers();
//    m->show();
//    this->close();
    this->hide();  // Hide the current window
        manageUsersWindow->show();  // Show the ManageUsers window
}
//从“管理用户”返回
void Mode::on_returnFromManageUsers()
{
    manageUsersWindow->hide();  // 隐藏 ManageUsers 窗口
    this->show();  // 显示 Mode 窗口
}

//进入“书籍入库”
void Mode::on_BookInButton_clicked()
{
    this->hide();
    bookinWindow->show();
}
//从“书籍入库”返回
void Mode::on_returnFromBookIn(){
    bookinWindow->hide();
    this->show();
}

//进入“管理书籍”
void Mode::on_managebooksButton_clicked()
{
    this->hide();
    managebooksWindow->show();
}
//从“管理书籍”返回
void Mode::on_returnFromManagebooks(){
    managebooksWindow->hide();
    this->show();
}
