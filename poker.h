#ifndef POKER_H
#define POKER_H

enum color_poke{Spades=0,Heart=1,Diamond=2,Club=3};
#pragma pack(1)
typedef struct aPoke
{
    color_poke type;
    unsigned char number;
}A_Poker;
#pragma pack()
#include <QObject>
#include <QMap>
class poker : public QObject
{
    Q_OBJECT
public:
    explicit poker(QObject *P = nullptr);
    void Shuffle();
    A_Poker* OutGiving();
private:
    A_Poker pokers[208];
    uint8_t position;
    QMap<uint8_t,A_Poker *> shuffledpokers;

};

#endif // POKER_H
