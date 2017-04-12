#ifndef POKERSERVER_H
#define POKERSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>
#include <QJsonDocument>
#include <QTimer>

struct ConnData
{
    QString name;
    QByteArray buff;
    int table;
};

struct PosData
{
    int pos;
    int pl;
    int stack;
    int bet;
};

struct Bank
{
    int sum;
    QList<int> list;
};

class pokerserver :public QObject
{
    Q_OBJECT

    int m_port;

    QTcpServer m_sock;

    QMap <QTcpSocket* , ConnData*> m_connections;
    QMap <QTcpSocket* , PosData* > table_1;

    int number_t1;

    int cards_number[16];
    int t1_players;
    int hod;
    int mb;
    int bb;
    int dealer;
    Bank bank[5];
    int n_bank;
    int win[6];
    int big_blind;
    int in_game[11];
    int newBlind;

    bool busy = false;

    int x;
    int y;
    int z;
    int u;
    int current;
    int round;

    QTcpSocket* sockets[6];

    QTimer start_timer;
    QTimer timer1;
    QTimer timer2;
    QTimer blind_timer;
    int timer1_val;
    int timer2_val;

public:
    pokerserver( int port = 8888);
    void messageReady( QTcpSocket *s, ConnData *);
    void randomHands(int* arr);
    void randomFlop(int* arr);
    void checkFlush(int* arr, int* result);
    int checkStreet(int* arr);
    void checkOther(int* arr, int* result);
    bool sortDown(const int a, const int b);

    void whoWin();
    ~pokerserver();

public slots:
    void newConnection();
    void readyRead();
    void disconnected();
    void sendJsonDocument(QTcpSocket *sock, int tab, QJsonDocument &d);
    void sendNewName(QTcpSocket* sock, QString &name);
    void sendBadName(QTcpSocket* sock);
    void sendJoinTable(int tab, QString &name);
    void sendGoodName(QTcpSocket* sock, QString &name);
    void sendTable(QTcpSocket* sock, QString &player);
    void sendClearList();
    void sendClearTable();
    void sendBlockTable(int t);
    void sendTableNumber(QTcpSocket* sock, int num);
    void sendStart(QTcpSocket* sock, QString &s1, QString &s2,
                   QString &s3, QString &s4, QString &s5,
                   int num);
    void onStartTimer();
    void sendHands(int tab, int c1,int c2,int c3,int c4,int c5,int c6,int c7,int c8,int c9,int c10,int num);
    void sendFold(int tab, int p);
    void sendCall(int tab, int p);
    void sendRaise(int tab, int p, int val);
    void sendHod(int tab, int h);
    void sendEndRound(int tab);
    void sendFlop(int tab, int hint);
    void sendTurn(int tab, int hint);
    void sendRiver(int tab, int hint);
    void nextHod();
    void sendShowCards(int tab);
    void sendWin(int tab, int* win);
    void nextPos();
    void onTimer1();
    void onTimer2();
    void sendWinner(int tab);
    void sendTableBusy(QTcpSocket* s);
    void onBlindTimer();
    void sendDisconnect(int tab, int pos);

};

#endif // POKERSERVER_H
