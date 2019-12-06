#ifndef TCPSEVER_H
#define TCPSEVER_H
#include <QtNetwork/QTcpServer>
#include <QHash>
#include <QTimer>
#include "tcpsocket.h"
#include "room.h"

class tcpsever : public QTcpServer
{
    Q_OBJECT
public:
    explicit tcpsever(int numConnections = 999);
    ~tcpsever() override;

    void setMaxPendingConnections(int numConnections);
signals:
    void connectCLient(const qintptr);
    void sockDisConnect(int ,QString , quint16);
    void sentDisConnect(int i);

public slots:
    void clear();

protected slots:
    void sockDisConnectSlot(int roomber,int handle,const QString & ip,quint16 port,const QThread *th);
protected:
    void incomingConnection(qintptr sockDescriptor) override;
private:
    QHash<int , room *> *playRoom;
    int maxConnections;
    int presentConnections;
};

#endif // TCPSEVER_H
