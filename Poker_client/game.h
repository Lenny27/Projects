#ifndef GAME_H
#define GAME_H

#include "game.h"
#include "enterwindow.h"
#include "lobby.h"
#include "table.h"
#include "dialog.h"

#include <QObject>
#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTimer>
#include <QMessageBox>
#include <QRect>
#include <QTimer>
#include <QPropertyAnimation>

class Game : public QObject
{
    Q_OBJECT


public:
    Game();

    QString card_str[54] = {":/Images/reverse_card.png", ":/Images/2piki.png", ":/Images/2cher.png", ":/Images/2krest.png", ":/Images/2bubi.png",
                            ":/Images/3piki.png", ":/Images/3cher.png", ":/Images/3krest.png", ":/Images/3bubi.png",
                            ":/Images/4piki.png", ":/Images/4cher.png", ":/Images/4krest.png", ":/Images/4bubi.png",
                            ":/Images/5piki.png", ":/Images/5cher.png", ":/Images/5krest.png", ":/Images/5bubi.png",
                            ":/Images/6piki.png", ":/Images/6cher.png", ":/Images/6krest.png", ":/Images/6bubi.png",
                            ":/Images/7piki.png", ":/Images/7cher.png", ":/Images/7krest.png", ":/Images/7bubi.png",
                            ":/Images/8piki.png", ":/Images/8cher.png", ":/Images/8krest.png", ":/Images/8bubi.png",
                            ":/Images/9piki.png", ":/Images/9cher.png", ":/Images/9krest.png", ":/Images/9bubi.png",
                            ":/Images/10piki.png", ":/Images/10cher.png", ":/Images/10krest.png", ":/Images/10bubi.png",
                            ":/Images/Jpiki.png", ":/Images/Jcher.png", ":/Images/Jkrest.png", ":/Images/Jbubi.png",
                            ":/Images/Qpiki.png", ":/Images/Qcher.png", ":/Images/Qkrest.png", ":/Images/Qbubi.png",
                            ":/Images/Kpiki.png", ":/Images/Kcher.png", ":/Images/Kkrest.png", ":/Images/Kbubi.png",
                            ":/Images/Apiki.png", ":/Images/Acher.png", ":/Images/Akrest.png", ":/Images/Abubi.png"
                           };

    QTcpSocket *m_socket;
    void serverConnect();
    QString message;

    EnterWindow* m_EntWin;
    Lobby* m_lobby;
    Table* m_table;
    Dialog* m_dialog;

    QString my_name;
    int number_t1;
    int t1_player;

    QPropertyAnimation* set_hands_anim[16];
    QPropertyAnimation* set_banks[6];
    QPropertyAnimation* drop_banks[10];

    QRect rects[16];
    QTimer create_banks;
    QTimer fold_timer;
    QTimer destroy_banks;
    int foldPos;



    void setHands(int* arr, int my_num);

    //~Game();


public slots:
    void onConnectButton();
    void readyReadSlot();
    void connectedSlot();
    void messageReady();
    void sendJsonDocument(QJsonDocument &d);
    void sendName(QString &name);
    void sendAction(QString &msg, int val, QString &from);
    void sendJoinTable();
    void sendCancelTable();
    void sendLose(int p);

    void onFoldButton();
    void onCallButton();
    void onRaiseButton();

    void onFoldTimer();
    void onBlindsTimer();
    void onBanksTimer();

};

#endif // GAME_H
