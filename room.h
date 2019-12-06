#ifndef ROOM_H
#define ROOM_H

#include "tcpsocket.h"
#include "poker.h"
#include <QThread>
#include <QObject>
#include <QList>
#include <QQueue>

typedef struct CARDS
{
    A_Poker cards[7];
    qint8 num;
    qint8 points;
    bool flag;
}Player;
class room : public QObject
{
    Q_OBJECT
public:
    explicit room(int roomnumber);
public:
    qint8 pnumber;
    QList<tcpsocket *> *roomMaters;
    QQueue<tcpsocket *> *waitplayers;
//    void welcome();
public slots:
    void playerOver(qint8 player);
    void playerWantCards(qint8 player);
    void playerBelts(qint8 player,QString mes);
    void playerBurst(qint8 num);
    void playerRecvCards();
    void shareCards();
    void playerReady(qint8 num);
    void quitRoom(qint8 playernumber);
private:
    poker *POKE;
    int roomNum;
    qint8 playerTurn;

    qint8 playerRedy;
    qint8 playerBelt;
    qint8 playerShCa;
    qint8 playerRvOr;

    Player players[7];
    int belt[6];

    void whoWin();
    void setPlayerOption();
    bool isBursted(qint8 number);
    bool BJ(Player *p);
    void isBlackJack(qint8 number);
    void start();
    QString makeOneCard(qint8 playernum,qint8 pos);
    void boradcastMessage(QByteArray mes);
    bool allPlayersBurst();
    bool sevserWin(qint8 maxPoint);
    bool drawGame(qint8 maxPoint);
    qint8 findMax();
    qint8 winnerNum(qint8 maxpoint);
    void clearPlayerInfo();
};

#endif // ROOM_H
