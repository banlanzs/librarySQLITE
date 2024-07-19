#include "register.h"
#include "ui_register.h"
#include"mainwindow.h"
#include "emailyanshi.h"//发邮件
#include "ui_emailyanshi.h"//没有这个，getUI()->sendButton会出错
#include "DatabaseManager.h"
#include<QDebug>//输出流
#include<QMessageBox>//消息盒子
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include<QNetworkReply>
#include<QSqlDatabase>//连接数据库
#include<QSqlQuery>
#include<QSqlError>
#include<QRegExp>
#include <QDateTime>
#include<QTcpSocket>
#include <QtNetwork>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include<QTimer>
#include<QDebug>
Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register),
    emailWidget(new emailyanshi(this)),
            m_pTimer(new QTimer(this)),
                m_nCountdown(60),// 初始化倒计时为60秒
              verificationCodeSent(false) // Initialize flag
{
    ui->setupUi(this);
/////////////////////////////////////////////////////////
    initDatabase();//要在这里初始化，不然就会出现Insert error: QSqlError("", "Parameter count mismatch", "")
/////////////////////////////////////////////////////////
    // 连接信号和槽
        connect(ui->userid, &QLineEdit::editingFinished, this, &Register::checkUserID);
        //connect(ui->qqmail, &QLineEdit::editingFinished, this, &Register::checkEmail);
        connect(ui->qqmail, &QLineEdit::editingFinished, this, &Register::validateEmail);


        //连接register和emailshiyan的“发送”
        connect(ui->sendCodeButton,&QPushButton::clicked,emailWidget->getUI()->sendButton,&QPushButton::click);
        // 连接 emailshiyan 的验证码信号到 Register 的槽函数
        connect(emailWidget,&emailyanshi::verificationCodeSent,this,&Register::receiveVerificationCode);
        // 设置计时器槽函数
            connect(m_pTimer, &QTimer::timeout, this, &Register::updateButtonText);
            connect(ui->userid, &QLineEdit::editingFinished, this, &Register::checkInput);
}

Register::~Register()
{
    delete ui;
   delete emailWidget;  // 释放 emailyanshi 对象
  delete m_pTimer; // 释放QTimer
}

//密码格式控制（6~20位数字、字母、特殊字符组合）
bool Register::validatePassword(const QString &password) {
    // 正则表达式：密码长度为6到20位，必须包含字母、数字和特殊字符
    QRegExp passwordPattern("^(?=.*[a-zA-Z])(?=.*\\d)(?=.*[^a-zA-Z\\d]).{6,20}$");
    return passwordPattern.exactMatch(password);
}

//qq邮箱判断
//bool Register::validateEmail(const QString &email){
//    QRegExp emailPattern("^[a-zA-Z0-9._%+-]+@qq\\.com$");
//        return emailPattern.exactMatch(email);
//}
void Register::validateEmail(){
    QString email= ui->qqmail->text();
    QRegExp emailPattern("^[a-zA-Z0-9._%+-]+@qq\\.com$");
         if(!emailPattern.exactMatch(email)){
             QMessageBox::warning(this,"error","邮箱格式错误，必须是QQ邮箱！");
             ui->qqmail->clear();
             ui->qqmail->setFocus();
             //ui->sendCodeButton->setEnabled(false);
         }
}

void Register::initDatabase(){
//    //首先初始化这个在头文件中声明的SqlDatabase对象
//    db=QSqlDatabase::addDatabase("QSQLITE", "library_connection");
//    db.setDatabaseName("library.db");//多个地方创建使用了数据库连接导致重复

    db = DatabaseManager::getDatabase();
    //验证是否已经连接
    if(!db.open()){
        qDebug()<<db.lastError();
        //QMessageBox::warning(this,"error","register未连接到数据库");
        qDebug()<<"register未连接到数据库"<<endl;
        return;
    }else{
        //QMessageBox::information(this,"sucess","register连接到数据库");
        qDebug()<<"register连接到数据库"<<endl;
    }
//    // 检查实际索引名称
//        QSqlQuery indexQuery(db);
//        QString actualIndexName;

////        if (indexQuery.exec("SHOW INDEX FROM users WHERE Column_name = 'email' AND Non_unique = 0")) {
////            while (indexQuery.next()) {
////                actualIndexName = indexQuery.value("Key_name").toString();
////                qDebug()<<actualIndexName<<endl;
////                break; // 假设只有一个唯一索引
////            }
////        }

//        if (!actualIndexName.isEmpty()) {
//            // 删除email字段的唯一性约束
//            QSqlQuery dropIndexQuery(db);
//            QString dropIndexSQL = QString("ALTER TABLE users DROP INDEX %1").arg(actualIndexName);
//            if (!dropIndexQuery.exec(dropIndexSQL)) {
//                qDebug() << "Drop index error:" << dropIndexQuery.lastError();
//            }
//        }

//        // 检查和更新现有数据，确保没有重复的 email 条目
//        QSqlQuery checkDuplicateQuery(db);
//        checkDuplicateQuery.exec("SELECT email, COUNT(*) as count FROM users GROUP BY email HAVING count > 1");
//        while (checkDuplicateQuery.next()) {
//            QString email = checkDuplicateQuery.value(0).toString();
//            qDebug() << "重复使用的 email found:" << email;
//            // 这里可以添加代码来处理重复的 email 条目，例如删除或修改它们
//        }

//        // 修改email字段定义
//        QSqlQuery modifyFieldQuery(db);
//        if (!modifyFieldQuery.exec("ALTER TABLE users MODIFY email TEXT NOT NULL")) {
//            qDebug() << "Modify field error:" << modifyFieldQuery.lastError();
//        }

///*
//    //    // 删除email字段的唯一性约束
////        QSqlQuery dropIndexQuery(db);
////        if (!dropIndexQuery.exec("ALTER TABLE users DROP INDEX email_UNIQUE;")) {
////            qDebug() << "Drop index error:" << dropIndexQuery.lastError();
////        }

////        // 检查和更新现有数据，确保没有重复的 email 条目
////        QSqlQuery checkDuplicateQuery(db);
////        checkDuplicateQuery.exec("SELECT email, COUNT(*) as count FROM users GROUP BY email HAVING count > 1");
////        while (checkDuplicateQuery.next()) {
////            QString email = checkDuplicateQuery.value(0).toString();
////            qDebug() << "Duplicate email found:" << email;
////            // 这里可以添加代码来处理重复的 email 条目，例如删除或修改它们
////        }

////        // 修改email字段定义
////        QSqlQuery modifyFieldQuery(db);
////        if (!modifyFieldQuery.exec("ALTER TABLE users MODIFY email VARCHAR(255) NOT NULL;")) {
////            qDebug() << "Modify field error:" << modifyFieldQuery.lastError();
////        }
//    //////////////////email索引使用错误，不是email_UNIQUE
//*/

//    //////////////////////////////////////////////////////////////////////
//    //检查表是否存在，并检查是否需要添加新列
//        QSqlQuery query(db);
//        if (query.exec("PRAGMA table_info(users)")) {//SQLITE的
//        //if (query.exec("DESCRIBE users")) {//MYSQL的
//            bool createTimeExists = false;
//            while (query.next()) {
//                //QString columnName = query.value(0).toString();
//                QString columnName = query.value(1).toString();//sqlite的,在MySQL会导致Add create_time column error: QSqlError("1060", "QMYSQL: Unable to execute query", "Duplicate column name 'create_time'")
//                if (columnName == "create_time") {
//                    createTimeExists = true;
//                    break;
//                }
//            }
//            if (!createTimeExists) {
//                // 添加 create_time 列
//                QSqlQuery alterTableQuery(db);
////                if (!alterTableQuery.exec("ALTER TABLE users ADD COLUMN create_time TEXT NOT NULL DEFAULT ''")) {
//                if (!alterTableQuery.exec("ALTER TABLE users ADD COLUMN create_time VARCHAR(255) NOT NULL DEFAULT ''")) {
//                    qDebug() << "Add create_time column error:" << alterTableQuery.lastError();
//                } else {
//                    qDebug() << "Column create_time added successfully.";
//                }
//            }
//        } else {
//            qDebug() << "create_time Table info error:" << query.lastError();
//        }


    //构建users表
    //sql语句
    QString new_sql("CREATE TABLE IF NOT EXISTS new_users("//检查表是否存在
                "xuehao text not null,"
                "password text not null ,"
                "email text not null ,"
                "create_time text not null,"
                "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                "is_online INTEGER DEFAULT 0)");//此处表明email不为空，则后面插入管理员时不能为空，随便写一个替代即可

//        QString sql("CREATE TABLE IF NOT EXISTS users("//检查表是否存在
//                       "xuehao VARCHAR(255) NOT NULL,"
//                       "password VARCHAR(255) NOT NULL,"
//                       "email VARCHAR(255) NOT NULL ,"
//                       "create_time VARCHAR(255) NOT NULL,"
//                       "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
//                       "is_online INTEGER DEFAULT 0)"); //此处表明email不为空，则后面插入管理员时不能为空，随便写一个替代即可



    //建表（table）
    QSqlQuery createTableQuery(db);//声明一个QsqlQuery对象
    //createTableQuery.exec(sql);//执行sql
    if (!createTableQuery.exec(new_sql)) {
            qDebug() << "Create new_users table error:" << createTableQuery.lastError();
        }else{
        qDebug()<<"Create new_users table sucess.";
    }
//    // 检查和更新现有数据，确保没有重复的 email 条目
//        QSqlQuery checkDuplicateQuery(db);
//        checkDuplicateQuery.exec("SELECT email, COUNT(*) as count FROM new_users GROUP BY email HAVING count > 1");
//        while (checkDuplicateQuery.next()) {
//            QString email = checkDuplicateQuery.value(0).toString();
//            qDebug() << "重复使用的 email found:" << email;
//        }

//        // 修改 email 字段定义
//        QSqlQuery modifyFieldQuery(db);
//        if (!modifyFieldQuery.exec("ALTER TABLE users MODIFY email VARCHAR(255) NOT NULL")) {
//            qDebug() << "Modify field error:" << modifyFieldQuery.lastError();
//        }

        // 检查表是否存在，并检查是否需要添加新列
        QSqlQuery query(db);
        if (query.exec("PRAGMA table_info(new_users)")) {
            bool createTimeExists = false;
            while (query.next()) {
                QString columnName = query.value(1).toString();
                if (columnName == "create_time") {
                    createTimeExists = true;
                    break;
                }
            }
            if (!createTimeExists) {
                QSqlQuery alterTableQuery(db);
                if (!alterTableQuery.exec("ALTER TABLE new_users ADD COLUMN create_time TEXT NOT NULL DEFAULT ''")) {
                    qDebug() << "Add create_time column error:" << alterTableQuery.lastError();
                } else {
                    qDebug() << "Column create_time added successfully.";
                }
            }
        } else {
            qDebug() << "create_time Table info error:" << query.lastError();
        }

//        // 构建 users 表
//        QString new_sql("CREATE TABLE IF NOT EXISTS new_users("
//                    "xuehao TEXT NOT NULL,"
//                    "password TEXT NOT NULL,"
//                    "email TEXT NOT NULL,"
//                    "create_time TEXT NOT NULL,"
//                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
//                    "is_online INTEGER DEFAULT 0)");

//        QSqlQuery createTableQuery(db);
//        if (!createTableQuery.exec(sql)) {
//            qDebug() << "Create new_users table error:" << createTableQuery.lastError();
//        } else {
//            qDebug() << "Create new_users table success.";
//        }
////////////////////////////////////////////////////////////////////////
    // 检查并添加管理员账户
        QString checkAdminSql = "SELECT * FROM new_users WHERE xuehao = 'admin'";
        QSqlQuery checkAdminQuery(db);
        if (!checkAdminQuery.exec(checkAdminSql)) {
            qDebug() << "Check admin error:" << checkAdminQuery.lastError();
        } else if (!checkAdminQuery.next()) {
            // 管理员账户不存在，插入管理员账户
            QString insertAdminSql = "INSERT INTO new_users (xuehao, password, email,create_time) VALUES ('admin', 'admin123', 'admin@example.com','2024.7.10')";
            QSqlQuery insertAdminQuery(db);
            if (!insertAdminQuery.exec(insertAdminSql)) {
                qDebug() << "Insert admin error:" << insertAdminQuery.lastError();
            } else {
                qDebug() << "Admin account created.";
            }
        } else {
            qDebug() << "Admin account already exists.";
        }

//        //关闭数据库连接
//        db.close();
//        QSqlDatabase::removeDatabase("library_connection");
        //在注册类中关闭和移除数据库连接是不必要的，
        //尤其是当其他类还需要使用同一个数据库连接时。
        //过早地关闭和移除连接会导致其他地方的数据库操作失败。
        //应该确保在应用程序的整个生命周期中维护一个有效的数据库连接，
        //并在应用程序结束时再关闭和移除它。
}

//在学号输入框就检查是否已经是图书馆账号
void Register::checkUserID() {
    QString userid = ui->userid->text();
    if (userid.isEmpty()) return;

    QSqlQuery checkUserQuery(db);
    checkUserQuery.prepare("SELECT * FROM users WHERE xuehao = ?");
    checkUserQuery.addBindValue(userid);
    if (!checkUserQuery.exec()) {
        qDebug() << "Check user error:" << checkUserQuery.lastError();
    } else if (checkUserQuery.next()) {
        QMessageBox::warning(this, "error", "该学号已有图书馆账号");
        ui->userid->clear();
        ui->userid->setFocus();
    }
}

/*
//在邮箱输入框检查邮箱是否被用过
void Register::checkEmail() {
    QString email = ui->qqmail->text();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE email = ?");
    query.addBindValue(email);
    if (query.exec() && query.next() && query.value(0).toInt() > 0) {
        QMessageBox::warning(this, "错误", "该QQ邮箱已经被使用");
        ui->qqmail->clear();
        ui->qqmail->setFocus();
    }
}
*/

//注册功能
void Register::saveToDatabase(){
        QString userid=ui->userid->text();//获取学号
        QString pwd=ui->userpwd->text();//获取密码;
        QString confpwd=ui->confirmuserpwd->text();//确认密码
        QString email=ui->qqmail->text();//获取QQ邮箱
        QString incode = ui->codeEdit->text();//获取验证码

        // 获取当前时间作为注册时间
            QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        //学号密码不为空
        if(userid==""){
            QMessageBox::warning(this,"error","学号不能为空");
        }else if(pwd==""){
            QMessageBox::warning(this,"error","密码不为空");
        }

        //检查学号是否符合规范(9位数字长度)，10位会超大小(管理员除外，admin)
        if(userid!="admin"&&(userid.length()!=9||!userid.toUInt())){
            QMessageBox::warning(this,"error","学号必须是9位数字。");
            return;
        }

        //检查密码和确认密码格式
        if (!validatePassword(pwd)&&!validatePassword(confpwd)) {
                QMessageBox::warning(this, "error", "密码格式不正确");
                return;
            }

        //检查两次输入的密码是否一致
        if(pwd!=confpwd){
            QMessageBox::warning(this,"error","两次输入的密码不一致。");
            return;
        }

        //检查qq邮箱格式
//        if (!validateEmail(email)) {
//                QMessageBox::warning(this, "error", "邮箱格式不正确，必须是QQ邮箱。");
//                return;
//            }
/////////////////////////////////////////////否则不发送验证码，不管验证码也能注册
        // Check if verification code has been sent
            if (!verificationCodeSent) {
                QMessageBox::warning(this, "error", "请先发送验证码。");
                return;
            }
        //验证码验证
        if(incode!=verificationCode){
            QMessageBox::warning(this,"error","验证码错误！");
            return;
        }

        //插入新用户
        QSqlQuery insertQuery(db);//创建对象
        QString sql=QString("INSERT INTO new_users(xuehao,password,email,create_time) VALUES (?,?,?,?)");
        insertQuery.prepare(sql);//准备执行插入
        insertQuery.addBindValue(userid);//插入学号
        insertQuery.addBindValue(pwd);//插入密码
        insertQuery.addBindValue(email);//插入邮箱
        insertQuery.addBindValue(currentTime);

        if (!insertQuery.exec()) {
                qDebug() << "Insert error:" << insertQuery.lastError();
                QMessageBox::warning(this, "error", "注册失败。");
            } else {
                QMessageBox::information(this, "success", "注册成功！");
/////////////////////////// 自动登录
                        MainWindow *mainWindow = new MainWindow();
                        if (mainWindow->validateUser(userid, pwd)) {
                            mainWindow->show();
                            this->close();
                            mainWindow->close();
                        } else {
                            delete mainWindow;
                            QMessageBox::warning(this, "error", "自动登录失败。");
                        }
/////////////////////////////不能自己决定是否自动登录
//           //改为
//                // 调用自动登录函数
//                           onRegistrationOrPasswordResetSuccess(userid, pwd);
            }

}

////自动登录
//void Register ::onRegistrationOrPasswordResetSuccess(const QString &inuserid, const QString &newpwd) {
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

//确认注册
void Register::on_confirmRegister_clicked()
{
    saveToDatabase();
}

//取消/返回
void Register::on_BackButton_clicked()
{
    this->close();//当前注册窗口关闭
    // 打开登录窗口
        MainWindow *loginWindow = new MainWindow();
        loginWindow->show();
}

//发送验证码
void Register::on_sendCodeButton_clicked()
{
    // 获取用户输入的 QQ 邮箱
        QString email = ui->qqmail->text();
//        if (!validateEmail(email)) {
//                QMessageBox::warning(this, "error", "邮箱格式不正确，必须是QQ邮箱。");
//                ui->qqmail->clear();
//                ui->qqmail->setFocus();
//                return;
//            }
        validateEmail();

        // 将 QQ 邮箱传递给 emailyanshi 界面的接收邮箱行编辑框
        emailWidget->getUI()->Rcvemail_lineEdit->setText(email);

        // 触发 emailyanshi 类中的发送按钮点击事件
        emailWidget->getUI()->sendButton->click();
        // 发送验证码后，开始倒计时
            ui->sendCodeButton->setEnabled(false);
            m_nCountdown = 60;
            m_pTimer->start(1000);
}

// 实现 receiveVerificationCode 槽函数
void Register::receiveVerificationCode(const QString &scode) {
    // 保存接收到的验证码
    verificationCode = scode;
    verificationCodeSent = true; // Mark the verification code as sent
}

// 更新按钮文本
void Register::updateButtonText()
{
    if (m_nCountdown > 0) {
        ui->sendCodeButton->setText(QString::number(m_nCountdown) + "秒后重试");
        m_nCountdown--;
    } else {
        m_pTimer->stop();
        ui->sendCodeButton->setText("发送验证码");
        ui->sendCodeButton->setEnabled(true);
    }
}

void Register::checkInput(){

        QString input=ui->userid->text();
        if((input.length()!=9||!input.toUInt())){
            QMessageBox::warning(this,"error","学号必须是9位数字。");
            ui->userid->clear();
            ui->userid->setFocus();
    }
}
