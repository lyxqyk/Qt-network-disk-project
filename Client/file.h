#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QWidget>
#include <qlistwidget.h>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strCurPath;
    QString m_strUserPath;//每个用户的固定路径
    void flushFile();
    QList<FileInfo*>m_fileList;
    void updateFileList(QList<FileInfo*> pFileList);
    QString m_strMvFileName;
    QString m_strMvPath;
    QString m_strUploadPath;//上传路径
    bool m_bUpload;//是否上传
    void uploadFile();
    QString m_strShareFile;//分享的文件名
    ShareFile * getShareFile();
    QString strDownloadFilePath;

private slots:
    void on_mkDir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_delDir_PB_clicked();

    void on_delFile_PB_clicked();

    void on_rename_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_mvFile_PB_clicked();

    void on_upload_PB_clicked();

    void on_shareFile_PB_clicked();


    void on_downLoad_PB_clicked();

private:
    Ui::File *ui;
    ShareFile * m_pShareFile;//分享文件界面类

};

#endif // FILE_H
