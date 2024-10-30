#ifndef FRIEND_H
#define FRIEND_H

#include "chat.h"
#include "onlineuser.h"

#include <QWidget>

namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();
    OnlineUser * getOnlineUser();
    void showOnlineFriend(QStringList friendList);
    void flushFriend();//用的多封装

    Chat * getChat();

    QListWidget * getFriend_LW();

private slots:
    void on_findUser_PB_clicked();

    void on_online_PB_clicked();

    void on_flushFriend_PB_clicked();

    void on_delFriend_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
    OnlineUser * m_pOnlineUser;
    Chat * m_pChat;//存聊天内容
};

#endif // FRIEND_H
