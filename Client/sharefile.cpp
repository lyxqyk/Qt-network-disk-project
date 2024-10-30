#include "sharefile.h"
#include "ui_sharefile.h"
#include "index.h"
#include "client.h"
#include "file.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    ui->listWidget->clear();
    QListWidget * friend_LW = Index::getInstance().getFriend()->getFriend_LW();
    for(int i=0;i<friend_LW->count();i++){
        QListWidgetItem * friendItem = friend_LW->item(i);
        QListWidgetItem *newItem = new QListWidgetItem(*friendItem);
        ui->listWidget->addItem(newItem);
    }
}

void ShareFile::on_allSelect_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelect_PB_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++){
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    qDebug()<<"on_ok_PB_clicked start";
    //获取当前路径和当前用户名
    QString strCurPath = Index::getInstance().getFile()->m_strCurPath;
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strPath = strCurPath + '/' + Index::getInstance().getFile()->m_strShareFile;//路径
//    qDebug()<<"strCurPath" << strCurPath<< "strCurName" << strCurName<< "strPath" << strPath;

    //获取选择的好友
    QList<QListWidgetItem*> pItem = ui->listWidget->selectedItems();
    int friendNum = pItem.size();

    //构建PUD，当前用户名和好友数量放入cadata 选择的好友名和路径放入camsg
    PDU * pdu = mkPDU(friendNum*32+strPath.toStdString().size()+1);
    pdu->uiMsgtype =ENUM_MSG_TYPE_SHARE_FILE_REQUEST;

    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&friendNum,sizeof(int));
    qDebug()<<" pdu->caData"<<pdu->caData<<"pdu->caData+32"<<pdu->caData+32;
    for(int i=0;i<friendNum;i++){
        memcpy(pdu->caMsg+i*32,pItem.at(i)->text().toStdString().c_str(),32);
        qDebug()<<"pdu->caMsg+i*32"<<pdu->caMsg+i*32;
    }

    memcpy(pdu->caMsg+friendNum*32,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}
