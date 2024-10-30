#ifndef CHAT_H
#define CHAT_H

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();
    QString m_strChatName;//目标聊天者
    void updateShow_LE(QString strMsg);

private slots:
//    void on_input_LE_clicked();

    void on_send_PB_clicked();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
