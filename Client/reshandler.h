#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"



class ResHandler
{
public:
    ResHandler();
    void regist(PDU * pdu);
    void login(PDU * pdu);
    void findUser(PDU * pdu);
    void onlineUser(PDU * pdu);
    void addFriend(PDU * pdu);
    void addFriendRequest(PDU * pdu);
    void addFriendAgree();
    void flushFriend(PDU * pdu);
    void deleteFriend(PDU * pdu);
    void chat(PDU * pdu);
    void mkdir(PDU * pdu);
    void flushFile(PDU * pdu);
    void delDir(PDU * pdu);
    void delFile(PDU * pdu);
    void renameFile(PDU * pdu);
    void moveFile(PDU * pdu);
    void uploadFile(PDU * pdu);
    void uploaxdFileData(PDU * pdu);
    void shareFile(PDU * pdu);
    void shareFileRequest(PDU * pdu);
    void shareFileAgree(PDU * pdu);
};

#endif // RESHANDLER_H
