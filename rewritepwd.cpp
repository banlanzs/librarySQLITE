#include "rewritepwd.h"
#include "ui_rewritepwd.h"
#include"mainwindow.h"
#include "emailyanshi.h"
#include "ui_emailyanshi.h"
#include"DatabaseManager.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <cstdlib>
#include <ctime>
#include<QLineEdit>
#include<QSqlError>
#include<QSqlQuery>
#include <QMessageBox>
#include <QTime>
#include <QLibrary>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QTcpSocket>
#include <QTextStream>
#include<QTimer>
rewritepwd::rewritepwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rewritepwd),
     emailWidget(new emailyanshi(this)),
     timer(new QTimer(this)),  // 初始化QTimer
        countdown(60)  // 初始化倒计时60秒
{
    ui->setupUi(this);
    // 获取数据库连接
        db = DatabaseManager::getDatabase();
        if (!db.open()) {
            qDebug() << "Failed to open database:" << db.lastError();
            //QMessageBox::warning(this, "Database Error", "Failed to open the database. Please check the database settings.");
            qDebug()<<"数据库连接失败"<<endl;
        }

    //连接register和emailshiyan的“发送”
    connect(ui->sendCodeButton,&QPushButton::clicked,emailWidget->getUI()->sendButton,&QPushButton::click);
    // 连接 emailshiyan 的验证码信号到 Register 的槽函数
    connect(emailWidget,&emailyanshi::verificationCodeSent,this,&rewritepwd::receiveVerificationCode);
    // 设置计时器槽函数
        connect(timer, &QTimer::timeout, this, &rewritepwd::updateButtonText);
        connect(ui->Reuserid, &QLineEdit::editingFinished, this, &rewritepwd::checkInput);
}

rewritepwd::~rewritepwd()
{
    delete ui;
    delete emailWidget;
}


//发送验证码
void rewritepwd::on_sendCodeButton_clicked()
{
    QString inuserid=ui->Reuserid->text();//获取学号
    // 检查数据库连接是否打开
        if (!db.isOpen()) {
            //QMessageBox::warning(this, "Database Error", "连接失败.");
            return;
        }

        // 从数据库中获取对应学号的邮箱
        QSqlQuery query(db);
        query.prepare("SELECT email FROM new_users WHERE xuehao = ?");
        query.addBindValue(inuserid);

        if (!query.exec()) {
            qDebug() << "Database query error:" << query.lastError();
            QMessageBox::warning(this, "Error", "没有找到该用户的邮箱.");
            return;
        }

        if (!query.next()) {
            QMessageBox::warning(this, "Error", "该账号无用户可匹配.");
            return;
        }

        QString email = query.value(0).toString(); // 获取邮箱

        // 将 QQ 邮箱传递给 emailyanshi 界面的接收邮箱行编辑框
        emailWidget->getUI()->Rcvemail_lineEdit->setText(email);
        // 触发 emailyanshi 类中的发送按钮点击事件
        emailWidget->getUI()->sendButton->click();

        // 发送验证码后，开始倒计时
            ui->sendCodeButton->setEnabled(false);
            countdown = 60;
            timer->start(1000);
}

// 实现 receiveVerificationCode 槽函数
void rewritepwd::receiveVerificationCode(const QString &scode) {
    // 保存接收到的验证码
    verificationCode = scode;
}

//返回主界面
void rewritepwd::on_back2Button_clicked()
{
    this->close();
    MainWindow*m=new MainWindow();
    m->show();
}

//重置
void rewritepwd::on_rewritepwd_2_clicked()
{
    QString inuserid=ui->Reuserid->text();
      QString code=ui->qqemailMa->text();//获取用户输入的验证码
      QString newpwd=ui->newpwd->text();//获取用户输入的新密码
      QString confirmpwd=ui->confirmnewpwd->text();//再次输入密码确认
      //验证码是否正确
      if(code!=verificationCode){
          QMessageBox::warning(this,"error","验证码错误！");
          return;
      }
      //两次密码是否一致
      if(newpwd!=confirmpwd){
          QMessageBox::warning(this,"error","两次输入密码不一致！");
          return;
      }
      //数据库是否打开
      if(!db.open()){
          QMessageBox::warning(this, "Database Error", "未连接.");
                  return;
      }
      //更新数据库中的密码
          QSqlQuery query(db);
          query.prepare("UPDATE new_users SET password = ? WHERE xuehao = ?");
          query.addBindValue(newpwd);
          query.addBindValue(inuserid);//用来绑定学号参数，以便在 UPDATE 查询中指定要更新密码的特定用户

          if (!query.exec()) {
              qDebug() << "Database update error:" << query.lastError();
              QMessageBox::warning(this, "Error", "密码重置失败！");
              return;
          }

//          QMessageBox::information(this, "Success", "密码重置成功！");
          // 确认更新是否成功
              query.prepare("SELECT password FROM new_users WHERE xuehao = ?");
              query.addBindValue(inuserid);
              if (!query.exec()) {
                  qDebug() << "Database query error:" << query.lastError();
                  QMessageBox::warning(this, "Error", "密码重置失败.");
                  return;
              }

              if (query.next()) {
                  QString updatedPassword = query.value(0).toString();
                  if (updatedPassword == newpwd) {
                      QMessageBox::information(this, "Success", "密码重置成功.");
////////////////////////////////////////////// 自动登录
                              MainWindow *mainWindow = new MainWindow();
                              if (mainWindow->validateUser(inuserid, newpwd)) {
                                  mainWindow->show();
                                  this->close();
                                  mainWindow->close();
                              } else {
                                  delete mainWindow;
                                  QMessageBox::warning(this, "error", "自动登录失败。");
                              }
//////////////////////////////////////////////不能自己决定是否自动登录
                //修改为
//                      // 调用自动登录函数
//                                 onRegistrationOrPasswordResetSuccess(inuserid, newpwd);
                  } else {
                      QMessageBox::warning(this, "Error", "密码更新失败.");
                  }
              } else {
                  QMessageBox::warning(this, "Error", "更新密码后未找到该用户.");
              }
}

////自动登录
//void rewritepwd::onRegistrationOrPasswordResetSuccess(const QString &inuserid, const QString &newpwd) {
//    QMessageBox::StandardButton reply;
//    reply = QMessageBox::question(nullptr, "自动登录", "是否自动登录？",
//                                  QMessageBox::Yes | QMessageBox::No);

//    if (reply == QMessageBox::Yes) {
//        MainWindow *mainWindow = new MainWindow();
//        mainWindow->fillCredentials(inuserid, newpwd); // 假设有一个函数来填充账号和密码
//        if (mainWindow->validateUser(inuserid, newpwd)) {
//            //mainWindow->show();
//            mainWindow->on_LoginButton_clicked();
//            this->close();
//            mainWindow->close();
//        } else {
//            delete mainWindow;
//            QMessageBox::warning(nullptr, "错误", "自动登录失败。");
//        }
//    } else {
//        MainWindow *mainWindow = new MainWindow();
//        mainWindow->show();  // 打开主界面，但不填充账号和密码
//        this->close();
//    }
//}

// 更新按钮文本
void rewritepwd::updateButtonText()
{
    if (countdown > 0) {
        ui->sendCodeButton->setText(QString::number(countdown) + "秒后重试");
        countdown--;
    } else {
        timer->stop();
        ui->sendCodeButton->setText("发送验证码");
        ui->sendCodeButton->setEnabled(true);
    }
}


void rewritepwd::checkInput(){
        QString input=ui->Reuserid->text();
        if(input!="admin"&&(input.length()!=9||!input.toUInt())){
            QMessageBox::warning(this,"error","学号必须是9位数字。");
            ui->Reuserid->clear();
            ui->Reuserid->setFocus();
    }
}
