#include "protocol.h"
#include <stdlib.h>
#include <string.h>

PDU *mkPDU(unit uiMsgLen)
{
    unit uiPDULen = sizeof(PDU) + uiMsgLen;//要申请的总长度
    PDU *pdu =(PDU*)malloc(uiPDULen);
    if(pdu == NULL)
    {
        exit(1);//结束进程
    }
    //申请完长度最好初始化一下
    memset(pdu,0,uiPDULen);
    pdu->uiPDULen=uiPDULen;
    pdu->uiMsgLen=uiMsgLen;
    return pdu;
}
