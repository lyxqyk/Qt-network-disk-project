#include "reshandler.h"
#include "client.h"
#include "index.h"

#include <QMessageBox>
#include <string.h>

ResHandler::ResHandler()
{

}

void ResHandler::regist(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"注册","注册成功");
    }else{
        QMessageBox::information(&Client::getInstance(),"注册","注册失败，用户名或密码非法");
    }
}

void ResHandler::login(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"登录","登录成功");
        //登陆成功然后才会弹出界面
        Index::getInstance().show();
        Index::getInstance().setWindowTitle(Client::getInstance().m_strLoginName);
        //登陆成功隐藏登录页面
        Client::getInstance().hide();
    }else{
        QMessageBox::information(&Client::getInstance(),"登录","登录失败，用户名或密码非法");
    }
}

void ResHandler::findUser(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_LOGIN_FIND_USER_RESPEND start";
    char caName[32]={'\0'};
    memcpy(caName,pdu->caData,32);
    int ret;
    memcpy(&ret,pdu->caData+32,sizeof(int));
    if(ret == 1){
        int ret = QMessageBox::information(Index::getInstance().getFriend(),"搜索",QString("%1 在线").arg(caName),"添加好友","取消");
        if(ret == 0){
            PDU * pdu = mkPDU(0);
            pdu->uiMsgtype = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
            memcpy(pdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
            memcpy(pdu->caData+32,caName,32);
            Client::getInstance().sendPDU(pdu);
        }
    }else if(ret == 0){
        QMessageBox::information(Index::getInstance().getFriend(),"搜索",QString("%1 不在线").arg(caName));
    }else{
        QMessageBox::information(Index::getInstance().getFriend(),"搜索",QString("%1 不存在").arg(caName));
    }
}

void ResHandler::onlineUser(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_ONLINE_USER_RESPEND start";
    unit uiSize = pdu->uiMsgLen/32;
    char caTmp[32];
    QStringList userlist;
    for(unit i = 0; i < uiSize ; i++){
        memcpy(caTmp,pdu->caMsg+32*i,32);
        if(QString(caTmp) == Client::getInstance().m_strLoginName){
            continue;
        }
        userlist.append(QString(caTmp));
    }
    Index::getInstance().getFriend()->getOnlineUser()->showOnlineUser(userlist);
}

void ResHandler::addFriend(PDU *pdu)
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof (int));
    if(ret == -1){
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","添加错误");
    }else if(ret == -2){
         QMessageBox::information(Index::getInstance().getFriend(),"添加好友","对方已经是你的好友");
    }else if(ret == 0){
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","对方不在线");
    }
}

void ResHandler::addFriendRequest(PDU *pdu)
{
    char caName={'\0'};
    memcpy(&caName,pdu->caData,32);
    int ret = QMessageBox::question(Index::getInstance().getFriend(),"添加好友请求",QString("是否同意 '%1' 的添加好友请求").arg(caName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU * respdu = mkPDU(0);
    respdu->uiMsgtype = ENUM_MSG_TYPE_ADD_FRIEND_AGREE_REQUEST;
    memcpy(respdu->caData,pdu->caData,64);
    Client::getInstance().sendPDU(respdu);
}

void ResHandler::addFriendAgree()
{
    QMessageBox::information(Index::getInstance().getFriend(),"添加好友","添加成功");
    Index::getInstance().getFriend()->flushFriend();
}

void ResHandler::flushFriend(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_FLUSH_FRIEND_RESPEND start";
    int size = pdu->uiMsgLen/32;
    QStringList friendList;
    char caTmp[32] = {'\0'};
    for(int i=0;i<size;i++){
        memcpy(caTmp,pdu->caMsg+i*32,32);
        friendList.append(QString(caTmp));
        qDebug()<<"caTmp"<<QString(caTmp);
    }
    Index::getInstance().getFriend()->showOnlineFriend(friendList);
}

void ResHandler::deleteFriend(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND start";
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(!ret){
        QMessageBox::information(Index::getInstance().getFriend(),"删除好友","删除失败");
    }else {
         QMessageBox::information(Index::getInstance().getFriend(),"删除好友","对方已不是好友");
    }
    Index::getInstance().getFriend()->flushFriend();
}

void ResHandler::chat(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_CHAT_RESPEND start";
    Chat* c= Index::getInstance().getFriend()->getChat();
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] ={'\0'};
    memcpy(caChatName,pdu->caData,32);
    QString strMsg = QString("%1: %2").arg(caChatName).arg(pdu->caMsg);
    c->m_strChatName = caChatName;
    c->updateShow_LE(strMsg);
}

void ResHandler::mkdir(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_MKDIR_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFile(),"创建文件夹","创建成功");
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"创建文件夹","创建失败");
    }
}

void ResHandler::flushFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_FLUSH_FILE_RESPEND start";
    int iCount = pdu->uiMsgLen/sizeof (FileInfo);//文件个数
//    qDebug()<<"iCount"<<iCount;
    QList<FileInfo*> pFileList;
    for(int i=0;i<iCount;i++){
        FileInfo *pFileInfo = new FileInfo;
        memcpy(pFileInfo,pdu->caMsg+i*sizeof (FileInfo),sizeof (FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::delDir(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_DEL_DIR_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFile(),"删除文件夹","删除成功");
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件夹","删除失败");
    }
}

void ResHandler::delFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_DEL_FILE_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFile(),"删除文件","删除成功");
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"删除文件","删除失败");
    }
}

void ResHandler::renameFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_RENAME_FILE_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"重命名文件","重命名文件失败");
    }
}

void ResHandler::moveFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_MOVE_FILE_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"移动文件","移动文件失败");
    }
}

void ResHandler::uploadFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND start";
    int ret;
    memcpy(&ret,pdu->caData,sizeof (int));
    if(ret == -1){
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","打开文件失败");
    }else if(ret == -2){
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","已有文件在上传");
    }else if(ret == 0){
        Index::getInstance().getFile()->uploadFile();
    }
}

void ResHandler::uploaxdFileData(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND start";
    bool ret;
//    qDebug()<<"ret"<<ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件成功");
        Index::getInstance().getFile()->flushFile();
    }else {
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件失败");
    }
    Index::getInstance().getFile()->m_bUpload = false;
}

void ResHandler::shareFile(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_SHATE_FILE_RESPEND start";
    QMessageBox::information(Index::getInstance().getFile(),"分享文件","文件已分享");
}

void ResHandler::shareFileRequest(PDU *pdu)
{
    qDebug()<<"ENUM_MSG_TYPE_SHATE_FILE_AGREE start";
    QString strSharePath = pdu->caMsg;
    int index= strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size() - index -1);

    int ret = QMessageBox::question(Index::getInstance().getFile()->getShareFile(),
               "分享文件",
               QString("%1 分享文件 %2\n是否接收？").arg(pdu->caData).arg(strFileName));
    if(ret!=QMessageBox::Yes){
        return;
    }
    PDU * respdu = mkPDU(pdu->uiMsgLen);
    respdu->uiMsgtype = ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;
    memcpy(respdu->caData,Client::getInstance().m_strLoginName.toStdString().c_str(),32);
    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
    Client::getInstance().sendPDU(respdu);
}

void ResHandler::shareFileAgree(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(Index::getInstance().getFile(),"分享文件","分享文件成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(),"分享文件","分享文件失败");
    }
}


