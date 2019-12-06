#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QtNetwork/QTcpSocket>
#include <QMetaType>
#include <QQueue>
#include <QFutureWatcher>
#include <QByteArray>
#include <QTimer>
#include <QTime>

class tcpsocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit tcpsocket(qintptr socketDescriptor,QObject *parent = nullptr);
    ~tcpsocket();

    void handleData(QByteArray data);

    qintptr getSocketID();
    void setPlayerNumber(const qint8 i);
    void setRoomNumber(const int i);
    qint8 getPlayerNumber();
signals:
    void sockDisConnect(const int,const qintptr,const QString &,const quint16,const QThread *);

    void playerReady(qint8 num);
    void playerShareCards();
    void playerRecvCards();
    void isBursted(qint8);
    void playerOver(qint8);
    void playerWantCards(qint8);
    void playerBelt(qint8,QString);
    void playerQuit(qint8);

public slots:
    void disConTcp(int i);
protected slots:
    void readDatas();
//    void startNext();

    void msleep(int msc);
protected:
    QFutureWatcher<void> watcher;
    QQueue<QByteArray> datas;
    QByteArray byte;

private:
    qintptr socketID;
    QMetaObject::Connection dis;
    qint8 playerNumber;
    int roomNumber;
};
#endif // TCPSOCKET_H
