#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::showOnlineUser(QStringList userList)
{
    ui->listWidget->clear();//不清空的话下线不会删除
    ui->listWidget->addItems(userList);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug()<<"on_listWidget_itemDoubleClicked start";
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    qDebug()<<"strCurName"<<strCurName
           <<"strTarName"<<strTarName;
    PDU * pdu = mkPDU(0);
    pdu->uiMsgtype = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    Client::getInstance().sendPDU(pdu);
}
