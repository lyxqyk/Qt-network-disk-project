#include "mytcpsocket.h"
#include "protocol.h"
#include "operatedb.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
    connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::clientOffline);
    m_pmh = new MsgHandler;//需要释放
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_pmh;
}

void MyTcpSocket::recvMsg()
{
    qDebug()<<"recvMsg 接收消息长度："<<this->bytesAvailable();    //解决粘包，半包问题
    QByteArray data = readAll();
    buffer.append(data);//buffer--当前所有数据
    qDebug()<<"buffer.size()"<< buffer.size()<<"sizeof(PDU)"<<sizeof(PDU);
    while(buffer.size()>=int(sizeof(PDU))){//判断是否是一个完整的PDU，为了能取PDU中的协议长度
        PDU *pdu =(PDU*)buffer.data();
        if(buffer.size()<int(pdu->uiPDULen)){//判断能否构成一个完整的协议请求（PDU+柔性数组）
            qDebug()<<"半包";
            break;
        }
        PDU * respdu = handleMsg(pdu);//读取消息
        sendPDU(respdu);//发送结果
       buffer.remove(0,pdu->uiPDULen);//释放
    }
}

//PDU *MyTcpSocket::readPDU()
//{
//    //读协议长度
//    unit uiPDULen = 0;
//    this->read((char*)&uiPDULen,sizeof(unit));//总的协议长度
//    //读取协议长度以外的数据
//    unit uiMsgLen =uiPDULen - sizeof(unit);
//    PDU *pdu = mkPDU(uiMsgLen);
//    this->read((char*)pdu + sizeof(unit), uiPDULen - sizeof(unit));//读取协议总长度以外的数据
//    qDebug()<<"readPDU 消息类型"<<pdu->uiMsgtype
//            <<" 消息内容："<<pdu->caMsg
//            <<" 参数1："<<pdu->caData
//            <<" 参数2："<<pdu->caData+32;
//    return pdu;
//}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    qDebug()<<"\n\n\nhandleMsg 消息类型"<<pdu->uiMsgtype
               <<" 消息内容："<<pdu->caMsg
               <<" 参数1："<<pdu->caData
               <<" 参数2："<<pdu->caData+32;
    //根据消息类型进行处理
    switch(pdu->uiMsgtype){
    case ENUM_MSG_TYPE_REGIST_REQUEST:
        return m_pmh->regist(pdu);
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
        return m_pmh->login(pdu,m_strLoginName);
    case ENUM_MSG_TYPE_FIND_USER_REQUEST:
        return m_pmh->findUser(pdu);
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:
        return m_pmh->onlineUser();
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        return m_pmh->addFriend(pdu);
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST:
        return m_pmh->addFriendAgree(pdu);
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
        return m_pmh->flushFriend(pdu);
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        return m_pmh->deleteFriend(pdu);
    case ENUM_MSG_TYPE_CHAT_REQUEST:
        m_pmh->chat(pdu);
        break;
    case ENUM_MSG_TYPE_MKDIR_REQUEST:
        return m_pmh->mkdir(pdu);
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        return m_pmh->flushFile(pdu);
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        return m_pmh->delDir(pdu);
    case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
        return m_pmh->delFile(pdu);
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        return m_pmh->renameFile(pdu);
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        return m_pmh->moveFile(pdu);
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        return m_pmh->uploadFile(pdu);
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:
        return m_pmh->uploadFileData(pdu);
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        return m_pmh->shareFile(pdu);
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:
        return m_pmh->shareFileAgree(pdu);
    default:
        qDebug()<<"未处理的消息类型"<<pdu->uiMsgtype;
        break;
    }
    return NULL;
}

void MyTcpSocket::sendPDU(PDU *pdu)
{
    if(pdu == NULL){
        return;
    }
    write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void MyTcpSocket::clientOffline()
{
    OperateDb::getInstance().handleOffline(m_strLoginName.toStdString().c_str());
    emit offline(this); //发出信号
}






