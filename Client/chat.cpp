#include "chat.h"
#include "protocol.h"
#include "ui_chat.h"
#include "client.h"

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::updateShow_LE(QString strMsg)
{
    ui->show_TE->append(strMsg);
}



void Chat::on_send_PB_clicked()
{
    //获取发送的消息
    QString strMsg = ui->input_LE->text();
    if(strMsg.isEmpty()){
        return;
    }
    PDU * pdu = mkPDU(sizeof(strMsg));
    pdu->uiMsgtype = ENUM_MSG_TYPE_CHAT_REQUEST;
    //发送的消息放入caMsg
    memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.toStdString().size());
    //发送者名和发送目标放入caDate]
    memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),32);
    Client::getInstance().sendPDU(pdu);
    ui->input_LE->clear();//输入后清空输入框
}
