//zhoushuang
#include "manageusers.h"
#include "ui_manageusers.h"
#include"mainwindow.h"
#include"DatabaseManager.h"
#include"emailyanshi.h"
#include "ui_emailyanshi.h"//没有这个，getUI()->sendButton会出错
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QDateTime>
#include <ctime>    // For time()
#include<QTimer>
#include<QDebug>
ManageUsers::ManageUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageUsers),
    emailWidget(new emailyanshi(this)),
    m_pTimer(new QTimer(this)),
        m_nCountdown(60),// 初始化倒计时为60秒
        allowSignal(false)
{
    ui->setupUi(this);
    initDatabase();// Initialize database connection
        loadUsers();// Load users data into the UI
       //connect(发送对象，信号，接收对象，槽函数)，其中发送信号和槽函数需要用 SIGNAL() 和 SLOT() 来进行声明。
        connect(ui->usersTable, &QTableWidget::cellClicked, this, &ManageUsers::onUserSelectionChanged);
            connect(ui->refreshButton, &QPushButton::clicked, this, &ManageUsers::on_refreshButton_clicked);
            //connect(ui->remindreturnButton, &QPushButton::clicked, emailWidget->getUI()->sendButton,&QPushButton::click);
            connect(ui->remindreturnButton, &QPushButton::clicked, this, [this]() {
                if (allowSignal) {
                    emit emailWidget->getUI()->sendButton->click();
                }
            });

            // 设置计时器槽函数
                connect(m_pTimer, &QTimer::timeout, this, &ManageUsers::updateButtonText);
}

ManageUsers::~ManageUsers()
{
    delete ui;
    delete emailWidget;
    delete m_pTimer;
}

void ManageUsers::initDatabase(){
//    db = QSqlDatabase::addDatabase("QSQLITE", "manage_connection");
//        db.setDatabaseName("library.db");//多次创建会导致重复出错
    db = DatabaseManager::getDatabase();
        if (!db.open()) {
            qDebug() << "Error: Failed to connect database." << db.lastError();
        } else {
            qDebug() << "Database connected.";
        }
}

void ManageUsers::loadUsers()
{
    QSqlQuery query(db);
    query.prepare("SELECT * FROM new_users");
    if (!query.exec()) {
        qDebug() << "Error loading new_users:" << query.lastError();
        return;
    }

    ui->usersTable->setRowCount(0); // Clear existing rows
    int row = 0;
    while (query.next()) {
        QString userid = query.value("xuehao").toString();
        QString pwd=query.value("password").toString();
        QString email = query.value("email").toString();
        QString createTime = query.value("create_time").toString();
        bool isOnline = query.value("is_online").toBool();

        // Insert row into the table
        ui->usersTable->insertRow(row);
        ui->usersTable->setItem(row, 0, new QTableWidgetItem(userid));
        ui->usersTable->setItem(row, 1, new QTableWidgetItem(pwd));
        ui->usersTable->setItem(row, 2, new QTableWidgetItem(email));
        ui->usersTable->setItem(row, 3, new QTableWidgetItem(createTime));
        ui->usersTable->setItem(row, 4, new QTableWidgetItem(isOnline ? "Online" : "Offline"));

        ++row;
    }
}

void ManageUsers::onUserSelectionChanged()
{
    int row = ui->usersTable->currentRow();
    QString xuehao = ui->usersTable->item(row, 0)->text();
    loadborrow(xuehao);
}

void ManageUsers::loadborrow(const QString&xuehao){
    QSqlQuery query(db);
    query.prepare("SELECT book_id,title,borrow_time, created_at FROM borrow WHERE xuehao = :xuehao");
        query.bindValue(":xuehao", xuehao);
        if (!query.exec()) {
            qDebug() << "Error loading borrowing data:" << query.lastError();
            return;
        }
        ui->borrowTableWidget->setRowCount(0); // Clear existing rows
            int row = 0;
            QString borrowdate = query.value("borrow_time").toString();
            QDateTime borrow_date=QDateTime::currentDateTime();
            QDateTime returndate=borrow_date.addMonths(1);
            QString return_date=returndate.toString("yyyy-MM-dd");
            while (query.next()) {
                ui->borrowTableWidget->insertRow(row);
                ui->borrowTableWidget->setItem(row, 0, new QTableWidgetItem(query.value("book_id").toString()));
                ui->borrowTableWidget->setItem(row, 1, new QTableWidgetItem(query.value("title").toString()));
                ui->borrowTableWidget->setItem(row, 2, new QTableWidgetItem(query.value("borrow_time").toString()));
                ui->borrowTableWidget->setItem(row, 3, new QTableWidgetItem(return_date));
                row++;
            }
}

//刷新
void ManageUsers::on_refreshButton_clicked()
{
    loadUsers(); // Reload new_users data when refresh button is clicked
    int row = ui->usersTable->currentRow();
        if (row >= 0) {
            QString xuehao = ui->usersTable->item(row, 0)->text();
            loadborrow(xuehao);
        }
}

//返回
void ManageUsers::on_closeButton_clicked()
{
//    this->close(); // Close the manage new_users dialog
//    MainWindow*m=new MainWindow();
//    m->show();
    emit returntomode();  // Emit the signal to return to the Mode window
    this->hide();  // Hide the ManageUsers window
}

//提醒归还
void ManageUsers::on_remindreturnButton_clicked()
{
    int row = ui->usersTable->currentRow();
    QString xuehao = ui->usersTable->item(row, 0)->text();
    QSqlQuery query(db);
   query.prepare("SELECT title FROM borrow WHERE xuehao = :xuehao AND julianday(created_at)-julianday(borrow_time)=7");//SQLITE
    //query.prepare("SELECT title FROM borrow WHERE xuehao = :xuehao AND DATEDIFF(created_at, borrow_time)=7");//MySQL
    query.bindValue(":xuehao", xuehao);
    QStringList booknames;
    if (query.exec()) {
        while (query.next()) {  // Move to the first record
            booknames<<query.value(0).toString(); //从检索结果的第一列获得该用户邮箱
            qDebug() << "将被提醒还书的用户book retrieved:" << booknames;
            //获取所有符合条件的书名
        }
        if (booknames.isEmpty()) {
            //qDebug() << "No book found for the provided xuehao.";
            QMessageBox::warning(this,"error","该用户没有需要提醒归还的书！");
            allowSignal=false;
            return;
    } else{
            qDebug() << "将被提醒还书的用户books retrieved:" << booknames;
            // 将书名列表连接成一个字符串，用逗号分隔
                    QString booknamesString = booknames.join("，");
                    QString message = QString("【图书馆】 尊敬的用户，您好！您的《%1》的可借阅时长还剩下7天，请你在7天内进行归还，谢谢！").arg(booknamesString);
                    emailWidget->getUI()->textEdit_Text->setText(message);

                    allowSignal=true;
                    query.prepare("SELECT email from new_users WHERE xuehao = :xuehao");
                                query.bindValue(":xuehao", xuehao);
                                QString email;
                                if (query.exec()) {
                                    if (query.next()) {  // Move to the first record
                                        email = query.value(0).toString(); //从检索结果的第一列获得该用户邮箱
                                        qDebug() << "将被提醒还书的用户Email retrieved:" << email;
                                    } else {
                                        qDebug() << "No email found for the provided xuehao.";
                                    }
                                } else {
                                    qDebug() << "找emailQuery execution error:" << query.lastError().text();
                                }
                                emailWidget->getUI()->Rcvemail_lineEdit->setText(email);
                                updateButtonText();
                    // 触发 emailyanshi 类中的发送按钮点击事件
                    emailWidget->getUI()->sendButton->click();
                    // 发送验证码后，开始倒计时
                        ui->remindreturnButton->setEnabled(false);
                        m_nCountdown = 60;
                        m_pTimer->start(1000);
        }
   }else {
        qDebug() << "找要还的书Query execution error:" << query.lastError().text();
    }
}

// 更新按钮文本
void ManageUsers::updateButtonText()
{
    if (m_nCountdown > 0) {
        ui->remindreturnButton->setText(QString::number(m_nCountdown) + "秒后重试");
        m_nCountdown--;
    } else {
        m_pTimer->stop();
        ui->remindreturnButton->setText("提醒归还");
        ui->remindreturnButton->setEnabled(true);
    }
}
