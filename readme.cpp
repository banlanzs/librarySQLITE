#include "readme.h"
#include "ui_readme.h"
#include<QTimer>
readme::readme(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::readme)
{
    ui->setupUi(this);
    ui->plainTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);
}

readme::~readme()
{
    delete ui;
}

void readme::on_confirmButton_clicked()
{
    this->hide();
}
