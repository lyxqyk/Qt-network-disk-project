#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    ~Server();
    Server(QWidget *parent = nullptr);
    //加载配置文件
    void loadConfig();
    static Server & getInstance();
    QString getRootPath();

private:
    Ui::Server *ui;

    Server(const Server& instance) = delete;
    Server& operator = (const Server&) = delete;

    //ip 和 端口号
    QString m_strIp;//设置成成员变量在函数外也能访问
    quint16 m_usProt;
    QString m_strRootPath;//根目录给注册的用户存文件
};
#endif // SERVER_H
