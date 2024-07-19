#include "returnbook.h"
#include "ui_returnbook.h"
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlError>
#include <QDebug>
#include<QRegularExpression>
#include<QKeySequenceEdit>
#include<QDateTime>
#include<QString>

returnbook::returnbook(QString&xuehao,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::returnbook),
    m_xuehao(xuehao)
{
    ui->setupUi(this);
    initDatabase();//初始化
    // 连接信号和槽函数
    //setupConnections();
}

returnbook::~returnbook()
{
    delete ui;
}

void returnbook::initDatabase()
{
    // 使用DatabaseManager获取数据库连接
    db = DatabaseManager::getDatabase();
    // 验证是否已经连接
    if (!db.open()) {
        qDebug() << db.lastError();
       // QMessageBox::warning(this, "error", "未连接到数据库");
        qDebug()<<"未连接到数据库"<<endl;
        return;
    } else {
        //QMessageBox::information(this, "success", "连接到数据库");
        qDebug()<<"连接到数据库"<<endl;
    }
}

void returnbook::on_returnButton_clicked()
{
    QString bookname;
    bookname=ui->bookname->text();//获取书名
    if(bookname==""){
        QMessageBox::warning(this,"","书名不能为空");
     }
/*
    QSqlQuery checkQuery2(db);
    checkQuery2.prepare("SELECT * FROM borrow WHERE title = ? ");
    checkQuery2.addBindValue(bookname);
*/
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT * FROM books WHERE title = ?");
    checkQuery.addBindValue(bookname);
    if (!checkQuery.exec()) {
        qDebug() << "Check error:" << checkQuery.lastError();
        QMessageBox::warning(this, "error", "检查错误");
    }else if(!checkQuery.next()){
        QMessageBox::warning(this, "error", "书名不存在");
    }
/*    if (!checkQuery2.exec()) {
        qDebug() << "Check error:" << checkQuery2.lastError();
        QMessageBox::warning(this, "error", "检查错误");
    }else  if(!checkQuery2.next()){
        QMessageBox::warning(this, "error", "未借阅该书或已经归还");
    }*/
    else {
       // QMessageBox::information(this, "", "请选择要归还的书！");
       int selectedRow =ui-> tableWidget->currentRow();
            if (selectedRow >= 0) {
            QString bookId = ui->tableWidget->item(selectedRow, 0)->text();
            QSqlQuery query(db);
            query.prepare("DELETE FROM borrow WHERE title = :title");
            query.bindValue(":title",bookname);
            loadBooks();
            if (!query.exec()) {
                qDebug() << "执行失败：" << query.lastError().text();
                QMessageBox::warning(this, "error", "归还书籍失败！");
                loadBooks();
                return;
            }else{
                QMessageBox::information(this, "success", "已成功归还");
                loadBooks();
                int current_stock=checkQuery.value("current_stock").toInt();
                int newcurrent_stock = current_stock +1;
                QSqlQuery updatecheckQuery(db);
                updatecheckQuery.prepare("UPDATE books SET current_stock = ? WHERE title = ?");
                updatecheckQuery.addBindValue(newcurrent_stock);
                updatecheckQuery.addBindValue(bookname);
                updatecheckQuery.exec();
            }
    }else {QMessageBox::information(this, "", "请选择要归还的书！");}
  }
}

void returnbook::on_backButton_2_clicked()
{
    emit returntomode();
    this->hide();
}
void returnbook::loadBooks(){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM borrow WHERE xuehao=:xuehao");
    query.bindValue(":xuehao", m_xuehao);
    if (!query.exec()) {
        qDebug() << "Error loading borrow:" << query.lastError();
        return;
    }

    ui->tableWidget->setRowCount(0); // Clear existing rows
    int row = 0;
    while (query.next()) {
        QString shuhao = query.value("book_id").toString();
        QString bookname=query.value("title").toString();
        QString xuehao = query.value("xuehao").toString();
        QString borrowdate = query.value("borrow_time").toString();
        QDateTime borrow_date=QDateTime::currentDateTime();
        QDateTime returndate=borrow_date.addMonths(1);
        QString return_date=returndate.toString("yyyy-MM-dd");

        // Insert row into the table
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(shuhao));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(bookname));
        //ui->tableWidget->setItem(row, 2, new QTableWidgetItem(xuehao));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(borrowdate));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(return_date));

        ++row;
    }
}
void returnbook::on_searchButton_clicked()
{
    //根据名字查询
    QString sql = QString("select * from borrow where title =\"%1\";").arg(ui->bookname->text());
    //创建一个可以对db执行语句的对象
    QSqlQuery query(db);
    //执行sql语句
    bool ret = query.exec(sql);
    if(ret == false)
    {
        qDebug()<<"select error "<<query.lastError().text();
    }
    //查询的结果
    int id = query.value(0).toInt();
    //行坐标向下移
    if(query.next())
    {
    //获取数据库query所指的那行的数据
    ui->tableWidget->setRowCount(id); // Clear existing rows
    QString shuhao = query.value("book_id").toString();
    QString bookname=query.value("title").toString();
    QString xuehao = query.value("xuehao").toString();
    QString borrowdate = query.value("borrow_time").toString();
    //int current_stock=query.value("current_stock").toInt();
    QDateTime borrow_date=QDateTime::currentDateTime();
    QDateTime returndate=borrow_date.addMonths(1);
    QString return_date=returndate.toString("yyyy-MM-dd");


    ui->tableWidget->insertRow(id);
    ui->tableWidget->setItem(id, 0, new QTableWidgetItem(shuhao));
    ui->tableWidget->setItem(id, 1, new QTableWidgetItem(bookname));
    //ui->tableWidget->setItem(id, 2, new QTableWidgetItem(xuehao));
    ui->tableWidget->setItem(id, 2, new QTableWidgetItem(borrowdate));
    ui->tableWidget->setItem(id, 3, new QTableWidgetItem(return_date));
    }
    else {
        QMessageBox::warning(this, "Error", "查询无结果");
    }
}

void returnbook::on_refreshButton_clicked()
{
    loadBooks();
    qDebug()<<"refresh";
}
