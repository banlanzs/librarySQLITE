#include "reservation.h"
#include "ui_reservation.h"
#include<QMessageBox>
#include<QRegularExpression>
#include<QSqlQuery>
#include<QSqlError>
#include <QDebug>
#include<QKeySequenceEdit>
#include "emailyanshi.h"//发邮件
#include "ui_emailyanshi.h"//没有这个，getUI()->sendButton会出错
#include "DatabaseManager.h"
#include<QDebug>//输出流
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include<QNetworkReply>
#include<QSqlDatabase>//连接数据库
#include<QRegExp>
#include <QDateTime>
#include<QTcpSocket>
#include <QtNetwork>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include<QTimer>
reservation::reservation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reservation),
    emailWidget(new emailyanshi(this)),
    m_pTimer(new QTimer(this)),
        m_nCountdown(60),// 初始化倒计时为60秒
          verificationCodeSent(false)// Initialize flag
{
    ui->setupUi(this);
    initDatabase();//初始化
    //连接reservation和emailshiyan的“发送”
    connect(ui->send,&QPushButton::clicked,emailWidget->getUI()->sendButton,&QPushButton::click);
    // 连接 emailshiyan 的验证码信号到 Register 的槽函数
    connect(emailWidget,&emailyanshi::verificationCodeSent,this,&reservation::receive);
    connect(ui->qqmail, &QLineEdit::editingFinished, this, &reservation::validateEmail);

    // 设置计时器槽函数
        connect(m_pTimer, &QTimer::timeout, this, &reservation::updateButtonText);

}

reservation::~reservation()
{
    delete ui;
    delete emailWidget;  // 释放 emailyanshi 对象
}

/*
//检查书号格式
void reservation::setupConnections(){
    // 连接编辑完成信号到槽函数
        connect(ui->Shuhao, &QLineEdit::editingFinished, this, &reservation::checkShuhaoFormat);
}
void reservation::checkShuhaoFormat() {
    QString shuhao = ui->Shuhao->text();
    QString pattern = "^[0-9]{4}-[0-9]{4}$";
    QRegularExpression re(pattern);

    if (!re.match(shuhao).hasMatch()) {
        QMessageBox::warning(this, "错误", "书号格式应为0000-0001！");
        ui->Shuhao->setFocus(); // 将焦点重新设置回书号输入框
    }
}
*/

//qq邮箱判断
void reservation::validateEmail(){
    QString email= ui->qqmail->text();
    QRegExp emailPattern("^[a-zA-Z0-9._%+-]+@qq\\.com$");
         if(!emailPattern.exactMatch(email)){
             QMessageBox::warning(this,"error","邮箱格式错误，必须是QQ邮箱！");
             ui->qqmail->clear();
             ui->qqmail->setFocus();
         }
}
void reservation::initDatabase()
{
    // 使用DatabaseManager获取数据库连接
    db = DatabaseManager::getDatabase();
    // 验证是否已经连接
    if (!db.open()) {
        qDebug() << db.lastError();
        qDebug() << "Database not connected.";
        return;
    } else {
        qDebug() << "Database connected.";
    }
}


void reservation::on_backButton_2_clicked()
{
     emit returntomode();
    this->hide();
}

void reservation::on_reservationButton_clicked()
{
    QString bookname;
    bookname=ui->bookname->text();//获取书名

    QString email;
    email=ui->qqmail->text();//获取邮箱

    QString incode;
    incode=ui->email->text();

    if(bookname==""){
        QMessageBox::warning(this,"","书名不能为空");
    }
    else{
    //检查书名是否存在
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT * FROM books WHERE title = ? ");
    checkQuery.addBindValue(bookname);

    if (!checkQuery.exec()) {
        qDebug() << "Check error:" << checkQuery.lastError();
        QMessageBox::warning(this, "error", "检查错误");
    }else if(!checkQuery.next()){
        QMessageBox::warning(this, "error", "书库中无此书");
    }else {

        if(!booknumber()){
        // Check if verification code has been sent
                if (!verificationCodeSent) {
                QMessageBox::warning(this, "error", "请先发送验证码。");
                return;
                }
        //验证码验证
                else if(incode!=verificationCode){
                QMessageBox::warning(this,"error","验证码错误！");
                return;
                }
                else {
                QMessageBox::information(this,"","预约成功，请在一天内取书");
                }

           }else {
                QMessageBox::information(this,"预约失败","该书还有余量，可直接去借阅");
                 }

        }
    }
}

// 实现 receive 槽函数
void reservation::receive(const QString &scode) {
    // 保存接收到的验证码
    verificationCode = scode;
    verificationCodeSent = true; // Mark the verification code as sent
}

void reservation::on_send_clicked()
{
    QString email;
    email=ui->qqmail->text();//获取邮箱
    validateEmail();
        // 将 QQ 邮箱传递给 emailyanshi 界面的接收邮箱行编辑框
        emailWidget->getUI()->Rcvemail_lineEdit->setText(email);

        // 触发 emailyanshi 类中的发送按钮点击事件
        emailWidget->getUI()->sendButton->click();
        // 发送验证码后，开始倒计时
            ui->send->setEnabled(false);
            m_nCountdown = 60;
            m_pTimer->start(1000);

}


// 更新按钮文本
void reservation::updateButtonText()
{
    if (m_nCountdown > 0) {
        ui->send->setText(QString::number(m_nCountdown) + "秒后重试");
        m_nCountdown--;
    } else {
        m_pTimer->stop();
        ui->send->setText("发送验证码");
        ui->send->setEnabled(true);
    }
}

bool reservation::booknumber()
{
    QString bookname;
    bookname=ui->bookname->text();//获取书名
    QSqlQuery querycurrent_stock(db);
    QString queryStr = "SELECT current_stock FROM books WHERE title = :title";
    querycurrent_stock.prepare(queryStr);
    querycurrent_stock.bindValue(":title", bookname);
    if (!querycurrent_stock.exec()) {
           qDebug() << "Error: Query failed:" << querycurrent_stock.lastError().text();
           return false;
     }
    if (querycurrent_stock.next()) {
            int stockQuantity = querycurrent_stock.value(0).toInt();
            qDebug() << "Stock quantity for book" << bookname << "is:" << stockQuantity;
            return stockQuantity>0;
    }
    return false;
}
