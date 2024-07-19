#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include<QTcpSocket>
#include<QtSql>
#include "emailyanshi.h"
#include<QTimer>
#include<QRegExp>
namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_BackButton_clicked();//取消（返回）
    //void on_sendCodeButton_clicked();//发送QQ邮箱验证码

    void initDatabase();//声明操作数据库的函数
    void saveToDatabase();//保存数据函数

    void on_confirmRegister_clicked();//确认注册
    void checkUserID();//在学号输入框检查是否已经是图书馆账号
     void validateEmail();//qq邮箱判断
    //void checkEmail();//在邮箱输入框检查邮箱是否已经被用过
    //自动登录
    //void onRegistrationOrPasswordResetSuccess(const QString &inuserid, const QString &newpwd);
     void checkInput();

//让emailshiyan发送验证码
    void on_sendCodeButton_clicked();
    //接收验证码
    void receiveVerificationCode(const QString &scode);

    void updateButtonText(); // 更新按钮文本

private:
    Ui::Register *ui;
    QSqlDatabase db;
    emailyanshi *emailWidget;  // emailyanshi 对象
        QString verificationCode;  // 接收的验证码

    QTcpSocket *m_pSocket;
    QTimer *m_pTimer; // 添加QTimer成员变量
        int m_nCountdown; // 倒计时变量
         bool verificationCodeSent; // 这里声明验证码发送状态的标志

    void userregister();//注册功能实现
    bool validatePassword(const QString &password);//密码格式控制（6~20位数字、字母、特殊字符组合）
//    bool validateEmail(const QString &email);//qq邮箱判断
//    bool verifyCode(const QString &code);//验证码判断
//    void on_emailTextChanged(const QString &text);
//    void on_sendCodeButtonContinue();

};

#endif // REGISTER_H
