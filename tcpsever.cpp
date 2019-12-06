#include "tcpsever.h"
#include "threadhandle.h"

tcpsever::tcpsever(int numConnections)
{
    playRoom = new  QHash<int, room *>;
    setMaxPendingConnections(numConnections);
    this->presentConnections = 0;
}
tcpsever::~tcpsever()
{
    emit this->sentDisConnect(-1);
    delete playRoom;
}
void tcpsever::setMaxPendingConnections(int numConnections)
{
    this->QTcpServer::setMaxPendingConnections(numConnections);//调用Qtcpsever函数，设置最大连接数，主要是使maxPendingConnections()依然有效
    this->maxConnections = numConnections;
}
void tcpsever::incomingConnection(qintptr socketDescriptor) //多线程必须在此函数里捕获新连接
{
    if (presentConnections > maxPendingConnections())//继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
    {
        QTcpSocket tcp;
        tcp.setSocketDescriptor(socketDescriptor);
        tcp.disconnectFromHost();
        return;
    }

    auto tcpTemp = new tcpsocket(socketDescriptor);
//    QString ip =  tcpTemp->peerAddress().toString();
//    qint16 port = tcpTemp->peerPort();

    connect(tcpTemp,&tcpsocket::sockDisConnect,this,&tcpsever::sockDisConnectSlot);//NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
    connect(this,&tcpsever::sentDisConnect,tcpTemp,&tcpsocket::disConTcp);//断开信号

    emit connectCLient(socketDescriptor);

    this->presentConnections++;
    if(this->presentConnections%6==1)
    {
        room *temp = new  room(presentConnections/6+1);
        temp->roomMaters->append(tcpTemp);
        temp->pnumber++;
        this->playRoom->insert(presentConnections/6,temp);
        tcpTemp->setPlayerNumber(1);
        tcpTemp->setRoomNumber(presentConnections/6+1);
        QByteArray mes=QString(QString::number(tcpTemp->getPlayerNumber())+"@0@0@0@00").toUtf8();
        tcpTemp->write(mes);

        auto th = ThreadHandle::getClass().getThread();
        auto thh = ThreadHandle::getClass().getThread();
        temp->moveToThread(th);
        tcpTemp->moveToThread(thh);

        connect(tcpTemp,&tcpsocket::playerWantCards,temp,&room::playerWantCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerBelt,temp,&room::playerBelts,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerShareCards,temp,&room::shareCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerReady,temp,&room::playerReady,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerRecvCards,temp,&room::playerRecvCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::isBursted,temp,&room::playerBurst,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerOver,temp,&room::playerOver,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerQuit,temp,&room::quitRoom,Qt::DirectConnection);
    }
    else {
        for(int i=10000;i>0;i--);
        room *temp = this->playRoom->find(presentConnections/7).value();
        temp->pnumber++;
        temp->waitplayers->append(tcpTemp);
        tcpTemp->setPlayerNumber(temp->pnumber);
        tcpTemp->setRoomNumber(presentConnections/7+1);

        auto th = ThreadHandle::getClass().getThread();
        tcpTemp->moveToThread(th);

        connect(tcpTemp,&tcpsocket::playerWantCards,temp,&room::playerWantCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerBelt,temp,&room::playerBelts,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerShareCards,temp,&room::shareCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerReady,temp,&room::playerReady,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerRecvCards,temp,&room::playerRecvCards,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::isBursted,temp,&room::playerBurst,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerOver,temp,&room::playerOver,Qt::DirectConnection);
        connect(tcpTemp,&tcpsocket::playerQuit,temp,&room::quitRoom,Qt::DirectConnection);
    }
}
void tcpsever::sockDisConnectSlot(int roomNumber,int handle,const QString & ip, quint16 prot,const QThread * th)
{
    QList<tcpsocket *> *temp = this->playRoom->find(roomNumber-1).value()->roomMaters;
    for(int i=0;i<temp->size();i++)
    {
        if(temp->at(i)->getSocketID()==handle)
        {
            temp->removeAt(i);
        }
    }

    ThreadHandle::getClass().removeThread(th); //告诉线程管理类那个线程里的连接断开了
    emit sockDisConnect(handle,ip,prot);
}
void tcpsever::clear()
{
    emit this->sentDisConnect(-1);
    ThreadHandle::getClass().clear();

    for(int i=1;i<=playRoom->size();++i) {
        playRoom->find(i).value()->roomMaters->clear();
    }
    playRoom->clear();
}
