//zhoushuang
#include "bookin.h"
#include "ui_bookin.h"
#include"DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include<QRegularExpression>
#include<QSqlDatabase>
#include<QTimer>
BookIn::BookIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookIn)
{
    ui->setupUi(this);
    initDatabase();//初始化
    // 连接信号和槽函数
    setupConnections();
}

BookIn::~BookIn()
{
    delete ui;
}

//检查书号格式
void BookIn::setupConnections(){
    // 连接编辑完成信号到槽函数
        //connect(ui->Shuhao, &QLineEdit::editingFinished, this, &BookIn::checkShuhaoFormat);
    connect(ui->BookName, &QLineEdit::editingFinished, this, &BookIn::checkBookExists);
}
void BookIn::checkBookExists() {
    QString bookname = ui->BookName->text();
    if (bookname.isEmpty()) {
        return;
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT author, publisher, total_stock, current_stock FROM books WHERE title = ?");
    checkQuery.addBindValue(bookname);

    if (!checkQuery.exec()) {
        qDebug() << "Check error:" << checkQuery.lastError();
        QMessageBox::warning(this, "error", "检查失败！");
        return;
    }

    if (checkQuery.next()) {
        QString author = checkQuery.value(0).toString();
        QString publisher = checkQuery.value(1).toString();
        int total_stock = checkQuery.value(2).toInt();
        int current_stock = checkQuery.value(3).toInt();

        ui->Writer->setText(author);
        ui->Publisher->setText(publisher);
        ui->TotalBookNumber->display(total_stock);
        ui->CurrentBookNumber->display(current_stock);

        ui->Writer->setDisabled(true);
        ui->Publisher->setDisabled(true);
    } else {
        ui->Writer->clear();
        ui->Publisher->clear();
        ui->TotalBookNumber->display(0);
        ui->CurrentBookNumber->display(0);

        ui->Writer->setDisabled(false);
        ui->Publisher->setDisabled(false);
    }
}
//void BookIn::checkShuhaoFormat() {
//    QString shuhao = ui->Shuhao->text();
//    QString pattern = "^[0-9]{4}-[0-9]{4}$";
//    QRegularExpression re(pattern);

//    if (!re.match(shuhao).hasMatch()) {
//        QMessageBox::warning(this, "错误", "书号格式应为0000-0001！");
//        ui->Shuhao->setFocus(); // 将焦点重新设置回书号输入框
//    }
//}
void BookIn::initDatabase()
{
    // 使用DatabaseManager获取数据库连接
    db = DatabaseManager::getDatabase();
    // 验证是否已经连接
    if (!db.open()) {
        qDebug() << db.lastError();
        //QMessageBox::warning(this, "error", "BookIn未连接到数据库");
        qDebug()<<"BookIn未连接到数据库"<<endl;
        return;
    } else {
        //QMessageBox::information(this, "success", "BookIn连接到数据库");
        qDebug()<<"BookIn连接到数据库"<<endl;
    }

    // 构建books表
    QString sql("CREATE TABLE IF NOT EXISTS books ("
                "book_id TEXT NOT NULL PRIMARY KEY,"//书号
                "title TEXT NOT NULL,"//书名
                "author TEXT NOT NULL,"//作者
                "publisher TEXT NOT NULL,"//出版社
                "total_stock INTEGER DEFAULT 0 NOT NULL,"//总库存
                "current_stock INTEGER DEFAULT 0 NOT NULL)");//当前库存

//    QString sql("CREATE TABLE IF NOT EXISTS books ("
//                    "book_id VARCHAR(255) NOT NULL PRIMARY KEY," // 书号
//                    "title VARCHAR(255) NOT NULL," // 书名
//                    "author VARCHAR(255) NOT NULL," // 作者
//                    "publisher VARCHAR(255) NOT NULL," // 出版社
//                    "total_stock INT DEFAULT 0 NOT NULL," // 总库存
//                    "current_stock INT DEFAULT 0 NOT NULL)"); // 当前库存

    QSqlQuery createTableQuery(db);  // 声明一个QSqlQuery对象
    if (!createTableQuery.exec(sql)) {
        qDebug() << "Create books table error:" << createTableQuery.lastError();
        return;
    } else {
        qDebug() << "Create books table success.";
    }
}

//书籍入库
void BookIn::saveToDatabase(){
     //QString shuhao=ui->Shuhao->text();//获取书号
     QString bookname=ui->BookName->text();//获取书名
     QString writer=ui->Writer->text();//获取作者
     QString publisher=ui->Publisher->text();//获取出版社
     int total_stock=ui->TotalBookNumber->intValue();//总库存
     int addnum=ui->BookInspinBox->value();//添加数量
     int current_stock=ui->CurrentBookNumber->intValue();//当前库存

     //输入不为空
     if ( bookname.isEmpty() || writer.isEmpty() || publisher.isEmpty()) {
             QMessageBox::warning(this, "error", "所有字段不能为空！");
             return;
         }

     // 检查书号、作者、出版社是否存在
         QSqlQuery checkQuery(db);
         checkQuery.prepare("SELECT book_id, total_stock, current_stock FROM books WHERE title = ? AND author = ? AND publisher = ?");
         checkQuery.addBindValue(bookname);
         checkQuery.addBindValue(writer);
         checkQuery.addBindValue(publisher);

         if (!checkQuery.exec()) {
             qDebug() << "Check error:" << checkQuery.lastError();
             QMessageBox::warning(this, "error", "检查失败！");
             return;
         }

         if (checkQuery.next()) {
             // 如果记录已存在，更新总库存和当前库存
             QString shuhao = checkQuery.value(0).toString();
             int existing_total_stock = checkQuery.value(1).toInt();
             int existing_current_stock = checkQuery.value(2).toInt();
             total_stock=existing_total_stock+addnum;
             current_stock = existing_current_stock + addnum;

             QSqlQuery updateQuery(db);
             updateQuery.prepare("UPDATE books SET total_stock = ?, current_stock = ? WHERE book_id = ?");
//             updateQuery.addBindValue(bookname);
             updateQuery.addBindValue(total_stock);
             updateQuery.addBindValue(current_stock);
             updateQuery.addBindValue(shuhao);
//             updateQuery.addBindValue(writer);
//             updateQuery.addBindValue(publisher);

             if (!updateQuery.exec()) {
                 qDebug() << "Update error:" << updateQuery.lastError();
                 QMessageBox::warning(this, "error", "更新书籍信息失败！");
                 return;
             } else {
                 QMessageBox::information(this, "success", "更新书库成功！");
                 ui->TotalBookNumber->display(total_stock);//更新总库存显示
                 ui->CurrentBookNumber->display(current_stock);//更新当前库存
             }
         }else{
     //插入xin信息(没有记录)
             // 获取当前最大书号
                     QSqlQuery maxIdQuery(db);
                     maxIdQuery.prepare("SELECT MAX(book_id) FROM books");
                     if (!maxIdQuery.exec() || !maxIdQuery.next()) {
                         qDebug() << "Max ID query error:" << maxIdQuery.lastError();
                         QMessageBox::warning(this, "error", "获取最大书号失败！");
                         return;
                     }

                     QString maxId = maxIdQuery.value(0).toString();
                     QStringList parts = maxId.split("-");
                     int newIdPart1 = parts[0].toInt();
                     int newIdPart2 = parts[1].toInt() + 1;
                     QString newId = QString("%1-%2").arg(newIdPart1, 4, 10, QChar('0')).arg(newIdPart2, 4, 10, QChar('0'));

     current_stock = addnum;
     QSqlQuery insertquery(db);//创建对象
     QString sql=QString("INSERT INTO books(book_id,title,author,publisher,total_stock,current_stock)VALUES(?,?,?,?,?,?)");
     insertquery.prepare(sql);//准备执行插入
     insertquery.addBindValue(newId);//插入书号
     insertquery.addBindValue(bookname);//插入书名
     insertquery.addBindValue(writer);//插入作者
     insertquery.addBindValue(publisher);//插入出版社
     insertquery.addBindValue(addnum);//插入总库存
     insertquery.addBindValue(current_stock);//插入当前库存

     if(!insertquery.exec()){
         qDebug() << "Insert error:" << insertquery.lastError();
         QMessageBox::warning(this,"error","书籍入库失败！");
         return;
     }else{
         QMessageBox::information(this,"sucess","书籍入库成功！");
         ui->TotalBookNumber->display(addnum);//更新总库存显示
         ui->CurrentBookNumber->display(current_stock);//更新当前库存
     }
   }
}

//返回
void BookIn::on_BackButton_clicked()
{
    ui->BookName->clear();
    ui->Writer->clear();
    ui->Publisher->clear();
    ui->BookInspinBox->setValue(0);
    ui->CurrentBookNumber->display(0);
    ui->TotalBookNumber->display(0);
    emit returntomode();  // Emit the signal to return to the Mode window
    this->hide();  // Hide the BookIn window
}

//确认书籍入库
void BookIn::on_OKpushButton_clicked()
{
    saveToDatabase();
}
