#include "client.h"
#include "file.h"
#include "ui_file.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QIcon>
#include <QFileDialog>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strCurPath=QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().m_strLoginName);
    m_strUserPath = m_strCurPath;
    m_bUpload = false;
    flushFile();//构造出页面就刷新一下
    m_pShareFile = new ShareFile;
}

File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::flushFile()
{
    qDebug()<<"flushFile start";
    PDU * pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiMsgtype=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::updateFileList(QList<FileInfo *> pFileList)
{
    //释放binggengxinm_fileList
    foreach(FileInfo*pFileInfo,m_fileList){
        delete pFileInfo;
    }
    m_fileList.clear();
    m_fileList = pFileList;

    //清空并更新列表框
    ui->listWidget->clear();
    foreach(FileInfo *pFileInfo,pFileList){
        QListWidgetItem * pItem = new QListWidgetItem;//框中的每一个元素
        if(pFileInfo->iFileType==0){
            pItem->setIcon((QIcon(QPixmap(":/dir.png"))));
        }else if(pFileInfo->iFileType==1){
            pItem->setIcon((QIcon(QPixmap(":/file.png"))));
        }
        pItem->setText(pFileInfo->caName);
        qDebug()<<"pFileInfo->caName"<<pFileInfo->caName;
        ui->listWidget->addItem(pItem);//显示到界面
    }
}

void File::uploadFile()
{
    //打开要上传的文件
    QFile file(m_strUploadPath);
    if(!file.open(QIODevice::ReadOnly)){
       QMessageBox::warning(this,"上传文件","打开文件失败") ;
       return;
    }
    //构建PU，每次发送4096字节
    m_bUpload = true;
    PDU*datapdu =mkPDU(4096);
    datapdu->uiMsgtype=ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
    qint64 ret = 0;
    //循环读取文件的数据发送给服务器
    while(true){
        ret = file.read(datapdu->caMsg,4096);
        if(ret == 0){//读取成功
            break;
        }
        if(ret < 0){
            QMessageBox::warning(this,"上传文件","上传文件失败：读取失败");
            break;
        }
        //根据ret设置PDU的长度
        datapdu->uiMsgLen = ret;
        datapdu->uiPDULen = ret +sizeof(PDU);
        Client::getInstance().getTcpSocket().write((char*)datapdu,datapdu->uiPDULen);
    }
    m_bUpload = false;
    file.close();
    free(datapdu);
    datapdu = NULL;
}

ShareFile * File::getShareFile()
{
    return m_pShareFile;
}

void File::on_mkDir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","文件夹名:");
    if(strNewDir.isEmpty() || strNewDir.toStdString().size()>32){
        QMessageBox::warning(this,"新建文件夹","文件夹名字长度非法");
        return;
    }
    //文件夹名字放入caData 当前路径放在caMag
    PDU * pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgtype =ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData,strNewDir.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_delDir_PB_clicked()
{
   //获取选择的文件夹名
   QListWidgetItem * pItem= ui->listWidget->currentItem();
   if(pItem == NULL){
       QMessageBox::warning(this,"删除文件夹","请选择要删除的文件夹");
       return;
   }
   QString strDelFileName = pItem->text();
   //判断选择是否为文件夹
   foreach(FileInfo * pFileInfo,m_fileList){
       if(pFileInfo->caName ==strDelFileName && pFileInfo->iFileType !=0){
           QMessageBox::warning(this,"删除文件夹","选择的不是文件夹");
           return;
       }
   }
   //确认是否删除
   int ret =QMessageBox::question(this,"删除文件夹",QString("是否确定删除文件夹：%1").arg(strDelFileName));
   if(ret!= QMessageBox::Yes){
       return;
   }
   //完整发送路径给服务器
   QString strPath =m_strCurPath +"/"+strDelFileName;
   PDU * pdu = mkPDU(strPath.toStdString().size()+1);
   pdu->uiMsgtype =ENUM_MSG_TYPE_DEL_DIR_REQUEST;
   memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
   Client::getInstance().sendPDU(pdu);
}

void File::on_delFile_PB_clicked()
{
    //获取选择的文件名
   QListWidgetItem * pItem= ui->listWidget->currentItem();
   if(pItem == NULL){
       QMessageBox::warning(this,"删除文件","请选择要删除的文件");
       return;
   }
   QString strDelFileName = pItem->text();
   //判断选择是否为文件
   foreach(FileInfo * pFileInfo,m_fileList){
       if(pFileInfo->caName ==strDelFileName && pFileInfo->iFileType !=1){
           QMessageBox::warning(this,"删除文件","选择的不是文件");
           return;
       }
   }
   //确认是否删除
   int ret =QMessageBox::question(this,"删除文件",QString("是否确定删除文件：%1").arg(strDelFileName));
   if(ret!= QMessageBox::Yes){
       return;
   }
   //完整发送路径给服务器
   QString strPath =m_strCurPath +"/"+strDelFileName;
   PDU * pdu = mkPDU(strPath.toStdString().size()+1);
   pdu->uiMsgtype =ENUM_MSG_TYPE_DEL_FILE_REQUEST;
   memcpy(pdu->caMsg,strPath.toStdString().c_str(),strPath.toStdString().size());
   Client::getInstance().sendPDU(pdu);
}

void File::on_rename_PB_clicked()
{
    //获取要重命名的文件名
    QListWidgetItem * pItem= ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"重命名文件","请选择要重命名的文件");
        return;
    }
    QString strOldFileName = pItem->text();
    //获取要修改的文件名
    QString strNewFileName = QInputDialog::getText(this,"重命名文件","文件夹名:");
    if(strNewFileName.isEmpty() || strNewFileName.toStdString().size()>32){
        QMessageBox::warning(this,"重命名文件","文件夹名字长度非法");
        return;
    }
    //新旧文件名字caData 当前路径放在caMag
    PDU * pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgtype =ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    memcpy(pdu->caData,strOldFileName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strNewFileName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();
    //判断选择是否为文件夹
    foreach(FileInfo * pFileInfo,m_fileList){
        if(pFileInfo->caName ==strDirName && pFileInfo->iFileType !=0){
            QMessageBox::warning(this,"提示","选择的不是文件夹");
            return;
        }
    }
    m_strCurPath = m_strCurPath +'/' +strDirName;
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath == m_strUserPath){
        QMessageBox::warning(this,"返回","返回失败，已在顶层目录");
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index,m_strCurPath.size() - index);//从哪开始移除移除多少
    flushFile();
}

void File::on_mvFile_PB_clicked()
{
    if(ui->mvFile_PB->text() == "移动文件"){
        //获取要移动的文件名
        QListWidgetItem * pItem= ui->listWidget->currentItem();
        if(pItem == NULL){
            QMessageBox::warning(this,"移动文件","请选择要移动的文件");
            return;
        }
        QMessageBox::information(this,"移动文件","请选择要移动的文件");
        ui->mvFile_PB->setText("确认/取消");
        m_strMvFileName = pItem->text();
        m_strMvPath = m_strCurPath+'/'+m_strMvFileName;
        return;
    }
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    QString m_strTarPath;
    QString boxMsg;
    if(pItem == NULL){
       m_strTarPath = m_strCurPath+'/'+m_strMvFileName;
       boxMsg = QString("是否移动到当前目录下");
    }else{
        //判断选择是否为文件夹
        foreach(FileInfo * pFileInfo,m_fileList){
            if(pFileInfo->caName ==pItem->text() && pFileInfo->iFileType !=0){
                QMessageBox::warning(this,"提示","选择的不是文件夹");
                return;
            }
        }
        m_strTarPath = m_strCurPath+'/'+pItem->text()+'/'+m_strMvFileName;
        boxMsg = QString("是否移动到已选中目录下");
    }
    int ret = QMessageBox::information(this,"移动文件",boxMsg,"确认","取消");
    ui->mvFile_PB->setText("移动文件");
    if(ret != 0){//取消
        return;
    }
    int srcLen = m_strMvPath.toStdString().size();
    int tarLen = m_strTarPath.toStdString().size();
    PDU*pdu = mkPDU(srcLen+tarLen+1);
    pdu->uiMsgtype=ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    //源路径和目标路径长度放在cadat,源路径和目标路径放在camsg
    memcpy(pdu->caData,&srcLen,sizeof(int));
    memcpy(pdu->caData+sizeof(int),&tarLen,sizeof(int));
    memcpy(pdu->caMsg,m_strMvPath.toStdString().c_str(),srcLen);
    memcpy(pdu->caMsg+srcLen,m_strTarPath.toStdString().c_str(),tarLen);
    Client::getInstance().sendPDU(pdu);
}

void File::on_upload_PB_clicked()
{
    if(m_bUpload){
        QMessageBox::warning(this,"上传文件","已有文件正在上传，请稍等");
        return;
    }
    m_strUploadPath = QFileDialog::getOpenFileName();
    qDebug()<<"m_strUploadPath"<<m_strUploadPath;
    //获取文件名
    int index = m_strUploadPath.lastIndexOf('/');
    QString strFileName = m_strUploadPath.right(m_strUploadPath.length()-index-1);//size取的长度与中文字符不符
    //获取文件大小
    QFile file(m_strUploadPath);
    qint64 fileSize = file.size();
    //发送PDU文件名和大小放入cadata 文件路径放在camsg
    PDU * pdu = mkPDU(m_strUploadPath.toStdString().size()+1);
    pdu->uiMsgtype = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,&fileSize,sizeof(qint64));
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    qDebug()<<"strFileName"<<strFileName
           <<"fileSize"<<fileSize
          <<"m_strCurPath"<<m_strCurPath;
    Client::getInstance().sendPDU(pdu);
}

void File::on_shareFile_PB_clicked()
{
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }
    m_strShareFile = pItem->text();
    m_pShareFile->updateFriend_LW();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }
}

void File::on_downLoad_PB_clicked()
{
    qDebug()<<"\n on_downLoad_PB_clicked";
    QListWidgetItem *pItem = ui->listWidget->currentItem(); // 选择要下载的文件
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件！");
        return ;
    }
    // 获取保存的位置
    strDownloadFilePath = QFileDialog::getSaveFileName();
    qDebug()<<"strDownloadFilePath"<<strDownloadFilePath;

    if(strDownloadFilePath.isEmpty())
    {
        QMessageBox::warning(this, "下载文件", "请指定下载文件的位置！");            return ;
    }

    //获取文件名
    int index = strDownloadFilePath.lastIndexOf('/');
    QString strFileName = strDownloadFilePath.right(strDownloadFilePath.length()-index-1);//size取的长度与中文字符不符
    qDebug()<<"获取文件名："<<strFileName;

    //发送PDU文件名和大小放入cadata 文件路径放在camsg
    PDU * pdu = mkPDU(m_strCurPath.size()+1);
    pdu->uiMsgtype = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;

    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);

    qDebug()<<"strFileName"<<strFileName
          <<"m_strCurPath"<<m_strCurPath;
    Client::getInstance().sendPDU(pdu);
    qDebug() << "下载文件：" << pdu -> caData;
}
