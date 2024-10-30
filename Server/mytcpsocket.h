#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <qtcpsocket.h>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    QString m_strLoginName;
//    PDU * readPDU();
    PDU * handleMsg(PDU * pdu);
    void sendPDU(PDU * pdu);
    MsgHandler * m_pmh;//实例调用处理读取消息中的各个功能函数
    QByteArray buffer;//当前所有数据

public slots:
    void recvMsg();//接收并处理消息
    void clientOffline();//处理下线



signals:
    //通知服务器移除socket的信号
    void offline(MyTcpSocket *mysocked);
};

#endif // MYTCPSOCKET_H
