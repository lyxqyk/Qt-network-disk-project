#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDb : public QObject
{
    Q_OBJECT
public:
    QSqlDatabase m_db;//数据库对象
    static OperateDb& getInstance();
    void connect();
    ~OperateDb();
    bool handleRegist(char *name,char *pwd);
    bool handleLogin(char *name,char *pwd);
    void handleOffline(const char *name);//哪个用户要移除
    int handleFindUser(const char *name);//处理查找用户请求
    QStringList handleOnlineUser();
    int handleAddFriend(const char *curName ,const char * tarName );
    int handleAddFriendAgree(const char *curName ,const char * tarName );
    QStringList handleFlushFriend(const char*name);
    bool handleDelFriend(const char *curName ,const char * tarName );

private:
    explicit OperateDb(QObject *parent = nullptr);
    OperateDb(const OperateDb&instance) = delete;
    OperateDb& operator=(const OperateDb&) = delete;

signals:

};

#endif // OPERATEDB_H
