#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer&getInstance();
    void incomingConnection(qintptr handle);//有客户端连接会调用该函数，返回客户端信息
    void resend(char * tarName,PDU * pdu);

public slots:
    //移除下线客户端的socket的槽函数
    void deleteSocket(MyTcpSocket*mysocket);

private:
    MyTcpServer();
    QList<MyTcpSocket*>m_tcpSocketList;
};

#endif // MYTCPSERVER_H
