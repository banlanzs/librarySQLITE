#ifndef REWRITEPWD_H
#define REWRITEPWD_H
#include"emailyanshi.h"
#include"mainwindow.h"
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include<QLineEdit>
#include<QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include<QTimer>
namespace Ui {
class rewritepwd;
}

class rewritepwd : public QDialog
{
    Q_OBJECT

public:
    explicit rewritepwd(QWidget *parent = nullptr);
    ~rewritepwd();

private slots:
    void on_back2Button_clicked();//取消（返回）

    void on_rewritepwd_2_clicked();//密码重置功能实现
    //让emailshiyan发送验证码
        void on_sendCodeButton_clicked();
        //接收验证码
        void receiveVerificationCode(const QString &scode);

        void updateButtonText(); // 更新按钮文本
        //自动登录
       // void onRegistrationOrPasswordResetSuccess(const QString &inuserid, const QString &newpwd);
        void checkInput();

private:
    Ui::rewritepwd *ui;
    emailyanshi*emailWidget;
    QSqlDatabase db;
    QString verificationCode;//6位验证码
    QString inuserid;//学号
    bool sendVerificationCode(const QString &email);//发送验证码到QQ邮箱
    QTimer *timer;  // 添加QTimer指针
        int countdown;  // 倒计时计数器
};

#endif // REWRITEPWD_H
