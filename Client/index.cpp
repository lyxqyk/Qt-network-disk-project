#include "index.h"
#include "ui_index.h"

Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
}

Index::~Index()
{
    delete ui;
}

Friend *Index::getFriend()
{
    return ui->friendPage;
}

File *Index::getFile()
{
    return ui->filePage;
}

Index &Index::getInstance()
{
    static Index instance;
    return instance;
}

void Index::on_friend_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->friend_PB->setStyleSheet("QPushButton{border:none;background-color:rgb(255,255,255);padding:20px;}");
    ui->file_PB->setStyleSheet("QPushButton{border:none;background-color:rgba(255,255,255,0);padding:20px;}");
}

void Index::on_file_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->friend_PB->setStyleSheet("QPushButton{border:none;background-color:rgba(255,255,255,0);padding:20px;}");
    ui->file_PB->setStyleSheet("QPushButton{border:none;background-color:rgb(255,255,255);padding:20px;}");
}



