#ifndef TABLE_H
#define TABLE_H

#include "users.h"
#include "cards.h"
#include <time.h>

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QMap>
#include <QRect>

namespace Ui {
class Table;
}

class Table : public QWidget
{
    Q_OBJECT




    QTimer t_blind_up;

    int randInt[16];
    int play;

public:
    Table(QWidget *parent, QString &p1,
                   QString &p2, QString &p3, QString &p4,
                   QString &p5, int pos,QString &name);
    ~Table();
    int current_bet;
    int big_blind;
    int my_position;
    int round;
    int bank1[5];
    int pot;
    QTimer t_del;

    Users* users[6];
    Cards* table_cards[16];
    int cards_number[16];
    int stacks[6];
    int bets[6];
    int player_now[6];
    QString names[6];
    QLabel* bank[5];

    QLabel* extraLabel[10];

    Ui::Table *ui;


public slots:
    void setPlayers(QWidget* wgt, QString &p1,QString &p2, QString &p3,
                     QString &p4, QString &p5);
    void setCards(QWidget* wgt);


private slots:


private:


};


#endif // TABLE_H
