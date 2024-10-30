#include "server.h"
#include "ui_server.h"
#include "mytcpserver.h"

#include <QDebug>
#include <qfile.h>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadConfig();
//    qDebug()<<"打开用户配置文件 ip:"<<m_strIp<<"port:"<<m_usProt;
    MyTcpServer::getInstance().listen(QHostAddress(m_strIp),m_usProt);//监听
}

void Server::loadConfig()
{
    //新建一个QFile对象
    QFile file(":/server.config");//固定冒号开头 /是前缀  创建file对象，利用file读取数据
    //只读模式打开文件
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");//读到的内容按照\r\n进行拆分返回一个列表
        m_strIp = strList.at(0);//ip
        m_usProt = strList.at(1).toUShort();//端口号 字符串转成16位短整型
        m_strRootPath = strList.at(2);
        qDebug()<<"打开用户配置文件 ip:"<<m_strIp<<"port:"<<m_usProt<<"RootPath"<<m_strRootPath;
        file.close();
    }else{
    qDebug()<<"打开配置失败";
    }
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

QString Server::getRootPath()
{
    return m_strRootPath;
}

Server::~Server()
{
    delete ui;
}


