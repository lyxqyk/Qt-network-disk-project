#include "client.h"
#include "ui_client.h"
#include "protocol.h"
#include "index.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    m_prh = new ResHandler;
    loadConfig();//加载配置文件
    //关联成功的信号和处理信号的槽函数
    connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::showConnect);
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::recvMsg);
    qDebug()<<"打开用户配置文件 ip:"<<m_strIp<<"port:"<<m_usProt;
    //连接到服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIp),m_usProt);//尝试连接到服务器，使用从配置文件加载的IP地址和端口号。

}

Client::~Client()
{
    delete m_prh ;
    delete ui;
}

void Client::showConnect()
{
    qDebug()<<"连接服务器成功";
}

void Client::loadConfig()
{
    //新建一个QFile对象
    QFile file(":/client.config");//固定冒号开头 /是前缀  创建file对象，利用file读取数据
    //只读模式打开文件
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");//读到的内容按照\r\n进行拆分返回一个列表
        m_strIp = strList.at(0);//ip
        m_usProt = strList.at(1).toUShort();//端口号 字符串转成16位短整型
        m_strRootPath = strList.at(2);
        qDebug()<<"打开用户配置文件 ip:"<<m_strIp<<"port:"<<m_usProt<<"RootPath"<<m_strRootPath;
    }else{
    qDebug()<<"打开配置失败";
    }
}

Client &Client::getInstance()
{
    //建立单例模式
    static Client instance;//静态局部变量在C++11后是线程安全的
    return instance;
}

QTcpSocket &Client::getTcpSocket()
{
    return m_tcpSocket;
}

QString Client::getRootPath()
{
    return m_strRootPath;
}

void Client::sendPDU(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//发送pdu
    free(pdu);
    pdu = NULL;
}

void Client::on_regist_PB_clicked()
{
    qDebug()<<"on_regist_PB_clicked start";
    QString strName = ui->username_LB->text();
    QString strPwd = ui->password_LB->text();
    qDebug()<<"strName:"<<strName
            <<"strPwd:"<<strPwd;
    if(strName.isEmpty()||strPwd.isNull()||strName.size()>32||strPwd.size()>32){
        QMessageBox::critical(this,"注册","用户名或密码非法");
        return;
    }
    PDU*pdu = mkPDU(0);//没使用柔性数组
    pdu->uiMsgtype = ENUM_MSG_TYPE_REGIST_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//注意指针偏移
    sendPDU(pdu);
}

void Client::on_login_PB_clicked()
{
    qDebug()<<"on_login_PB_clicked start";
    QString strName = ui->username_LB->text();
    QString strPwd = ui->password_LB->text();
    qDebug()<<"strName:"<<strName
            <<"strPwd:"<<strPwd;
    if(strName.isEmpty()||strPwd.isNull()||strName.size()>32||strPwd.size()>32){
        QMessageBox::critical(this,"注册","用户名或密码非法");
        return;
    }
    PDU*pdu = mkPDU(0);//没使用柔性数组
    pdu->uiMsgtype = ENUM_MSG_TYPE_LOGIN_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//注意指针偏移
    m_strLoginName = strName;//存登录名
    sendPDU(pdu);
}

void Client::recvMsg()
{
     qDebug()<<"recvMsg 接收消息长度："<<m_tcpSocket.bytesAvailable();
    //解决粘包，半包问题
    QByteArray data = m_tcpSocket.readAll();
    buffer.append(data);//buffer--当前所有数据

    while(buffer.size()>=int(sizeof(PDU))){//判断是否是一个完整的PDU，为了能取PDU中的协议长度
        PDU *pdu =(PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){//判断能否构成一个完整的协议请求（PDU+柔性数组）
            break;
        }
        handleMsg(pdu);//读取消息
        buffer.remove(0,pdu->uiPDULen);//释放
    }
}

//PDU *Client::readPDU()
//{
//    qDebug()<<"\n\n\nreadPDU 接收消息长度："<<m_tcpSocket.bytesAvailable();
//    //读协议长度
//    unit uiPDULen = 0;
//    m_tcpSocket.read((char*)&uiPDULen,sizeof(unit));//总的协议长度

//    //读取协议长度以外的数据
//    unit uiMsgLen =uiPDULen - sizeof(unit);
//    PDU *pdu = mkPDU(uiMsgLen);
//    m_tcpSocket.read((char*)pdu + sizeof(unit), uiPDULen - sizeof(unit));//读取协议总长度以外的数据
//    return pdu;
//}

void Client::handleMsg(PDU *pdu)
{
    qDebug()<<"\n\n\nsendPDU caData:"<<pdu->caData
            <<"caData+32:"<<pdu->caData+32
            <<"uiMsgtype:"<<pdu->uiMsgtype
            <<"caMsg:"<<pdu->caMsg;
    //根据消息类型进行处理
    switch(pdu->uiMsgtype){
    case ENUM_MSG_TYPE_REGIST_RESPEND:
        m_prh->regist(pdu);
        break;
    case ENUM_MSG_TYPE_LOGIN_RESPEND:
        m_prh->login(pdu);
        break;
    case ENUM_MSG_TYPE_FIND_USER_RESPEND:
        m_prh->findUser(pdu);
        break;
    case ENUM_MSG_TYPE_ONLINE_USER_RESPEND:
        m_prh->onlineUser(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPEND:
        m_prh->addFriend(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        m_prh->addFriendRequest(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPEND:
        m_prh->addFriendAgree();
        break;
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPEND:
        m_prh->flushFriend(pdu);
        break;
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND:
        m_prh->deleteFriend(pdu);
        break;
    case ENUM_MSG_TYPE_CHAT_REQUEST:
        m_prh->chat(pdu);
        break;
    case ENUM_MSG_TYPE_MKDIR_RESPEND:
        m_prh->mkdir(pdu);
        break;
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPEND:
        m_prh->flushFile(pdu);
        break;
    case ENUM_MSG_TYPE_DEL_DIR_RESPEND:
        m_prh->delDir(pdu);
        break;
    case ENUM_MSG_TYPE_DEL_FILE_RESPEND:
        m_prh->delFile(pdu);
        break;
    case ENUM_MSG_TYPE_RENAME_FILE_RESPEND:
        m_prh->renameFile(pdu);
    case ENUM_MSG_TYPE_MOVE_FILE_RESPEND:
        m_prh->moveFile(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND:
        m_prh->uploadFile(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND:
        m_prh->uploaxdFileData(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_RESPEND:
        m_prh->shareFile(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        m_prh->shareFileRequest(pdu);
        break;
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND:
        m_prh->shareFileAgree(pdu);
        break;
    default:
        qDebug()<<"未处理的消息类型"<<pdu->uiMsgtype;
        break;
    }
}



//void Client::on_pushButton_clicked()
//{
//    QString strMsg =ui->input_LE->text();
//    qDebug()<<"on_pushButton_clicked strMsg="<<strMsg;
//    if(strMsg.isEmpty()){
//        QMessageBox::warning(this,"发送消息","发送消息不能为空");
//        return;
//    }
//    PDU*pdu=mkPDU(strMsg.size());//构建pdu
//    memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
//    pdu->uiMsgtype = ENUM_MSG_TYPE_REGIST_REQUEST;
//    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);//发送pdu
//    free(pdu);
//    pdu = NULL;
//}

