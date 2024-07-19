#ifndef MANAGEUSERS_H
#define MANAGEUSERS_H

#include <QDialog>
#include<QSqlDatabase>
#include"emailyanshi.h"
#include<QTimer>
namespace Ui {
class ManageUsers;
}

class ManageUsers : public QDialog
{
    Q_OBJECT

public:
    explicit ManageUsers(QWidget *parent = nullptr);
    ~ManageUsers();
signals:
    void returntomode();//->closeButton

private slots:
    void on_refreshButton_clicked();
    void on_closeButton_clicked();

    void on_remindreturnButton_clicked();
    void onUserSelectionChanged();
    void updateButtonText();

private:
    Ui::ManageUsers *ui;
    emailyanshi*emailWidget;
    QSqlDatabase db;
    QTimer *m_pTimer; // 添加QTimer成员变量
        int m_nCountdown; // 倒计时变量
        bool allowSignal;
        void initDatabase();
        void loadUsers();//导入用户信息
        void loadborrow(const QString& xuehao);//导入借阅信息
};

#endif // MANAGEUSERS_H
