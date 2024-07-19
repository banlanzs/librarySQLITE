#ifndef RESERVATION_H
#define RESERVATION_H

#include <QDialog>
#include"DatabaseManager.h"
#include<QSqlDatabase>
#include <QDialog>
#include<QTcpSocket>
#include<QtSql>
#include "emailyanshi.h"
#include<QTimer>
namespace Ui {
class reservation;
}

class reservation : public QDialog
{
    Q_OBJECT

public:
    explicit reservation(QWidget *parent = nullptr);
    ~reservation();

private slots:
    void on_backButton_2_clicked();

    void on_reservationButton_clicked();

    void receive(const QString &scode);


    //void checkShuhaoFormat();//立即检查书号格式
    void on_send_clicked();

    void updateButtonText(); // 更新按钮文本
    bool booknumber();

signals:
    void returntomode();//->closeButton
private:
    Ui::reservation *ui;
    void validateEmail();//qq邮箱判断
    QSqlDatabase db;
    emailyanshi *emailWidget;  // emailyanshi 对象
    QString verificationCode;  // 接收的信息
    QTcpSocket *m_pSocket;
    QTimer *m_pTimer; // 添加QTimer成员变量
        int m_nCountdown; // 倒计时变量
         bool verificationCodeSent; // 这里声明信息发送状态的标志

    void initDatabase();//声明操作数据库函数
    void saveToDatabase();//保存数据函数
    //void setupConnections();
};

#endif // RESERVATION_H
