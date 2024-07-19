#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//zhoushuang
#include <QMainWindow>
#include<QWidget>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlDatabase>
#include"usermode.h"
#include"borrowbook.h"
#include<QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //登录
    bool validateUser(const QString &userid, const QString &password);
    //填充
    //void fillCredentials(const QString &userid, const QString &password);
    //void on_LoginButton_clicked();//登录功能caohanshu
//protected:
//    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_RegisterButton_clicked();//转到注册
    void on_Forgetpwd_clicked();//密码重置信号

    //void on_pushButton_clicked();
    void on_LoginButton_clicked();//槽函数不能放到其他地方了，不然不会有任何反应
    void on_out_clicked();
void checkInput();

    //void on_pushButton_clicked();//转到邮箱测试


private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    void initDatabase();
    bool exitButtonClicked;
    UserMode *userWindow;


};
#endif // MAINWINDOW_H
