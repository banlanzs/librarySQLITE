#ifndef USERMODE_H
#define USERMODE_H

#include <QDialog>
#include"borrowbook.h"
#include"returnbook.h"
#include"reservation.h"
#include"managebooks.h"
namespace Ui {
class UserMode;
}

class UserMode : public QDialog
{
    Q_OBJECT

public:
    explicit UserMode(QString xuehao,QWidget *parent = nullptr);
    ~UserMode();

private slots:
    void on_backButton_clicked();

    void on_borrowButton_clicked();
    void on_reservationButton_clicked();
    void on_returnBookButton_clicked();

    void on_returnFromborrowbook();  // 槽函数处理从"借书"返回
    void on_returnFromreturnbook();//从“还书”返回
    void on_returnFromreservation();//从“预约”返回

private:
    Ui::UserMode *ui;
    QString m_xuehao;
    borrowbook*borrowbookWindow;
    returnbook*returnbookWindow;
    reservation*reservationWindow;
    managebooks*managebooksWindow;
    bool backButtonClicked; // 添加标志位
};

#endif // USERMODE_H
