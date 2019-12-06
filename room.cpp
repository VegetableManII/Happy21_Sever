#include "room.h"

#include <QDebug>
room::room(int roomnumber) : QObject()
{
    this->roomNum=roomnumber;
    this->pnumber=0;
    this->playerTurn=0;
    this->playerRedy=0;
    this->playerBelt=0;
    this->playerShCa=0;
    this->playerRvOr=0;
    this->POKE=new  poker();
    POKE->Shuffle();
    this->roomMaters=new  QList<tcpsocket *>;
    this->waitplayers=new  QQueue<tcpsocket *>;
    for(int i=0;i<6;i++)
    {
        this->players[i]={{{Spades,0}},0,0,false};
    }
    players[6]={{{Club,0}},0,0,true};
}
//void room::welcome()
//{
//    QThread *th=QThread::currentThread();
//    th->wait(10000);
//    while (!waitplayers->isEmpty()) {
//        tcpsocket *tcpTemp=waitplayers->dequeue();
//        roomMaters->append(tcpTemp);
//        QByteArray mes=QString(QString::number(tcpTemp->getPlayerNumber())+"@0@0@0@00").toUtf8();
//        tcpTemp->write(mes);
//    }
//}
void room::clearPlayerInfo()
{
    for(int i=0;i<6;i++)
    {
        this->players[i]={{{Spades,0}},0,0};
    }
    players[6]={{{Club,0}},0,0,true};
}
void room::playerReady(qint8 num)
{
    players[num-1].flag=true;
    qDebug()<<"--------------------";
    qDebug()<<"player:"<<num<<"ready";
    playerRedy++;
    if(playerRedy==roomMaters->size())
    {
        qDebug()<<"--------------------";
        qDebug()<<"room:"<<roomNum<<"start game";
        start();
    }
}
void room::start()
{
    QByteArray mes=QString("0@1@"+QString::number(roomMaters->size()+1)+"@0@00").toUtf8();
    boradcastMessage(mes);
}
void room::playerRecvCards()
{
    playerRvOr++;
    if(playerRvOr==roomMaters->size())
    {
        setPlayerOption();
    }
}
void room::setPlayerOption()
{
    playerTurn++;
    if(playerTurn-1==roomMaters->size())
    {
        QByteArray mes=QString("0@4@"+QString::number(7)+"@0@00").toUtf8();
        boradcastMessage(mes);
    }
    else {
        qDebug()<<"--------------------";
        qDebug()<<"player:"<<roomMaters->at(playerTurn-1)->getPlayerNumber()<<"optioning";
        QByteArray mes=QString("0@4@"+QString::number(roomMaters->at(playerTurn-1)->getPlayerNumber())+"@0@00").toUtf8();
        boradcastMessage(mes);
    }
}
void room::quitRoom(qint8 playernumber)
{
    qDebug()<<"--------------------";
    qDebug()<<"player:"<<playernumber<<"quit room";
    this->players[playernumber-1]={{{Club,0}},0,0,false};
}
QString room::makeOneCard(qint8 playernumber,qint8 pos)
{
    A_Poker * tmp=POKE->OutGiving();
    color_poke p1= tmp->type;
    unsigned char a1 = tmp->number;

    QString poke_num1;

    qDebug()<<p1 <<a1;
    if(a1>9)
    {
        poke_num1=QString::number(a1);

    }
    else {
        poke_num1="0"+QString::number(a1);

    }
    poke_num1=QString::number(p1)+"@"+poke_num1;
    players[playernumber].cards[pos].type=p1;players[playernumber].cards[pos].number=a1;
    players[playernumber].num++;

    return poke_num1;
}
void room::boradcastMessage(QByteArray mes)
{
    for(int i=0;i<roomMaters->size();i++)
    {
        roomMaters->at(i)->write(mes);
        roomMaters->at(i)->flush();
    }
}
void room::shareCards()
{
    playerShCa++;
    if(playerShCa==roomMaters->size())
    {
        for(int j=0;j<roomMaters->size()+1;j++)
        {
            QByteArray m1;QByteArray m2;
            if(j==roomMaters->size())
            {
                m1=QString("0@3@7@"+makeOneCard(6,players[6].num)).toUtf8();
                m2=QString("0@3@7@"+makeOneCard(6,players[6].num)).toUtf8();
            }
            else {
                qint8 playernum=roomMaters->at(j)->getPlayerNumber();
                m1=QString("0@3@"+QString::number(playernum)+"@"+makeOneCard(playernum-1,players[playernum-1].num)).toUtf8();
                m2=QString("0@3@"+QString::number(playernum)+"@"+makeOneCard(playernum-1,players[playernum-1].num)).toUtf8();
            }
            boradcastMessage(m1);
            boradcastMessage(m2);
            isBlackJack(j);
        }
    }
}
void room::playerBelts(qint8 number,QString mes)
{
    qDebug()<<"--------------------";
    qDebug()<<"player:"<<number<<"belt"<<mes.toInt();
    belt[number-1]+=mes.toInt();
    playerBelt++;
    if(playerBelt==roomMaters->size())
    {
        QByteArray mes=QString("0@2@0@0@00").toUtf8();
        boradcastMessage(mes);
        qDebug()<<"--------------------";
        qDebug("Room:%d all players belted over!",this->roomNum);
    }
}
void room::playerWantCards(qint8 number)
{
    qDebug()<<"--------------------";
    qDebug()<<"player:"<<number<<"want card";
    if(players[number-1].num<8)
    {
        QByteArray m=QString("0@5@"+QString::number(number)+"@"+makeOneCard(number-1,players[number-1].num)).toUtf8();
        boradcastMessage(m);
    }
}
bool room::BJ(Player *p)
{
    if(p->cards[0].number==1&&p->cards[1].number>9)
    {   return true;}
    else if (p->cards[1].number==1&&p->cards[0].number>9) {
        return true;
    }
    else {
        return false;
    }
}
void room::isBlackJack(qint8 number)
{
    Player *p=&players[number-1];
    if(BJ(p))
    {
        p->points=22;
    }
}
bool room::isBursted(qint8 number)
{
    Player *p=&players[number-1];
    qint8 point=0;
    qint8 num_of_A=0;
    for(uint8_t i=0;i<=p->num;i++)
    {
        if(p->cards[i].number==1)
        {
            num_of_A++;
            point+=11;
        }
        else if (p->cards[i].number==10||p->cards[i].number==11||p->cards[i].number==12||p->cards[i].number==13) {
            point+=10;
        }
        else {
            point+=p->cards[i].number;
        }
    }
    if(point>21)
    {
        for(uint8_t i=1;i<=num_of_A;i++)
        {
            point=point-10;
        }
        if(point>21)
        {
            p->num=0;
            return true;
        }
        else {
            return false;
        }
    }
    else {
        p->points=point;
        return false;
    }
}
void room::playerBurst(qint8 num)
{
    if(isBursted(num))
    {
        qDebug()<<"--------------------";
        qDebug()<<"player:"<<num<<"burst!";
        players[num-1].points=0;
        QByteArray mes=QString("0@6@"+QString::number(num)+"@0@00").toUtf8();
        boradcastMessage(mes);
    }
    else {
        QByteArray mes=QString("0@7@"+QString::number(num)+"@0@00").toUtf8();
        boradcastMessage(mes);
    }
}

void room::playerOver(qint8 number)
{
    qDebug()<<"--------------------";
    qDebug()<<"player:"<<number<<"option over";
    if(players[number-1].num==2)
    {
        isBursted(number);
    }
    setPlayerOption();
    if(playerTurn==roomMaters->size()+1)
    {
        qDebug()<<"--------------------";
        qDebug()<<"sever optioning";
        boradcastMessage("0@8@0@0@00");
        if(allPlayersBurst())
        {
            qDebug()<<"all player burst sever win";
            boradcastMessage(QString("1@9@7@0@00").toUtf8());
            for(int i=0;i<6;i++)
            {
                belt[i]=0-belt[i];
            }
        }
        else {
            //Sever want card
            isBursted(7);
            while (players[6].points<18&&players[6].num<8) {
                QByteArray P1=QString("0@5@7@"+makeOneCard(6,players[6].num)).toUtf8();
                boradcastMessage(P1);
                if(isBursted(7))
                {
                    players[6].points=0;
                    boradcastMessage("0@6@7@0@00");
                    break;
                }
            }
            whoWin();
        }
        playerTurn=0;
        playerRedy=0;
        playerBelt=0;
        playerShCa=0;
        playerRvOr=0;
        clearPlayerInfo();
        while (!waitplayers->isEmpty()) {
            tcpsocket *tcptemp=waitplayers->dequeue();
            roomMaters->append(tcptemp);
            QByteArray mes=QString(QString::number(tcptemp->getPlayerNumber())+"@0@0@0@00").toUtf8();
            tcptemp->write(mes);
            tcptemp->flush();
        }
   }
}
bool room::allPlayersBurst()
{
    for(qint8 i=0;i<6;i++)
    {
        if(players[i].num!=0)
        {
            return false;
        }
    }
    return true;
}
bool room::sevserWin(qint8 maxPoint)
{
    if(players[6].points>maxPoint)
    {
        return true;
    }
    else {
        return false;
    }
}
bool room::drawGame(qint8 maxPoint)
{
    if(players[6].points==maxPoint)
    {
        return true;
    }
    else {
        return false;
    }
}
qint8 room::findMax()
{
    qint8 tmp=players[0].points;
    for(qint8 i=0;i<6;i++)
    {
        if(tmp<players[i].points)
        {
            tmp=players[i].points;
        }
    }
    return tmp;
}
qint8 room::winnerNum(qint8 maxpoint)
{
    qint8 num=0;
    for(qint8 i=0;i<6;i++)
    {
        if( maxpoint==players[i].points)
        {
            num++;
        }
    }
    return num;
}
void room::whoWin()
{
    qint8 maxPoint=findMax();
    if(sevserWin(maxPoint))
    {
        qDebug()<<"--------------------";
        qDebug()<<"sever win"<<players[6].points;
        boradcastMessage(QString("1@9@7@0@00").toUtf8());
        for(int i=0;i<6;i++)
        {
            if(players[i].flag==true)
            {
                belt[i]=belt[i]-belt[i];
            }
        }
    }
    else if (drawGame(maxPoint)) {
        qDebug()<<"--------------------";
        qDebug()<<"draw game";
        boradcastMessage(QString("0@9@0@0@00").toUtf8());
    }
    else {
        QString winners=QString::number(winnerNum(maxPoint));
        for(qint8 i=0;i<6;i++)
        {
            if(players[i].flag==true)
            {
                if(players[i].points==maxPoint)
                {
                    qDebug()<<"--------------------";
                    qDebug()<<"player:"<<i+1<<"win";
                    QByteArray mes=QString(winners+"@9@"+QString::number(i+1)+"@0@00").toUtf8();
                    boradcastMessage(mes);
                    belt[i]+=belt[i];
                }
                else {
                        belt[i]=belt[i]-belt[i];
                }
            }
        }
    }
    qDebug()<<"--------------------";
    for(qint8 i=0;i<roomMaters->size();i++)
    {
        qint8 playernum=roomMaters->at(i)->getPlayerNumber();        
        qDebug()<<"player:"<<playernum<<" "<<players[playernum-1].points;       
    }
    qDebug()<<"sever "<<players[6].points;
}
