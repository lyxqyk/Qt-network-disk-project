#include "msghandler.h"
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"

#include <QDebug>
#include <QDir>

MsgHandler::MsgHandler()
{
    m_bUpload= false;
}

PDU *MsgHandler::regist(PDU *pdu)
{
    //处理注册
    qDebug()<<"ENUM_MSG_TYPE_REGIST_REQUEST star";
    //读取pdu中的用户名和密码
    char caName[32]={'\0'};//初始化字符串数组
    char caPwd[32]={'\0'};
    memcpy(caName,pdu->caData,32);
    memcpy(caPwd,pdu->caData+32,32);
     qDebug()<<"用户名："<<caName<<" 密码："<<caPwd;
    //数据库处理注册
    bool ret = OperateDb::getInstance().handleRegist(caName,caPwd);
    qDebug()<<"数据库处理结果 ret:"<<ret;
    //构建pdu并发送
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype =ENUM_MSG_TYPE_REGIST_RESPEND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::login(PDU *pdu, QString &strLoginName)
{
    //处理登录
    qDebug()<<"ENUM_MSG_TYPE_LOGIN_REQUEST star";
    //读取pdu中的用户名和密码
    char caName[32]={'\0'};//初始化字符串数组
    char caPwd[32]={'\0'};
    memcpy(caName,pdu->caData,32);
    memcpy(caPwd,pdu->caData+32,32);
     qDebug()<<"用户名："<<caName<<" 密码："<<caPwd;
    //数据库处理登录
    bool ret = OperateDb::getInstance().handleLogin(caName,caPwd);
    strLoginName = caName;
    qDebug()<<"数据库处理结果 ret:"<<ret;
    if(ret){
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(caName));
    }
    //构建pdu并发送
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype =ENUM_MSG_TYPE_LOGIN_RESPEND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::findUser(PDU *pdu)
{
    //处理用户查找
    qDebug()<<"ENUM_MSG_TYPE_FIND_USER_REQUEST start";
    //读取pdu中的要查找的用户名
    char caName[32]={'\0'};//初始化字符串数组
    memcpy(caName,pdu->caData,32);
    qDebug()<<"要查找的用户名："<<caName;
    //数据库处理
    int ret = OperateDb::getInstance().handleFindUser(caName);
    qDebug()<<"数据库处理结果 ret:"<<ret;
    //构建pdu并发送
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype =ENUM_MSG_TYPE_FIND_USER_RESPEND;
    memcpy(respdu->caData,caName,32);
    memcpy(respdu->caData+32,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    //在线用户
    qDebug()<<"ENUM_MSG_TYPE_ONLINE_USER_REQUEST start";
    //数据库处理
    QStringList ret = OperateDb::getInstance().handleOnlineUser();
    qDebug()<<"数据库处理结果 ret:"<<ret;
    //构建pdu并发送
    PDU * respdu = mkPDU(ret.size()*32);
    respdu->uiMsgtype =ENUM_MSG_TYPE_ONLINE_USER_RESPEND;
    for(int i=0;i<ret.size();i++)
    {
        qDebug()<<"ret.at(i)"<<ret.at(i);
        memcpy(respdu->caMsg+i*32,ret.at(i).toStdString().c_str(),32);
    }
    return respdu;
}

PDU *MsgHandler::addFriend(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_ADD_FRIEND_REQUEST start";
    char curName[32]={'\0'};
    char tarNmae[32]={'\0'};
    //读取pdu中当前用户和目标用户
    memcpy(curName,pdu->caData,32);
    memcpy(tarNmae,pdu->caData+32,32);
    int ret = OperateDb::getInstance().handleAddFriend(curName,tarNmae);
    qDebug()<<"数据库处理结果 ret:"<<ret;
    if(ret == 1){
        MyTcpServer::getInstance().resend(tarNmae,pdu);
    }
    //构建pdu并发送
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype =ENUM_MSG_TYPE_ADD_FRIEND_RESPEND;
    memcpy(respdu->caData,&ret,sizeof(int));
    return respdu;
}

PDU *MsgHandler::addFriendAgree(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST start";
    char curName[32]={'\0'};
    char tarNmae[32]={'\0'};
    //读取pdu中当前用户和目标用户
    memcpy(curName,pdu->caData,32);
    memcpy(tarNmae,pdu->caData+32,32);
    int ret = OperateDb::getInstance().handleAddFriendAgree(curName,tarNmae);
    PDU * respdu =mkPDU(0);
    respdu->uiMsgtype = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_RESPEND;
    memcpy(respdu->caData,&ret,sizeof(int));
    qDebug()<<"转发："<<curName;
    MyTcpServer::getInstance().resend(curName,respdu);
    return respdu;
}

PDU *MsgHandler::flushFriend(PDU *pdu)
{
    //刷新好友
    qDebug()<<"ENUM_MSG_TYPE_ADD_FLUSH_FRIEND_REQUEST start";
    QStringList ret = OperateDb::getInstance().handleFlushFriend(pdu->caData);
    qDebug()<<"数据库处理结果 ret:"<<ret;
    PDU * respdu = mkPDU(ret.size()*32);
    respdu->uiMsgtype =ENUM_MSG_TYPE_FLUSH_FRIEND_RESPEND;
    for(int i=0;i<ret.size();i++){
        qDebug()<<"ret.at(i)"<<ret.at(i);
        memcpy(respdu->caMsg+i*32,ret.at(i).toStdString().c_str(),32);
    }
    return respdu;
}

PDU *MsgHandler::deleteFriend(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST start";
    char curName[32]={'\0'};
    char tarNmae[32]={'\0'};
    //读取pdu中当前用户和目标用户
    memcpy(curName,pdu->caData,32);
    memcpy(tarNmae,pdu->caData+32,32);
    int ret = OperateDb::getInstance().handleDelFriend(curName,tarNmae);
    qDebug()<<"数据库处理结果 ret:"<<ret;
    //构建pdu并发送
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype =ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

void MsgHandler::chat(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_CHAT_REQUEST start";
    char tarName[32] ={'\0'};
    memcpy(tarName,pdu->caData+32,32);
    MyTcpServer::getInstance().resend(tarName,pdu);
}

PDU *MsgHandler::mkdir(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_MKDIR_REQUEST start";
    QString strCurPath = pdu->caMsg;
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype=ENUM_MSG_TYPE_MKDIR_RESPEND;
    bool res = false;
    QDir dir;
    // if -else -if
    if(!dir.exists(strCurPath)){// 检查当前路径是否存在
        memcpy(respdu->caData,&res,sizeof (bool));
        return respdu;
    }
    QString strNewPath = QString("%1/%2").arg(strCurPath).arg(pdu->caData);
    qDebug()<<"mkdir strNewPath"<<strNewPath;
    if(dir.exists(strNewPath)||!dir.mkdir(strNewPath)){//检查新路径是否已存在或尝试创建新路径
        memcpy(respdu->caData,&res,sizeof (bool));
        return respdu;
    }
    //成功创建
    res = true;
    memcpy(respdu->caData,&res,sizeof (bool));
    return respdu;
}

PDU *MsgHandler::flushFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_FLUSH_FILE_REQUEST start";
    QString strPath = pdu->caMsg;
    QDir dir(strPath);//传入路径
    QFileInfoList fileInfoList = dir.entryInfoList();//获取目录下的全部文件文件夹
    int iFileCount = fileInfoList.size();

//    qDebug()<<"iFileCount"<<iFileCount;
    PDU *respdu = mkPDU(sizeof(FileInfo)*(iFileCount-2));
    respdu->uiMsgtype = ENUM_MSG_TYPE_FLUSH_FILE_RESPEND;
    FileInfo * pFileInfo =NULL;//定义指针指向结构体
    QString strFileName;
    for(int i = 0,j=0;i<iFileCount;i++){
        strFileName = fileInfoList[i].fileName();
        if(strFileName == QString(".")||strFileName == QString("..")){
            continue;
        }
        pFileInfo = (FileInfo *)(respdu->caMsg)+j++;
        memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),32);
        if(fileInfoList[i].isDir()){
            pFileInfo->iFileType =0;//目录
        }else if(fileInfoList[i].isFile()){
            pFileInfo->iFileType =1;//文件
        }
        qDebug()<<"flushFIle strFileName"<<pFileInfo->caName
               <<"iFileType"<<pFileInfo->iFileType;
    }
//    qDebug()<<"respdu->uiMsg"<<respdu->uiMsgLen/sizeof(FileInfo);
    return respdu;
}

PDU *MsgHandler::delDir(PDU *pdu)
{
    QFileInfo fileInfo(pdu->caMsg);
    bool ret;
    if(fileInfo.isDir()){
        QDir dir(pdu->caMsg);
        ret = dir.removeRecursively();
    }
    PDU *respdu = mkPDU(0);
    respdu->uiMsgtype = ENUM_MSG_TYPE_DEL_DIR_RESPEND;
    memcpy(respdu->caData,&ret,sizeof (bool));
    return respdu;
}

PDU *MsgHandler::delFile(PDU *pdu)
{
    QFileInfo fileInfo(pdu->caMsg);
    bool ret;
    if(fileInfo.isFile()){
        QFile file(pdu->caMsg);
        ret = file.remove();
    }
    PDU *respdu = mkPDU(0);
    respdu->uiMsgtype = ENUM_MSG_TYPE_DEL_FILE_RESPEND;
    memcpy(respdu->caData,&ret,sizeof (bool));
    return respdu;
}

PDU *MsgHandler::renameFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_RENAME_FILE_REQUEST start";
    //获取原文件名和新文件名
    char caOldFileName[32] = {'\0'};
    char caNewFileName[32] = {'\0'};
    memcpy(caOldFileName,pdu->caData,32);
    memcpy(caNewFileName,pdu->caData+32,32);
    //取当前路径并拼接完整路径
    char * pPath = pdu->caMsg;
    QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldFileName);
    QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewFileName);
//    qDebug()<<"strOldPath"<<strOldPath;
//    qDebug()<<"strNewPath"<<strOldPath;
    //使用QDir对文件重命名
    QDir dir;
    bool res =dir.rename(strOldPath,strNewPath);
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype=ENUM_MSG_TYPE_RENAME_FILE_RESPEND;
    memcpy(respdu->caData,&res,sizeof (bool));
    return respdu;
}

PDU *MsgHandler::moveFile(PDU *pdu)
{
    int srcLen =0;
    int tarLen =0;

    memcpy(&srcLen,pdu->caData,sizeof (int));
    memcpy(&tarLen,pdu->caData+sizeof (int),sizeof (int));

    char * pSrcPath = new char[srcLen+1];
    char * pTarPath = new char[tarLen+1];

    memset(pSrcPath,'\0',srcLen+1);
    memset(pTarPath,'\0',tarLen+1);

    memcpy(pSrcPath,pdu->caMsg,srcLen);
    memcpy(pTarPath,pdu->caMsg+srcLen,tarLen);

    bool res = QFile::rename(pSrcPath,pTarPath);
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype=ENUM_MSG_TYPE_MOVE_FILE_RESPEND;
    memcpy(respdu->caData,&res,sizeof (bool));
    return respdu;

    delete [] pSrcPath;
    delete [] pTarPath;
    pSrcPath=NULL;
    pTarPath=NULL;
    return respdu;
}

PDU *MsgHandler::uploadFile(PDU *pdu)
{
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype=ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND;
    int ret;
    if(m_bUpload){
        qDebug()<<"已有文件正在上传";
        ret =1;
        memcpy(respdu->caData,&ret,sizeof(int));
        return respdu;
    }
    char caFileName[32]={'\0'};
    qint64 fileSize = 0;

    memcpy(caFileName,pdu->caData,32);
    memcpy(&fileSize,pdu->caData+32,sizeof (qint64));
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    if(m_fUploadFile.open(QIODevice::WriteOnly)){//打开成功,更新上传属性
        m_bUpload = true;
        m_iUploadTotal = fileSize;
        m_inUploadReceived=0;
        qDebug()<<"打开文件成功";
    }else{
        qDebug()<<"打开文件失败";
        ret = -1;
    }
    memcpy(respdu->caData,&ret,sizeof (int));
    return respdu;
}

PDU *MsgHandler::uploadFileData(PDU *pdu)
{
   m_fUploadFile.write(pdu->caMsg,pdu->uiMsgLen);
   m_inUploadReceived += pdu->uiMsgLen;
   if(m_inUploadReceived < m_iUploadTotal){//未接收完
       return NULL;
   }
   m_fUploadFile.close();
   m_bUpload = false;
   PDU * respdu = mkPDU(0);
   respdu->uiMsgtype=ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND;
   bool ret = m_inUploadReceived == m_iUploadTotal;
   memcpy(respdu->caData,&ret,sizeof(bool));
   return respdu;
}

PDU *MsgHandler::shareFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_SHARE_FILE_REQUEST start";
    char caCurName[32]={'\0'};
    int friendNum = 0;

    memcpy(caCurName,pdu->caData,32);
    memcpy(&friendNum,pdu->caData+32,sizeof(int));
    int size = friendNum*32;

    PDU * resendpdu = mkPDU(pdu->uiMsgLen-size);
    resendpdu->uiMsgtype=pdu->uiMsgtype;

    memcpy(resendpdu->caData,caCurName,32);
    memcpy(resendpdu->caMsg,pdu->caMsg+size,pdu->uiMsgLen-size);

    char caRecvName[32]={'\0'};
    for(int i=0;i<friendNum;i++){
        memcpy(caRecvName,pdu->caMsg+i*32,32);
        MyTcpServer::getInstance().resend(caRecvName,resendpdu);
    }
    free(resendpdu);
    resendpdu = NULL;

    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype=ENUM_MSG_TYPE_SHARE_FILE_RESPEND;
    return respdu;
}

PDU *MsgHandler::shareFileAgree(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST start";
    QString strSharePath = pdu->caMsg;
    QString strRecvPath = QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(pdu->caData);

    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size() - index - 1);
    strRecvPath = strRecvPath + "/" + strFileName;

    bool ret = QFile::copy(strSharePath,strRecvPath);
    qDebug()<<"strRecvPath"<<strRecvPath;
    qDebug()<<"strSharePath"<<strSharePath;
    PDU * respdu = mkPDU(0);
    memcpy(respdu->caData,&ret,sizeof (bool));
    respdu->uiMsgtype=ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND;
    return respdu;
}

PDU *MsgHandler::handleDownloadFileRequest(PDU *pdu)
{
//    char caFileName[32] = {'\0'};
//    char caCurPath[pdu -> uiMsgLen];
//    memcpy(caFileName, pdu -> caData, 32);
//    memcpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);

//    QString strDownloadFilePath = QString("%1/%2").arg(caCurPath).arg(caFileName);
//    fDownloadFile->setFileName(strDownloadFilePath);

//    qDebug() << "下载文件：" << strDownloadFilePath;
//    qint64 fileSize = fDownloadFile -> size();

//    PDU *resPdu = NULL;
//    if(fDownloadFile->open(QIODevice::ReadOnly))
//    {
//        resPdu = mkPDU(32 + sizeof (qint64) + 5);

//        resPdu -> uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
//        strncpy(resPdu -> caData, DOWNLOAD_FILE_START, 32);
//        sprintf((char*)resPdu -> caMsg, "%s %lld", caFileName, fileSize);
//        pTimer -> start(1000); // 开始计时器1000ms
//        qDebug() << (char*)resPdu -> caMsg;
//    }
//    else // 打开文件失败
//    {
//        resPdu = mkPDU(0);

//        resPdu -> uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
//        strncpy(resPdu -> caData, DOWNLOAD_FILE_FAILED, 32);
//    }

//    return resPdu;
}

