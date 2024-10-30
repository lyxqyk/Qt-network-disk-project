#include "operatedb.h"

#include <qmessagebox.h>

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDb::OperateDb(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}

OperateDb &OperateDb::getInstance()
{
    static OperateDb instance;
    return instance;

}

void OperateDb::connect()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("mydb2310");
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    if(m_db.open()){
        qDebug()<<"数据库连接成功";
    }else{
        QMessageBox::critical(0,"连接数据库",m_db.lastError().text());
    }
}

OperateDb::~OperateDb()
{
    m_db.close();
}

bool OperateDb::handleRegist(char *name, char *pwd)
{
    qDebug()<<"handleRegist start name"<<name
            <<"pwd"<<pwd;
    if(name == NULL ||pwd == NULL){
        return false;
    }
    //检查添加的用户是否已经存在
    QString sql = QString("select * from user_info where name ='%1'").arg(name);
    qDebug()<<"检查添加的用户是否存在 sql:"<<sql;
    QSqlQuery q;
    if(!q.exec(sql)||q.next()){
        return false;
    }
    //添加用户
    sql = QString("insert into user_info(name,pwd) values('%1','%2')").arg(name).arg(pwd);
//    qDebug()<<"添加用户:"<<sql;
    return q.exec(sql);//exec--执行
}

bool OperateDb::handleLogin(char *name, char *pwd)
{
    qDebug()<<"handleRegist start name"<<name
            <<"pwd"<<pwd;
    if(name == NULL ||pwd == NULL){
        return false;
    }
    //检查登录用户名和密码是否匹配
    QString sql = QString("select * from user_info where name='%1' and pwd='%2'").arg(name).arg(pwd);
    qDebug()<<"检查登录的用户名和密码是否正确 sql:"<<sql;
    QSqlQuery q;
    if(!q.exec(sql) || !q.next())
    {
        return false;
    }
    //将online置为1
    sql = QString("update user_info set online=1 where name='%1' and pwd='%2'").arg(name).arg(pwd);
//    qDebug()<<"online置为1 sql:"<<sql;
    return q.exec(sql);
}

void OperateDb::handleOffline(const char *name)
{
    qDebug()<<"handleOffline start";
    if(name == NULL){
        qDebug()<<"handleOffline name is NULL";
    }
    QString sql = QString("update user_info set online=0 where name='%1'").arg(name);
//    qDebug()<<"online置为1 sql:"<<sql;
    QSqlQuery q;
    q.exec(sql);
}

int OperateDb::handleFindUser(const char *name)
{
    qDebug()<<"handleFindUser start";
    if(name == NULL){
        qDebug()<<"handleFindUser name is NULL";
    }
    QString sql = QString("select online from user_info where name='%1'").arg(name);
//    qDebug()<<"查找用户的online sql:"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next())
    {
        return q.value(0).toInt();
    }
    return -1;
}

QStringList OperateDb::handleOnlineUser()
{
    qDebug()<<"handleOnlineUser start";
    QString sql = QString("select name from user_info where online = 1");
//    qDebug()<<"在线用户 sql:"<<sql;
    QSqlQuery q;
    q.exec(sql);

    QStringList result;
    while(q.next())
    {
        result.append(q.value(0).toString());
    }

    return result;
}

int OperateDb::handleAddFriend(const char *curName, const char *tarName)
{
    if(curName == NULL || tarName == NULL){
        return -1;
    }
    QString sql =QString( R"(
                          select * from friend where
                          (user_id =(select id from user_info where name = '%1')
                          and
                          friend_id = (select id from user_info where name = '%2')
                           )
                          or
                          (friend_id = (select id from user_info where name = '%3')
                           and
                          user_id = (select id from user_info where name = '%4')
                          )
                          )").arg(curName).arg(tarName).arg(curName).arg(tarName);
    qDebug()<<"是否是好友sql : "<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return -2;//已经是好友
    }
    sql = QString("select online from user_info where name = '%1'").arg(tarName);
    q.exec(sql);
//    qDebug()<<"查找要添加的好友是否在线sql"<<sql;
    if(q.next()){
        return q.value(0).toInt();//1在线，0不在线
    }
    return -1;
}

int OperateDb::handleAddFriendAgree(const char *curName, const char *tarName)
{
    if(curName == NULL || tarName == NULL){
        return 0;
    }
    QString sql = QString(R"(insert into friend(user_id,friend_id)
                          select u1.id,u2.id
                          from user_info u1,user_info u2
                          where u1.name='%1' and u2.name='%2')").arg(curName).arg(tarName);
    QSqlQuery q;
    return q.exec(sql);
}

QStringList OperateDb::handleFlushFriend(const char *name)
{
    qDebug()<<"handleFlushFriend start";
    QStringList result;
    result.clear();
    if(name == NULL){
        return result;
    }
    QString sql = QString(R"(select name from user_info
                          where id in(
                          select user_id from friend where friend_id =
                          (select id from user_info where name = '%1')
                          union
                          select friend_id from friend where user_id =
                          (select id from user_info where name = '%1')
                          )and online = 1;
                          )").arg(name);
    QSqlQuery q;
    q.exec(sql);
    while (q.next()){
        result.append(q.value(0).toString());
    }
    return result;
}

bool OperateDb::handleDelFriend(const char *curName, const char *tarName)
{
    qDebug()<<"handleDelFriend start";
    if(curName == NULL || tarName == NULL){
        return -1;
    }
    QString sql =QString( R"(
                          select * from friend where
                          (user_id =(select id from user_info where name = '%1')
                          and
                          friend_id = (select id from user_info where name = '%2')
                           )
                          or
                          (friend_id = (select id from user_info where name = '%3')
                           and
                          user_id = (select id from user_info where name = '%4')
                          );
                          )").arg(curName).arg(tarName).arg(curName).arg(tarName);
    qDebug()<<"是否是好友sql : "<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return false;//已经不是好友
    }
    sql =QString( R"(
                  delete from friend where
                  (user_id =(select id from user_info where name = '%1')
                  and
                  friend_id = (select id from user_info where name = '%2')
                   )
                  or
                  (friend_id = (select id from user_info where name = '%3')
                   and
                  user_id = (select id from user_info where name = '%4')
                  );
                  )").arg(curName).arg(tarName).arg(curName).arg(tarName);
    q.exec(sql);
//    qDebug()<<"删除的好友sql"<<sql;
    return true;
}






;
