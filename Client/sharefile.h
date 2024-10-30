#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    ~ShareFile();
    void updateFriend_LW();//刷新好友界面

private slots:
    void on_allSelect_PB_clicked();

    void on_cancelSelect_PB_clicked();

    void on_ok_PB_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H
