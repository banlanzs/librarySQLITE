//zhoushuang
#include "managebooks.h"
#include "ui_managebooks.h"
#include"DatabaseManager.h"
#include"borrowbook.h"
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QMessageBox>
#include<QAbstractItemView>
#include<QTimer>
managebooks::managebooks(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::managebooks)
{
    ui->setupUi(this);
    initDatabase();//初始化数据库连接
    loadBooks();// Load books data into the UI
}

managebooks::~managebooks()
{
    delete ui;
}

void managebooks::initDatabase(){
//    db = QSqlDatabase::addDatabase("QSQLITE", "manage_connection");
//        db.setDatabaseName("library.db");
    db = DatabaseManager::getDatabase();
        if (!db.open()) {
            qDebug() << "Error: Failed to connect database." << db.lastError();
            return;
        } else {
            qDebug() << "Database connected.";
        }
}
void managebooks::loadBooks(){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM books");
    if (!query.exec()) {
        qDebug() << "Error loading books:" << query.lastError();
        return;
    }

    ui->booksWidget->setRowCount(0); // Clear existing rows
    int row = 0;
    while (query.next()) {
        QString shuhao = query.value("book_id").toString();
        QString bookname=query.value("title").toString();
        QString writer = query.value("author").toString();
        QString publisher = query.value("publisher").toString();
        int total_stock=query.value("total_stock").toInt();
        int current_stock=query.value("current_stock").toInt();

        // Insert row into the table
        ui->booksWidget->insertRow(row);
        ui->booksWidget->setItem(row, 0, new QTableWidgetItem(shuhao));
        ui->booksWidget->setItem(row, 1, new QTableWidgetItem(bookname));
        ui->booksWidget->setItem(row, 2, new QTableWidgetItem(writer));
        ui->booksWidget->setItem(row, 3, new QTableWidgetItem(publisher));
        ui->booksWidget->setItem(row, 4, new QTableWidgetItem(QString::number(total_stock)));
        ui->booksWidget->setItem(row, 5, new QTableWidgetItem(QString::number(current_stock)));

        ++row;
    }
}
//关闭
void managebooks::on_closeButton_clicked()
{
    emit returntomode();  // Emit the signal to return to the Mode window
    this->hide();  // Hide the ManageUsers window
}
//刷新
void managebooks::on_refreshButton_clicked()
{
    loadBooks();
    qDebug()<<"refresh";
}

//删除库存
void managebooks::on_cleanButton_clicked()
{
    int selectedRow =ui-> booksWidget->currentRow();
        if (selectedRow >= 0) {
            QString bookId = ui->booksWidget->item(selectedRow, 0)->text();
        QSqlQuery query(db);
        query.prepare("DELETE FROM books WHERE book_id = :bookId");
        query.bindValue(":bookId",bookId);
        loadBooks();
        if (!query.exec()) {
            qDebug() << "执行失败：" << query.lastError().text();
            QMessageBox::warning(this, "error", "删除书籍失败！");
            loadBooks();
            return;
        }else{
            QMessageBox::information(this, "success", "已删除");
            loadBooks();
        }
    }
}


//下架归档
