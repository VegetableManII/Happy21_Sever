#include <QCoreApplication>

#include "tcpsever.h"
#include "connectMysql.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    tcpsever ser(999);
    QHostAddress address("127.0.0.1");
    ser.listen(address,8001);
//    ConMysql mysql;
//    mysql.createConnection();
//    mysql.UpdateMoney(3000,"Jacky");
    return a.exec();
}
