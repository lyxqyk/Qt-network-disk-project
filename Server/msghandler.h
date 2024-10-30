#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <qfile.h>
#include <qstring.h>



class MsgHandler
{
public:
    MsgHandler();
    PDU * regist(PDU * pdu);
    PDU * login(PDU * pdu,QString& strLoginName);
    PDU * findUser(PDU * pdu);
    PDU * onlineUser();
    PDU * addFriend(PDU * pdu);
    PDU * addFriendAgree(PDU * pdu);
    PDU * flushFriend(PDU * pdu);
    PDU * deleteFriend(PDU * pdu);
    void chat(PDU * pdu);
    PDU * mkdir(PDU * pdu);
    PDU * flushFile(PDU * pdu);
    PDU * delDir(PDU *pdu);
    PDU * delFile(PDU *pdu);
    PDU * renameFile(PDU *pdu);
    PDU * moveFile(PDU *pdu);
    PDU * uploadFile(PDU *pdu);
    PDU * uploadFileData(PDU *pdu);
    PDU * shareFile(PDU *pdu);
    PDU * shareFileAgree(PDU *pdu);
    PDU * handleDownloadFileRequest(PDU *pdu);

    QFile m_fUploadFile;//上传文件对象
    qint64 m_iUploadTotal;//上传文件大小
    qint64 m_inUploadReceived;//已上传大小
    bool m_bUpload;//是否在上传

};

#endif // MSGHANDLER_H
