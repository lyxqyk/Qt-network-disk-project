#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "reshandler.h"

#include <QTcpSocket>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    ~Client();
    //加载配置文件
    void loadConfig();
    static Client&getInstance();
    QTcpSocket &getTcpSocket();//获取私有成员
    QString getRootPath();

    void recvMsg();
//    PDU * readPDU();
    void handleMsg(PDU * pdu);
    void sendPDU(PDU * pdu);

    ResHandler * m_prh;
    QString m_strLoginName;
    QByteArray buffer;//当前所有数据

public slots:
    void showConnect();

private slots:
    void on_regist_PB_clicked();
    void on_login_PB_clicked();

private:
    //为了实现单例模式，私有化构造函数，删除拷贝构造函数和拷贝赋值运算符
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator = (const Client&) = delete;

    Ui::Client *ui;
    //ip 和 端口号
    QString m_strIp;//设置成成员变量在函数外也能访问
    quint16 m_usProt;
    QString m_strRootPath;
    //用于连接服务器的socket
    QTcpSocket m_tcpSocket;


};
#endif // CLIENT_H
