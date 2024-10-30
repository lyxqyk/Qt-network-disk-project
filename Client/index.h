#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include "friend.h"
#include "file.h"

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = nullptr);
    ~Index();

    Friend * getFriend();//因为ui是私有的 要定义函数获取界面
    File * getFile();

    static Index&getInstance();

private slots:

    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    Ui::Index *ui;
};

#endif // INDEX_H
