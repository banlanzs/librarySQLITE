#include "borrowbook.h"
#include "ui_borrowbook.h"
#include"DatabaseManager.h"
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QMessageBox>
#include<QDateTime>
#include<QString>

borrowbook::borrowbook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::borrowbook)
{
    ui->setupUi(this);
    initDatabase();//初始化数据库连接
}

void borrowbook::initDatabase()
{
    // 使用DatabaseManager获取数据库连接
    db = DatabaseManager::getDatabase();
    // 验证是否已经连接
    if (!db.open()) {
        qDebug() << db.lastError();
        //QMessageBox::warning(this, "error", "borrowbook未连接到数据库");
        qDebug()<<"borrowbook未连接到数据库"<<endl;
        return;
    } else {
        //QMessageBox::information(this, "success", "borrowbook连接到数据库");
        qDebug()<<"borrowbook连接到数据库"<<endl;
    }

    // 构建borrow表
//    QString sql("CREATE TABLE IF NOT EXISTS borrow("//检查表是否存在
//                "book_id text not null,"
//                "title text not null ,"
//                "xuehao text not null ,"
//                "borrow_time text not null,"
//                "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
//                "is_online INTEGER DEFAULT 0)");
    QString sql("CREATE TABLE IF NOT EXISTS borrow ("
                    "book_id VARCHAR(255) NOT NULL,"
                    "title VARCHAR(255) NOT NULL,"
                    "xuehao VARCHAR(255) NOT NULL,"
                    "borrow_time VARCHAR(255) NOT NULL,"//借阅日期
                    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"//截止日期
                    "is_online TINYINT DEFAULT 0)");

    QSqlQuery createTableQuery(db);  // 声明一个QSqlQuery对象
    if (!createTableQuery.exec(sql)) {
        qDebug() << "Create borrow table error:" << createTableQuery.lastError();
        return;
    } else {
        qDebug() << "Create borrow table success.";
    }
}

void borrowbook::loadBooks(){
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
        int current_stock=query.value("current_stock").toInt();

        // Insert row into the table
        ui->booksWidget->insertRow(row);
        ui->booksWidget->setItem(row, 0, new QTableWidgetItem(shuhao));
        ui->booksWidget->setItem(row, 1, new QTableWidgetItem(bookname));
        ui->booksWidget->setItem(row, 2, new QTableWidgetItem(writer));
        ui->booksWidget->setItem(row, 3, new QTableWidgetItem(publisher));
        ui->booksWidget->setItem(row, 4, new QTableWidgetItem(QString::number(current_stock)));

        ++row;
    }
}

borrowbook::~borrowbook()
{
    delete ui;
}

void borrowbook::on_backButton_2_clicked()
{
    emit returntomode();
    this->hide();
}

void borrowbook::on_refreshButton_clicked()
{
    loadBooks();
    qDebug()<<"refresh";
}

void borrowbook::on_searchButton_clicked()
{
    //根据名字查询
    QString sql = QString("select * from books where title =\"%1\";").arg(ui->bookname->text());
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
    ui->booksWidget->setRowCount(id); // Clear existing rows
    QString shuhao = query.value("book_id").toString();
    QString bookname=query.value("title").toString();
    QString writer = query.value("author").toString();
    QString publisher = query.value("publisher").toString();
    int current_stock=query.value("current_stock").toInt();

    // Insert row into the table
    ui->booksWidget->insertRow(id);
    ui->booksWidget->setItem(id, 0, new QTableWidgetItem(shuhao));
    ui->booksWidget->setItem(id, 1, new QTableWidgetItem(bookname));
    ui->booksWidget->setItem(id, 2, new QTableWidgetItem(writer));
    ui->booksWidget->setItem(id, 3, new QTableWidgetItem(publisher));
    ui->booksWidget->setItem(id, 4, new QTableWidgetItem(QString::number(current_stock)));
    }
    else {
        QMessageBox::warning(this, "Error", "查询无结果");
    }
}

void borrowbook::on_borrowButton_clicked()
{
    QString bookname=ui->bookname->text();//获取书名
    //根据名字查询
    QString sql = QString("select * from books where title =\"%1\";").arg(bookname);
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
    QString xuehao=ui->lineEdit->text();
    if(xuehao==NULL){
        QMessageBox::warning(this, "error", "请输入学号");
        return;
    }
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT * FROM borrow WHERE title = ? AND xuehao = ?");
    checkQuery.addBindValue(bookname);
    checkQuery.addBindValue(xuehao);
    if (!checkQuery.exec()) {
    }
    if(checkQuery.next()){
        QMessageBox::warning(this, "error", "已借阅该书");
        return;
    }
    if(query.next())
    {
    //获取数据库query所指的那行的数据
    ui->booksWidget->setRowCount(id); // Clear existing rows
    QString shuhao = query.value("book_id").toString();
    QString bookname=query.value("title").toString();
    QString writer = query.value("author").toString();
    QString publisher = query.value("publisher").toString();
    int current_stock=query.value("current_stock").toInt();
    if(current_stock>0){
        int newcurrent_stock = current_stock -1;
        QDateTime borrow_date=QDateTime::currentDateTime();
        QString borrowdate=borrow_date.toString("yyyy-MM-dd");
        ui->booksWidget->insertRow(id);
        ui->booksWidget->setItem(id, 0, new QTableWidgetItem(shuhao));
        ui->booksWidget->setItem(id, 1, new QTableWidgetItem(bookname));
        ui->booksWidget->setItem(id, 2, new QTableWidgetItem(writer));
        ui->booksWidget->setItem(id, 3, new QTableWidgetItem(publisher));
        ui->booksWidget->setItem(id, 4, new QTableWidgetItem(QString::number(newcurrent_stock)));
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE books SET current_stock = ? WHERE title = ?");
        updateQuery.addBindValue(newcurrent_stock);
        updateQuery.addBindValue(bookname);
        updateQuery.exec();
        QSqlQuery insertquery(db);//创建对象
        QString sql=QString("INSERT INTO borrow(book_id,title,xuehao,borrow_time) VALUES(?,?,?,?)");
        insertquery.prepare(sql);//准备执行插入
        insertquery.addBindValue(shuhao);//插入书号
        insertquery.addBindValue(bookname);//插入书名
        insertquery.addBindValue(xuehao);//插入学号
        insertquery.addBindValue(borrowdate);//插入借书时间

        if(!insertquery.exec()){
            qDebug() << "Insert error:" << insertquery.lastError();
            QMessageBox::warning(this,"error","借阅记录入库失败！");
            return;
        }else{
            QMessageBox::information(this, "success", "借阅书籍成功！");
        }
    }
        else {
        QMessageBox::warning(this, "error", "库存不足，借阅书籍失败！");
        }
    }
    else {
        QMessageBox::warning(this, "error", "无该书，借阅书籍失败！");
    }
}
