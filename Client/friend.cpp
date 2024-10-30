#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"
#include "onlineuser.h"

#include <QInputDialog>
#include <QDebug>
#include <qmessagebox.h>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
    flushFriend();
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;
    delete m_pChat;
}

OnlineUser *Friend::getOnlineUser()
{
    return m_pOnlineUser;
}

void Friend::showOnlineFriend(QStringList friendList)
{
    ui->listWidget->clear();//不清空的话下线不会删除
    ui->listWidget->addItems(friendList);
}

void Friend::flushFriend()
{
    QString strName = Client::getInstance().m_strLoginName;
    PDU * pdu = mkPDU(0);
    memcpy(pdu->caData,strName.toStdString().c_str(),sizeof(strName));
    pdu->uiMsgtype=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    Client::getInstance().sendPDU(pdu);
}

Chat * Friend::getChat()
{
    return m_pChat;
}

QListWidget *Friend::getFriend_LW()
{
    return ui->listWidget;
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this,"搜索","用户名：");
    qDebug()<<"on_findUser_PB_clicked strName:"<<strName;
    if(strName.isNull())
    {
        return;
    }
    PDU*pdu = mkPDU(0);//没使用柔性数组
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    pdu->uiMsgtype = ENUM_MSG_TYPE_FIND_USER_REQUEST;
//    定义公有方法获取私有成员
    Client::getInstance().sendPDU(pdu);
}

void Friend::on_online_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
    }
    PDU * pdu = mkPDU(0);
    pdu->uiMsgtype=ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
    Client::getInstance().sendPDU(pdu);
}

void Friend::on_flushFriend_PB_clicked()
{
    flushFriend();
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem * pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"删除好友","请选择想要删除的好友");
        return;
    }
    QString strTarName = pItem->text();
    int ret =QMessageBox::question(this,"删除好友",QString("是否确定删除好友: %1").arg(strTarName));
    if(ret != QMessageBox::Yes){
       return;
    }
    PDU *pdu = mkPDU(0);
    pdu->uiMsgtype=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    QString strCurName = Client::getInstance().m_strLoginName;
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    Client::getInstance().sendPDU(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem * pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"聊天","请选择想要聊天的好友");
        return;
    }
    if(m_pChat->isHidden()){
        m_pChat->show();
    }
    m_pChat->m_strChatName = pItem->text();//好友名
}
