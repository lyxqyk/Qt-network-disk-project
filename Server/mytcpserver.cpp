#include "mytcpserver.h"

#include <QDebug>

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)//参数就是用来获取客户端socket
{
    qDebug()<<"新客户端连接";
    //将连接的客户端socket存入socket列表
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);//新建的socket对应的就是接收到的客户端socked
    m_tcpSocketList.append(pTcpSocket);
//    //打印接收到的客户端
//    foreach (MyTcpSocket *pTcpSocket,m_tcpSocketList){
//        qDebug() <<pTcpSocket;
//    }

    //连接退出登录信号和槽--因为服务器连接之后才知道是哪一个socket
    connect(pTcpSocket,&MyTcpSocket::offline,this,&MyTcpServer::deleteSocket);
}

void MyTcpServer::resend(char *tarName, PDU *pdu)
{
    if(tarName == NULL || pdu == NULL){
        return;
    }
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(tarName == m_tcpSocketList.at(i)->m_strLoginName){
            m_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    m_tcpSocketList.removeOne(mysocket);//从socket列表移除
    mysocket->deleteLater();//延迟移除  自己消灭自己
    mysocket = NULL;
    //测试是否成功移除
    qDebug()<<m_tcpSocketList.size();
    for(int i=0;i<m_tcpSocketList.size();i++)
    {
        qDebug()<<m_tcpSocketList.at(i)->m_strLoginName;
    }
//    //增强for循环
//    foreach(MyTcpSocket * pSocket,m_tcpSocketList)//将每一个m_tcpSocketList中的元素
//    {
//        qDebug()<<pSocket->m_strLoginName;
//    }

}

MyTcpServer::MyTcpServer()
{

}
