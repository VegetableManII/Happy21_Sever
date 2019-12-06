#include "poker.h"
#include "ctime"
#include <QtGlobal>
#include <QMap>
poker::poker(QObject *P)
{
    position=0;
    for(int i=0;i<208;i++)
    {
        if(i<52)
        {
            this->pokers[i].type=Spades;
        }
        else if (i>=52&&i<104) {
            this->pokers[i].type=Heart;
        }
        else if (i>=104&&i<156) {
            this->pokers[i].type=Diamond;
        }
        else if (i>=156&&i<208) {
            this->pokers[i].type=Club;
        }
        this->pokers[i].number=(uint8_t)((i%13)+1);
    }
}
void poker::Shuffle()   //xi pai
{
    qsrand(time(0));
    uint8_t temp = 0;
    for(int i=0;i<208;i++)
    {
        temp = qrand()%208;
        if(!this->shuffledpokers.contains(temp))
        {
            shuffledpokers.insert(i,&pokers[temp]);
        }
        else {
            i--;
        }
    }
}
A_Poker* poker::OutGiving()
{ 
    if(position==208)
    {
        this->Shuffle();
        position=0;
    }
    A_Poker * t = this->shuffledpokers[position];
    position++;
    return t;
}
