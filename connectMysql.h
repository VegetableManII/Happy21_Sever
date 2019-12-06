#ifndef CONNECTMYSQL_H
#define CONNECTMYSQL_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDebug>
#include <QString>
struct ConMysql
{
    QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");

    void createConnection()
    {
        db.setHostName("localhost");
        db.setDatabaseName("Lucky21");
        db.setUserName("root");
        db.setPassword("0811");
        db.setPort(3306);
        if(!db.open())
            qDebug("Error");
    }
    void UpdateMoney(int debelt,QString username)
    {
        QSqlQuery query;
        QString sql = QString("UPDATE userinfo SET money = '%1' WHERE username ='%2'").arg(debelt).arg(username);
        query.exec(sql);
        qDebug()<<"update "+username+" money "+QString::number(debelt)+" Ok!";
    }
    QStringList SelectMANAGER_INFO_Name()
    {
        //QString name;
        QStringList namelist;//储存所有名字
        QSqlQuery query("SELECT username FROM userinfo");
        while(query.next())
        {
            namelist<<query.value(0).toString();
        }
        return namelist;
    }
    QStringList getUsername()
    {
        //QString name;
        QStringList namelist;//储存所有名字
        QSqlQuery query("SELECT username FROM userinfo");
        while(query.next())
        {
            namelist<<query.value(0).toString();
        }
        return namelist;
    }
    QStringList getMoney()
    {
        //QString name;
        QStringList namelist;//储存所有名字
        QSqlQuery query("SELECT money FROM userinfo");
        while(query.next())
        {
            namelist<<query.value(0).toString();
        }
        return namelist;
    }
};

#endif // CONNECTMYSQL_H
