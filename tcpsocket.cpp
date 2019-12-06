#include "tcpsocket.h"

#include <QtConcurrent/QtConcurrent>
#include <QtNetwork/QHostAddress>
#include <QDebug>

tcpsocket::tcpsocket(qintptr socketDescriptor,QObject *parent) : QTcpSocket(parent),socketID(socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    connect(this,&tcpsocket::readyRead,this,&tcpsocket::readDatas);


        dis = connect(this,&tcpsocket::disconnected,
            [&](){
                qDebug() << "disconnect..." ;
                emit playerQuit(this->playerNumber);
                const QThread *th=QThread::currentThread();
                qRegisterMetaType<qintptr>("qintptr");
                emit sockDisConnect(roomNumber,socketID,this->peerAddress().toString(),this->peerPort(),th);
                this->deleteLater();
            });

//        connect(&watcher,&QFutureWatcher<QByteArray>::finished,this,&tcpsocket::startNext);
//        connect(&watcher,&QFutureWatcher<QByteArray>::canceled,this,&tcpsocket::startNext);
        qDebug() << "new connect..." ;
}
tcpsocket::~tcpsocket()
{
}
//void tcpsocket::sentData(const QByteArray &data, const int id)
//{
//    if(id == socketID)
//    {
//        write(data);
//        flush();
//    }
//}

void tcpsocket::disConTcp(int i)
{
    if (i == socketID)
    {
        this->disconnectFromHost();
    }
    else if (i == -1) //-1为全部断开
    {
        disconnect(dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}

void tcpsocket::readDatas()
{
    //将socket内缓存数据全部取出，保存至datas
    QByteArray tmp=this->read(10);

    if(tmp.isEmpty())
    {

    }
    else {
        datas.enqueue(tmp);

        while (!datas.isEmpty()) {
            handleData(datas.dequeue());
        }
//        if (!watcher.isRunning())//放到异步线程中处理。
//        {
//            watcher.setFuture(QtConcurrent::run(this,&tcpsocket::handleData,datas.dequeue()));
//        }
    }
}

void tcpsocket::handleData(QByteArray data)
{
    byte.append(data);

    QStringList mess=QString(byte).split("@");
    if(mess.at(0)=="00")
    {
        emit playerReady(playerNumber);
    }
    else if(mess.at(0)=="01"){
        emit this->playerBelt(playerNumber,mess.at(1));
    }
    else if (mess.at(0)=="02") {
        emit this->playerShareCards();
    }
    else if (mess.at(0)=="03") {
        emit this->playerRecvCards();
    }
    else if(mess.at(0)=="04")
    {
        emit this->playerWantCards(playerNumber);
    }
    else if(mess.at(0)=="05"){
        emit this->isBursted(playerNumber);
    }
    else if(mess.at(0)=="06"){
        emit this->playerOver(playerNumber);
    }
    byte.clear();
    return;
}

//void tcpsocket::startNext()
//{
//    if(datas.isEmpty()){
//        datas.append(byte);
//        byte.clear();
//    }

//    if (!datas.isEmpty())
//        watcher.setFuture(QtConcurrent::run(this,&tcpsocket::handleData,datas.dequeue()));
//}

void tcpsocket::msleep(int msc)
{
    QTime time;
    time.start();

    QElapsedTimer tm;
    tm.start();
    while(tm.elapsed() < msc) {}
}
qintptr tcpsocket::getSocketID()
{
    return this->socketID;
}
qint8 tcpsocket::getPlayerNumber()
{
    return this->playerNumber;
}
void tcpsocket::setPlayerNumber(const qint8 i)
{
    this->playerNumber=i;
}
void tcpsocket::setRoomNumber(const int i)
{
    this->roomNumber=i;
}
