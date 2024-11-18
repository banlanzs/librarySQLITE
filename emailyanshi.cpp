//zhoushuang
#include "emailyanshi.h"
#include "ui_emailyanshi.h"
#include"mainwindow.h"
#include<QString>
#include <QtNetwork>
#include<QtDebug>
#include<QMessageBox>
#include<cstdlib>//for rand() and srand()
#include<QTimer>//for time()
#include<QSqlError>
emailyanshi::emailyanshi(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::emailyanshi)
{
    ui->setupUi(this);
    m_pSocket=new QTcpSocket();
    m_pTimer=new QTimer(this);//初始化QTimer
    // 设置默认值
        ui->SendEmail_lineEdit->setText("填入自己的转发邮箱");
        ui->logid_lineEdit->setText("邮箱号");
        ui->logpasswd_lineEidt->setText("");//授权码
        ui->logid_lineEdit->setText("邮箱号");
        ui->subject_lineEdit->setText("来自图书管理系统的信息");
        ui->textEdit_Text->setText("您好！您的验证码为");
        // Generate initial verification code
            verificationCode = generateVerificationCode();
            ui->textEdit_Text->setText(QString("【图书馆】验证码 %1 用于图书馆验证，60秒内有效，请勿泄露和转发。如非本人操作，请忽略此邮件。").arg(verificationCode));
    //点击发送，获取所需参数，进行发送
        connect(ui->sendButton,&QPushButton::clicked,this,[=]{
            this->m_pSocket = new QTcpSocket;
            this->name = ui->logid_lineEdit->text().toUtf8().toBase64();//发送邮箱数字
            this->passwd = ui->logpasswd_lineEidt->text().toUtf8().toBase64();//授权码
            this->sendemail = ui->SendEmail_lineEdit->text().toUtf8();//发送邮箱
            this->rcvemail=ui->Rcvemail_lineEdit->text().toUtf8();//接收邮箱
            if(!validateEmail(rcvemail)){
                QMessageBox::warning(this,"error","邮箱格式错误！");
                ui->sendButton->setEnabled(false);
                ui->SendEmail_lineEdit->clear();
                ui->SendEmail_lineEdit->setFocus();
                return;
            }else ui->sendButton->setEnabled(true);
            m_receiverData = ui->subject_lineEdit->text().toUtf8();//邮件主题
            s_Content = ui->textEdit_Text->toPlainText().toUtf8();//邮件内容
                //sendemil_fun();
            if (sendemil_fun()) {
                emit verificationCodeSent(verificationCode);
                        QMessageBox::information(this, "sucess", "邮件发送成功");
                        startCountdown();//读秒60
                    } else {
                        QMessageBox::warning(this, "error", "邮件1发送失败");
                    }

        });

        // Connect the timer for countdown
            connect(m_pTimer, &QTimer::timeout, this, [=]{
                m_nCountdown--;
                updateButtonText();
                if (m_nCountdown <= 0) {
                    m_pTimer->stop();
                    verificationCode = generateVerificationCode();
                    ui->textEdit_Text->setText(QString("【图书馆】验证码 %1 用于图书馆验证，60秒内有效，请勿泄露和转发。如非本人操作，请忽略此邮件。").arg(verificationCode));
                    ui->sendButton->setEnabled(true);
                    ui->sendButton->setText("发送");
                }
            });
}

////qq邮箱判断
bool emailyanshi::validateEmail(const QString &email){
    QRegExp emailPattern("^[a-zA-Z0-9._%+-]+@qq\\.com$");
        return emailPattern.exactMatch(email);
}
emailyanshi::~emailyanshi()
{
    delete ui;
    delete  m_pSocket;
}

void emailyanshi::waitAndReadData()
{
        m_pSocket->waitForReadyRead(1000);
        m_receiverData = m_pSocket->readAll();

        //将其显示在服务器反馈TextEdit中
        qDebug()<<m_receiverData<<endl;
        ui->server_textEdit->append(m_receiverData);//服务器反馈
}

bool emailyanshi::sendemil_fun()
{

        m_pSocket->connectToHost("smtp.qq.com",25,QTcpSocket::ReadWrite);  //连接qq邮箱

//        m_pSocket->waitForConnected(1000);
//        waitAndReadData();
        if (!m_pSocket->waitForConnected(1000)) return false;
            waitAndReadData();

        m_pSocket->write("helo yuanzhaoyi\r\n");
        waitAndReadData();

        m_pSocket->write("auth login\r\n");
        waitAndReadData();

        m_pSocket->write(name+"\r\n");  //写入用户名
        waitAndReadData();

        m_pSocket->write(passwd+"\r\n");  //写入密码
        waitAndReadData();

        m_pSocket->write("mail from: <"+sendemail+">\r\n"); //发送的邮箱
        waitAndReadData();

        m_pSocket->write("rcpt to: <"+rcvemail+">\r\n"); //接收的邮箱
        waitAndReadData();

        m_pSocket->write("data\r\n");  //开始写入
        waitAndReadData();

        m_pSocket->write("from:图书管理系统 <"+sendemail+">\r\n");  //发送名称
        waitAndReadData();

        m_pSocket->write("to:<"+rcvemail+">");  //接收名称
        waitAndReadData();

        m_pSocket->write("data\r\n");
        waitAndReadData();

        m_pSocket->write("Subject:来自图书管理系统的信息："+s_Title+"\r\n");  //标题
        m_pSocket->write("\r\n");

        m_pSocket->write(s_Content.append("\r\n")); //内容
        m_pSocket->write(".\r\n");
        waitAndReadData();

        m_pSocket->write("quit\r\n");
        m_pSocket->disconnect();

        // 检查成功响应（假设 "250" 是成功响应的一部分）
           return m_receiverData.contains("250");


}

QString emailyanshi::generateVerificationCode()
{
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    int code = qrand() % 900000 + 100000; // 随机生成6位数字
    return QString::number(code);
}

void emailyanshi::startCountdown()
{
    m_nCountdown = 60;
    ui->sendButton->setEnabled(false);
    updateButtonText();
    m_pTimer->start(1000); // Start the timer to update every second
}

void emailyanshi::updateButtonText()
{
    if (m_nCountdown > 0) {
        ui->sendButton->setText(QString("重新发送 (%1)s").arg(m_nCountdown));
    } else {
        ui->sendButton->setText("发送");
    }
}

void emailyanshi::on_backbutton_clicked()
{
    this->close();
    MainWindow*m=new MainWindow();
    m->show();
}

Ui::emailyanshi* emailyanshi::getUI()
{
    return ui;
}
