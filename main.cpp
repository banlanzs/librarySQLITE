#include "mainwindow.h"
#include"emailyanshi.h"
#include"DatabaseManager.h"
#include"readme.h"
#include <QApplication>
#include <QSqlDatabase>     //连接数据库
#include <QDebug>
#include <QSqlError>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    readme r;

    w.show();
    r.show();
//    emailyanshi*e=new emailyanshi();
//    e->show();
    //return a.exec();
    int result = a.exec();

//        // 关闭数据库连接
//        QSqlDatabase::database("library_connection").close();
//        QSqlDatabase::removeDatabase("library_connection");

    DatabaseManager::closeDatabase();
        return result;
}
//QSqlDatabasePrivate::removeDatabase: connection 'library_connection' is still in use, all queries will cease to work.
//QSqlDatabasePrivate::addDatabase: duplicate connection name 'library_connection', old connection removed.
//Create table sucess.
//问题在于在多个地方创建和使用了数据库连接，导致重复添加数据库连接。这个问题可以通过确保每个窗口或类使用同一个连接来解决。
