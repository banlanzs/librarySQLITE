#ifndef EMAILYANSHI_H
#define EMAILYANSHI_H

#include <QDialog>

#include<QTcpSocket>
namespace Ui {
class emailyanshi;
}

class emailyanshi : public QDialog
{
    Q_OBJECT

public:
    explicit emailyanshi(QWidget *parent = nullptr);
    ~emailyanshi();

    void sendEmail();

    //用来等待服务器响应
        void  waitAndReadData();

        //功能函数
        bool sendemil_fun();

        //生成验证码
        QString generateVerificationCode();

        // 发送成功后读秒60
        void startCountdown();

        //接收地址
        QByteArray rcvemail;

        Ui::emailyanshi* getUI(); // 添加函数返回UI指针

signals:
//    void emailSent(bool success);
//    void verificationCodeUpdated(const QString &code);
        void verificationCodeSent(const QString&code);


private slots:
    void on_backbutton_clicked();

private:
    Ui::emailyanshi *ui;
    QTcpSocket *m_pSocket;
        QString m_receiverData;
        QTimer *m_pTimer; // 添加QTimer成员变量
        int m_nCountdown; // 倒计时变量
        bool validateEmail(const QString&email);

        //登陆smtp服务器时所需的id和password，需要是base64编码格式
        QByteArray name;
        QByteArray passwd;

        //发送的标题和内容
        QByteArray s_Title;
        QByteArray s_Content;

        //发送邮件的邮件地址，发送地址就是登陆的
        QByteArray sendemail;

        //验证码
        QString verificationCode;

        void updateButtonText();//按钮失效-有效



};

#endif // EMAILYANSHI_H

