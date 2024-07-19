#include "usermode.h"
#include "ui_usermode.h"
#include"mainwindow.h"
#include"borrowbook.h"
#include"returnbook.h"
#include"reservation.h"
#include"managebooks.h"
#include<QDebug>
UserMode::UserMode(QString xuehao,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserMode),
    m_xuehao(xuehao),
    borrowbookWindow(new borrowbook(m_xuehao)),
    returnbookWindow(new returnbook(m_xuehao)),
    reservationWindow(new reservation),
    managebooksWindow(new managebooks),
    backButtonClicked(false) // 初始化标志位
{
    ui->setupUi(this);
    //ui->backButton->setEnabled(false);
    // Connect the return signal from ManageReaders to the slot in Mode
        connect(borrowbookWindow, &borrowbook::returntomode, this, &UserMode::on_returnFromborrowbook);
        connect(returnbookWindow,&returnbook::returntomode,this,&UserMode::on_returnFromreturnbook);
        connect(reservationWindow,&reservation::returntomode,this,&UserMode::on_returnFromreservation);
        connect(borrowbookWindow,&borrowbook::bookBorrowed,managebooksWindow,&managebooks::on_refreshButton_clicked);
        qDebug()<<"usermode被创建"<<endl;
}



//退出
void UserMode::on_backButton_clicked()
{
//    MainWindow*m=new MainWindow();
//    this->close();
//    m->show();

    backButtonClicked = true; // 设置标志位
    MainWindow *mainWindow = qobject_cast<MainWindow *>(parentWidget());
        if (mainWindow) {
            mainWindow->show();
            this->close();
        }else{
            MainWindow*m=new MainWindow();
            m->show();
            this->close();
        }

}

//进入“借书”页面
void UserMode::on_borrowButton_clicked()
{
    this->hide();
    borrowbookWindow->show();
}
//从“借书”返回
void UserMode::on_returnFromborrowbook(){
    borrowbookWindow->hide();
    this->show();
}

//进入“预约”页面
void UserMode::on_reservationButton_clicked()
{
    this->hide();
    reservationWindow->show();
}
//从“预约”返回
void UserMode::on_returnFromreservation(){
    reservationWindow->hide();
    this->show();
}

//进入“还书”
void UserMode::on_returnBookButton_clicked()
{
    this->hide();
    returnbookWindow->show();
}
//从“还书”返回
void UserMode::on_returnFromreturnbook(){
    returnbookWindow->hide();
    this->show();
}


UserMode::~UserMode()
{
//    delete ui;
//    qDebug()<<"执行了ui delete"<<endl;
//    delete borrowbookWindow;
//    qDebug()<<"执行了b delete"<<endl;
//    delete returnbookWindow;
//    qDebug()<<"执行了r delete"<<endl;
//    delete reservationWindow;
//    qDebug()<<"执行了re delete"<<endl;
//    delete managebooksWindow;
//    qDebug()<<"执行了m delete"<<endl;
    if(backButtonClicked){
    delete ui;
        if (borrowbookWindow) {
            delete borrowbookWindow;
            borrowbookWindow = nullptr;
            qDebug() << "Deleted borrowbookWindow";
        }
        if (returnbookWindow) {
            delete returnbookWindow;
            returnbookWindow = nullptr;
            qDebug() << "Deleted returnbookWindow";
        }
        if (reservationWindow) {
            delete reservationWindow;
            reservationWindow = nullptr;
            qDebug() << "Deleted reservationWindow";
        }
        if (managebooksWindow) {
            delete managebooksWindow;
            managebooksWindow = nullptr;
            qDebug() << "Deleted managebooksWindow";
        }
        qDebug() << "UserMode destructor called";
    }
}
