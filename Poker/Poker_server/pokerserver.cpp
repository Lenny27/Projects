#include "pokerserver.h"
#include "cstdlib"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <time.h>
#include <unistd.h>

bool pokerserver::sortDown(const int a, const int b){
    return a > b;
}

pokerserver::pokerserver(int port) //OK
{
    bool ret;
    m_port = port;
    number_t1 = 0;

    QObject::connect(&m_sock, SIGNAL(newConnection()),
                     this, SLOT(newConnection()));

    ret = m_sock.listen(QHostAddress::Any, m_port);
    qDebug() << "Listen on port " << m_port << ": " << ret;

    qDebug() << "timer starts" <<
                QObject::connect(&start_timer, SIGNAL(timeout()), this, SLOT(onStartTimer()));
    qDebug() << "timer1" <<
                QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(onTimer1()));
    qDebug() << "timer2" <<
                QObject::connect(&timer2, SIGNAL(timeout()), this, SLOT(onTimer2()));
    qDebug() << "blind timer" <<
                QObject::connect(&blind_timer, SIGNAL(timeout()),this,SLOT(onBlindTimer()));

}

void pokerserver::newConnection() //OK
{
    qDebug() << "New connection available";
    QTcpSocket *s;

    while( s = m_sock.nextPendingConnection()) {
        ConnData *d = new ConnData();
        m_connections[s] = d;
        qDebug() << "connect ready read: "<<
                    QObject::connect(s, SIGNAL(readyRead()),
                    this, SLOT(readyRead()));
        qDebug() << "connect disconnected: "<<
                    QObject::connect(s, SIGNAL(disconnected()),
                                     this, SLOT(disconnected()));
    }
}

void pokerserver::sendJsonDocument(QTcpSocket *sock,int tab,
                                  QJsonDocument &d)//OK
{


    QString s;
//    QTcpSocket* ss;
    ConnData* dd;

    s = d.toJson(QJsonDocument::Compact) + "\n";

    if( sock != 0)
    {
        sock->write(s.toLocal8Bit());
    }
    else
    {
        if(tab != 0){
            foreach(sock, m_connections.keys()){
                dd = m_connections[sock];
                if(dd->table == tab)
                {sock->write(s.toLocal8Bit());}
            }
        }
        else
        {
            foreach( sock, m_connections.keys())
            {
                sock->write(s.toLocal8Bit());
            }
        }

    }

}

void pokerserver::sendNewName(QTcpSocket* sock, QString &name) //OK
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "new_user";
    o["data"] = name;

    d.setObject(o);

    sendJsonDocument(sock,0, d);
}

void pokerserver::messageReady(QTcpSocket *s, ConnData *d)//OK
{
    QJsonDocument doc;
    QJsonObject o;

    doc = QJsonDocument::fromJson(d->buff);
    o = doc.object();

    if( o["type"] == "name")
    {
        QJsonValue v = o["data"];
        QString name = v.toString();
        QTcpSocket* ss;
        ConnData* dd;
        bool res = false;
        foreach (ss, m_connections.keys())
        {
            dd = m_connections[ss];
            QString new_name = dd->name;
            if(name == new_name)
            {
                res = true;
                sendBadName(s);
            }
        }
        if(!res)
        {
            sendGoodName(s, name);
            if(!busy){
                sendTableNumber(s,number_t1);
            }else{ sendTableBusy(s);}

            foreach(ss, m_connections.keys())
            {
                dd = m_connections[ss];
                QString str = dd->name;
                sendNewName(s, str);
                if(dd->table == 1){
                    sendTable(s,str);
                }
            }

            d->name = name;
            d->table = 0;
            sendNewName(0, name);
        }
    }
    else if (o["type"] == "joinTable")
    {
        QJsonValue v = o["table"];
        int t = v.toInt();

        d->table = 1;
        number_t1++;

        sendJoinTable(t, d->name);

    }else if (o["type"] == "cancelTable"){
        number_t1--;
        d->table = 0;
        sendClearTable();
        sendTableNumber(0,number_t1);
        QTcpSocket* ss;
        ConnData* dd;
        foreach(ss, m_connections.keys()){
            dd = m_connections[ss];
            if(dd->table == 1){
                QString sstr = dd->name;
                sendTable(0, sstr);
            }
        }
    }else if (o["type"] == "startGame"){
        busy = true;
        int t = o["table"].toInt();
        sendBlockTable(t);
        QTcpSocket* ss;
        ConnData* dd;
        int i = 1;
        QString str[6];

        for(i = 1; i<=10; i++){
            in_game[i] = 1;
        }
        i = 1;
        foreach(ss, m_connections.keys()){
            dd = m_connections[ss];
            if(dd->table == 1){
                str[i]=dd->name;
                qDebug() << str[i];
                sockets[i] = ss;
                i++;
            }
        }
        QString s1 = str[1];
        QString s2 = str[2];
        QString s3 = str[3];
        QString s4 = str[4];
        QString s5 = str[5];
        i = 1;
        int st,bet;
        big_blind = 40;
        newBlind = 40;
        qDebug() << "before";
        foreach(ss, m_connections.keys()){
            dd = m_connections[ss];
            if(dd->table == 1){
                switch(i)
                {
                    case 1:  st=1460; bet=big_blind; break;
                    case 2:  st=1480; bet=big_blind/2; break;
                    case 3:  st=1500; bet=0; break;
                    case 4:  st=1500; bet=0; break;
                    case 5:  st=1500; bet=0; break;
                }
                PosData* ddd = new PosData();
                table_1[ss] = ddd;
                ddd->pos = i;
                ddd->pl = 1;
                ddd->bet = bet;
                ddd->stack = st;
                sendStart(ss,s1,s2,s3,s4,s5,i);
                i++;
            }
        }
        qDebug() << "after";
        t1_players = 5;
        hod = 5;
        dealer = 3;
        mb = 2;
        bb = 1;
        bank[0].sum = 0; bank[1].sum = 0; bank[2].sum = 0; bank[3].sum = 0; bank[4].sum = 0;
        bank[0].list.clear();
        bank[1].list.clear();
        bank[2].list.clear();
        bank[3].list.clear();
        bank[4].list.clear();

        x=0;
        y=0;
        z=0;
        u=0;
        current = big_blind;
        round = 1;


//        qDebug() << "timer starts" <<
//                    QObject::connect(&start_timer, SIGNAL(timeout()), this, SLOT(onStartTimer()));
//        qDebug() << "timer1" <<
//                    QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(onTimer1()));
//        qDebug() << "timer2" <<
//                    QObject::connect(&timer2, SIGNAL(timeout()), this, SLOT(onTimer2()));

        start_timer.start(3000);
        blind_timer.start(420000);


        qDebug() << "in the end";

    }else if(o["type"] == "fold"){
        int t = o["table"].toInt();
        int p = o["pos"].toInt();
        int c = o["current"].toInt();
        int r = o["round"].toInt();

        current = c;
        round = r;

        PosData* dd = table_1[s];
        dd->pl = 0;

        sendFold(t,p);

        QTcpSocket* ss;
        foreach(ss,table_1.keys()){
            dd = table_1[ss];
            if(dd->pl > 0){ x++; }
            if(dd->pl == 1){ y++; }
            if((dd->pl > 0)and(dd->bet != c)){ z++; }
            if(dd->pl == -1){ u++; }
        }
        qDebug() << "fold" << x << y << z << u;
        bool err = (x + u == 1)or(x == 1)or((x == 1)and(z == 0));
        qDebug() << err;
        if((x + u == 1)or(x == 0)or((x == 1)and(z == 0))){
            if(x + u == 1){
                qDebug() << "no show";
                sendEndRound(t);
                timer1_val = 5;
                timer1.start(1000);
            }else{
                qDebug() << "show";
                sendEndRound(t);
                sendShowCards(t);
                qDebug() << "round" << r;
                timer2_val = r;
                timer2.start(1000);
            }
            goto ex1;
        }else{
            for(int i = 0; i<=6; i++){
                hod--;
                if(hod == 0){ hod = 5; }
                ss = sockets[hod];
                dd = table_1[ss];
                if(dd->pl > 0){
                    if(((dd->pl > 0)and(c != dd->bet))or(dd->pl == 1)){
                        qDebug() << "Hod";
                        sendHod(t,dd->pos);
                        goto ex1;
                    }
                }
            }
        }
        qDebug() << "ulica";
        switch(r){
            case 1:
                sendEndRound(t);
                timer1_val = 1;
                timer1.start(1000);
                break;
            case 2:
                sendEndRound(t);
                timer1_val = 2;
                timer1.start(1000);
                break;
            case 3:
                sendEndRound(t);
                timer1_val = 3;
                timer1.start(1000);
                break;
            case 4:
                sendEndRound(t);
                timer1_val = 4;
                timer1.start(1000);
                break;
        }

        ex1:;

        x=0;
        y=0;
        z=0;
        u=0;

    }else if(o["type"] == "call"){
        int t = o["table"].toInt();
        int p = o["pos"].toInt();
        int c = o["current"].toInt();
        int r = o["round"].toInt();

        current = c;
        round  = r;

        PosData* dd = table_1[s];
        dd->pl = 2;

        if(dd->stack + dd->bet > c){
            dd->stack = dd->stack - (c - dd->bet);
            dd->bet = c;
        }else{
            dd->bet = dd->stack + dd->bet;
            dd->stack = 0;
            dd->pl = -1;
        }
        sendCall(t,p);

        QTcpSocket* ss;
        foreach(ss,table_1.keys()){
            dd = table_1[ss];
            if(dd->pl > 0){ x++; }
            if(dd->pl == 1){ y++; }
            if((dd->pl > 0)and(dd->bet != c)){ z++; }
            if(dd->pl == -1){ u++; }
        }
        qDebug() << "call" << x << y << z << u;
        bool err = (x + u == 1)or(x == 1)or((x == 1)and(z == 0));
        qDebug() << err;
        if((x + u == 1)or(x == 0)or((x == 1)and(z == 0))){
            if(x + u == 1){
                qDebug() << "no show";
                sendEndRound(t);
                timer1_val = 5;
                timer1.start(1000);
            }else{
                qDebug() << "show";
                sendEndRound(t);
                sendShowCards(t);
                qDebug() << "round" << r;
                timer2_val = r;
                timer2.start(1000);
            }
            goto ex2;
        }else{
            for(int i = 0; i<=6; i++){
                hod--;
                if(hod == 0){ hod = 5; }
                ss = sockets[hod];
                dd = table_1[ss];
                if(dd->pl > 0){
                    if(((dd->pl > 0)and(c != dd->bet))or(dd->pl == 1)){
                        qDebug() << "Hod";
                        sendHod(t,dd->pos);
                        goto ex2;
                    }
                }
            }
        }
        qDebug() << "ulica";
        switch(r){
            case 1:
                sendEndRound(t);
                timer1_val = 1;
                timer1.start(1000);
                break;
            case 2:
                sendEndRound(t);
                timer1_val = 2;
                timer1.start(1000);
                break;
            case 3:
                sendEndRound(t);
                timer1_val = 3;
                timer1.start(1000);
                break;
            case 4:
                sendEndRound(t);
                timer1_val = 4;
                timer1.start(1000);
                break;
        }

        ex2:;

        x=0;
        y=0;
        z=0;
        u=0;

    }else if (o["type"] == "raise"){
        int t = o["table"].toInt();
        int p = o["pos"].toInt();
        int v = o["value"].toInt();
        int c = o["current"].toInt();
        int r = o["round"].toInt();

        round  = r;
        current = v;

        PosData* dd = table_1[s];
        dd->pl = 2;

        c = v;
        dd->stack = dd->stack - (v - dd->bet);

        dd->bet = v;
        if(dd->stack == 0){
            dd->pl = -1;
        }

        sendRaise(t,p,v);

        QTcpSocket* ss;
        foreach(ss,table_1.keys()){
            dd = table_1[ss];
            if(dd->pl > 0){ x++; }
            if(dd->pl == 1){ y++; }
            if((dd->pl > 0)and(dd->bet != c)){ z++; }
            if(dd->pl == -1){ u++; }
        }
        qDebug() << "raise" << x << y << z << u;
        if((x + u == 1)or(x == 0)or((x == 1)and(z == 0))){
            if(x + u == 1){
                sendEndRound(t);
                timer1_val = 5;
                timer1.start(1000);
            }else{
                sendEndRound(t);
                sendShowCards(t);
                qDebug() << "round" << r;
                timer2_val = r;
                timer2.start(1000);
            }
            goto ex3;
        }else{
            for(int i = 0; i<=6; i++){
                hod--;
                if(hod == 0){ hod = 5; }
                ss = sockets[hod];
                dd = table_1[ss];
                if(dd->pl > 0){
                    if(((dd->pl > 0)and(c != dd->bet))or(dd->pl == 1)){
                        sendHod(t,dd->pos);
                        goto ex3;
                    }
                }
            }
        }
        switch(r){
            case 1:
                sendEndRound(t);
                timer1_val = 1;
                timer1.start(1000);
                break;
            case 2:
                sendEndRound(t);
                timer1_val = 2;
                timer1.start(1000);
                break;
            case 3:
                sendEndRound(t);
                timer1_val = 3;
                timer1.start(1000);
                break;
            case 4:
                sendEndRound(t);
                timer1_val = 4;
                timer1.start(1000);
                break;
        }

        ex3:;

        x=0;
        y=0;
        z=0;
        u=0;

    }else if(o["type"] == "lose"){
        int p = o["pos"].toInt();
        QTcpSocket* ss = sockets[p];
        PosData* dd = table_1[ss];
        ConnData* cd = m_connections[ss];
        dd->pl = -2;
        t1_players--;
        in_game[2*p] = 0;
        in_game[2*p - 1] = 0;
        cd->table = 0;

    }


    d->buff = "";
}

void pokerserver::readyRead()//OK
{

    QTcpSocket *s = (QTcpSocket *)this->sender();

    ConnData *d = m_connections[s];

    QByteArray ba = s->readAll();

    for( int i=0; i<ba.length(); i++){
        char c = ba.at(i);
        if(c != '\n') {
            d->buff.append(c);
        } else {
            messageReady(s, d);
        }
    }

}

void pokerserver::disconnected()//OK
{
    qDebug() << "i am disconnect";
    QTcpSocket *s = (QTcpSocket *)this->sender();

    ConnData *d = m_connections[s];
    m_connections.remove(s);
    QTcpSocket* ss;
    ConnData* dd;

    if( ! d->name.isEmpty()) {
        sendClearList();
        foreach(ss, m_connections.keys()){
            dd = m_connections[ss];
            QString str = dd->name;
            sendNewName(0,str);
        }
    }
    if(d->table == 1){
        sendClearTable();
        number_t1--;
        sendTableNumber(0,number_t1);
        foreach(ss, m_connections.keys()){
            dd = m_connections[ss];
            if(dd->table == 1){
                QString sstr = dd->name;
                sendTable(0,sstr);
            }
        }
        if(busy){
        PosData* pd = table_1[s];
        pd->pl = -2;
        in_game[2*pd->pos] = 0;
        in_game[2*pd->pos - 1] = 0;
        pd->stack = 0;
        t1_players--;
        sendDisconnect(d->table, pd->pos);

        if(pd->pos == hod){
            foreach(ss,table_1.keys()){
                pd = table_1[ss];
                if(pd->pl > 0){ x++; }
                if(pd->pl == 1){ y++; }
                if((pd->pl > 0)and(pd->bet != current)){ z++; }
                if(pd->pl == -1){ u++; }
            }
            qDebug() << "call" << x << y << z << u;
            bool err = (x + u == 1)or(x == 1)or((x == 1)and(z == 0));
            qDebug() << err;
            if((x + u == 1)or(x == 0)or((x == 1)and(z == 0))){
                if(x + u == 1){
                    qDebug() << "no show";
                    sendEndRound(1);
                    timer1_val = 5;
                    timer1.start(1000);
                }else{
                    qDebug() << "show";
                    sendEndRound(1);
                    sendShowCards(1);
                    qDebug() << "round" << round;
                    timer2_val = round;
                    timer2.start(1000);
                }
                goto ex2;
            }else{
                for(int i = 0; i<=6; i++){
                    hod--;
                    if(hod == 0){ hod = 5; }
                    ss = sockets[hod];
                    pd = table_1[ss];
                    if(pd->pl > 0){
                        if(((pd->pl > 0)and(current != pd->bet))or(pd->pl == 1)){
                            qDebug() << "Hod";
                            sendHod(1,pd->pos);
                            goto ex2;
                        }
                    }
                }
            }
            qDebug() << "ulica";
            switch(round){
                case 1:
                    sendEndRound(1);
                    timer1_val = 1;
                    timer1.start(1000);
                    break;
                case 2:
                    sendEndRound(1);
                    timer1_val = 2;
                    timer1.start(1000);
                    break;
                case 3:
                    sendEndRound(1);
                    timer1_val = 3;
                    timer1.start(1000);
                    break;
                case 4:
                    sendEndRound(1);
                    timer1_val = 4;
                    timer1.start(1000);
                    break;
            }

            ex2:;

            x=0;
            y=0;
            z=0;
            u=0;
        }
        }
    }

    s->deleteLater();
}

void pokerserver::sendBadName(QTcpSocket* sock)//OK
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "BadName";

    d.setObject(o);

    sendJsonDocument(sock,0, d);
}

void pokerserver::sendJoinTable(int tab, QString &name)//OK
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "joinTable";
    o["table"] = tab;
    o["name"] = name;

    d.setObject(o);

    sendJsonDocument(0,0, d);
}

void pokerserver::sendGoodName(QTcpSocket* sock, QString &name)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "GoodName";
    o["data"] = name;

    d.setObject(o);

    sendJsonDocument(sock,0,d);
}

void pokerserver::sendTable(QTcpSocket* sock, QString &player)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "Table";
    o["player"] = player;

    d.setObject(o);

    sendJsonDocument(sock,0, d);
}

void pokerserver::sendClearList()
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "ClearList";
    d.setObject(o);
    sendJsonDocument(0,0,d);
}

void pokerserver::sendClearTable()
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "ClearTable";
    d.setObject(o);
    sendJsonDocument(0,0,d);
}

void pokerserver::sendBlockTable(int t)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "BlockTable";
    d.setObject(o);
    sendJsonDocument(0,0,d);
}

void pokerserver::sendTableNumber(QTcpSocket* sock, int num)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "TableNumber";
    o["table"] = 1;
    o["number"] = num;
    d.setObject(o);
    sendJsonDocument(sock,0,d);
}

void pokerserver::sendStart(QTcpSocket* sock, QString &s1, QString &s2,
                            QString &s3, QString &s4, QString &s5,
                            int num)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "Start";
    o["p1"] = s1;
    o["p2"] = s2;
    o["p3"] = s3;
    o["p4"] = s4;
    o["p5"] = s5;
    o["number"] = num;
    d.setObject(o);
    sendJsonDocument(sock,0,d);
}

void pokerserver::onStartTimer()
{
    qDebug() << "1";
    start_timer.stop();
    qDebug() << "2";
    randomHands(cards_number);
    qDebug() << "3";
    sendHands(1,cards_number[1],cards_number[2],cards_number[3],cards_number[4],cards_number[5],cards_number[6],cards_number[7],cards_number[8],cards_number[9],cards_number[10],t1_players);
    qDebug() << "4";
}

void pokerserver::sendHands(int tab, int c1,int c2,int c3,int c4,int c5,int c6,int c7,int c8,int c9,int c10,int num)
{
    round = 1;
    current = newBlind;

    QTcpSocket* ss;
    ConnData* cd;
    sendClearTable();

    foreach(ss, m_connections.keys()){
        cd = m_connections[ss];
        if(cd->table == 1){
            QString sstr = cd->name;
            sendTable(0, sstr);
        }
    }

    n_bank = 0;
    bank[0].sum = 0; bank[1].sum = 0; bank[2].sum = 0; bank[3].sum = 0; bank[4].sum = 0;
    bank[0].list.clear();
    bank[1].list.clear();
    bank[2].list.clear();
    bank[3].list.clear();
    bank[4].list.clear();

    if(t1_players == 1){
        sendWinner(tab);
    }else{

    QJsonDocument d;
    QJsonObject o;
    QJsonArray a;
    a.append(c1);
    a.append(c2);
    a.append(c3);
    a.append(c4);
    a.append(c5);
    a.append(c6);
    a.append(c7);
    a.append(c8);
    a.append(c9);
    a.append(c10);

    o["type"] = "hands";
    o["data"] = a;
    o["number"] = num;
    o["deal"] = dealer;
    o["m_blind"] = mb;
    o["b_blind"] = bb;
    o["hod"] = hod;
    o["blindValue"] = newBlind;

    d.setObject(o);
    sendJsonDocument(0,tab,d);

    }
}

void pokerserver::randomHands(int* arr)
{
    int i,j,ii;
    for(i = 0; i<16;i++){ arr[i]=0;}
    srand(time(NULL));
    for(i=1; i<=10; i++){
       if(in_game[i] != 0){
            replace:;
            j = rand()%52+1;
            for(ii=1; ii<=10; ii++)
            {
                if(j == arr[ii]){goto replace;}
            }
            arr[i] = j;
        }else{
            arr[i] = -1;
        }
    }
    return;
}

void pokerserver::sendFold(int tab, int p)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "fold";
    o["pos"] = p;
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::sendCall(int tab, int p)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "call";
    o["pos"] = p;
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::sendRaise(int tab, int p, int val)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "raise";
    o["pos"] = p;
    o["value"] = val;
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::sendHod(int tab, int h)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "hod";
    o["hod"] = h;
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::sendEndRound(int tab)
{
    QTcpSocket*ss;

    foreach(ss,table_1.keys()){
        PosData* ddd = table_1[ss];
        if(ddd->pl == 2){ddd->pl = 1;}
    }

    int j, min=10000;
    int nb;
    int sum[5] = {0,0,0,0,0};
    int bets[6] = {0,0,0,0,0,0};
    int pl[6] = {0,0,0,0,0,0};
    foreach(ss,table_1.keys()){
        PosData* dd = table_1[ss];
        bets[dd->pos] = dd->bet;
        dd->bet = 0;
        if((dd->pl > 0)or(dd->pl == -1)){
            pl[dd->pos] = 1;
        }
    }

    nb = n_bank;

    while(nb < 5){
        for(j=1; j<=5; j++){
            if((pl[j] == 1)and(bets[j] < min)and(bets[j]!=0)){
                min = bets[j];
            }
        }
        for(j=1;j<=5; j++){
            if(bets[j] < min){
                sum[nb] += bets[j];
                bets[j] = 0;
            }else{
                sum[nb] += min;
                bank[nb].list.append(j);
                bets[j] -= min;
                n_bank = nb;
            }
        }
        nb++;
        min = 10000;
    }

    for(j = 0; j<=4;j++){
        bank[j].sum += sum[j];
    }




    QJsonDocument d;
    QJsonObject o;
    o["type"] = "endRound";
    o["sum0"] = sum[0];
    o["sum1"] = sum[1];
    o["sum2"] = sum[2];
    o["sum3"] = sum[3];
    o["sum4"] = sum[4];
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::randomFlop(int * arr)
{
    int i,j,ii;
    srand(time(NULL));
    for(i=11; i<=13; i++){
        replace2:;
        j = rand()%52+1;
        for(ii=1; ii<=13; ii++)
        {
            if(j == arr[ii]){goto replace2;}
        }
        arr[i] = j;
    }
    return;
}

void pokerserver::sendFlop(int tab, int hint)
{
    randomFlop(cards_number);

    round++;
    current = 0;

    QJsonDocument d;
    QJsonObject o;
    o["type"] = "flop";
    o["c1"] = cards_number[11];
    o["c2"] = cards_number[12];
    o["c3"] = cards_number[13];
    d.setObject(o);
    sendJsonDocument(0,tab,d);

    if(hint  == 0){
        nextHod();
        qDebug() << "nextHod" << hod;
        if(hod != 0){
           sendHod(tab, hod);
        }else{
            sendEndRound(tab);
            timer1_val = 2;
            timer1.start(1000);
        }
    }
}

void pokerserver::nextHod()
{
    hod = dealer - 1;
    for(int i = 1; i<=6; i++){
        if(hod == 0){hod = 5;}
        QTcpSocket* ss = sockets[hod];
        PosData* dd = table_1[ss];
        if(dd->pl == 1){
            hod = dd->pos;
            goto hh;
        }
        hod--;
    }
    hod = 0;
    hh:;
}

void pokerserver::sendTurn(int tab, int hint)
{
    round++;
    current = 0;

    int j,ii;
    srand(time(NULL));
    replace3:;
    j = rand()%52+1;
    for(ii=1; ii<=14; ii++)
    {
        if(j == cards_number[ii]){goto replace3;}
    }
    cards_number[14] = j;

    QJsonDocument d;
    QJsonObject o;
    o["type"] = "turn";
    o["c4"] = cards_number[14];
    d.setObject(o);
    sendJsonDocument(0,tab,d);

    if(hint == 0){
        nextHod();
        if(hod != 0){
            sendHod(tab, hod);
        }else{
            sendEndRound(tab);
            timer1_val = 3;
            timer1.start(1000);
        }
    }

}

void pokerserver::sendRiver(int tab,int hint)
{
    round++;
    current = 0;

    int j,ii;
    srand(time(NULL));
    replace4:;
    j = rand()%52+1;
    for(ii=1; ii<=15; ii++)
    {
        if(j == cards_number[ii]){goto replace4;}
    }
    cards_number[15] = j;

    QJsonDocument d;
    QJsonObject o;
    o["type"] = "river";
    o["c5"] = cards_number[15];
    d.setObject(o);
    sendJsonDocument(0,tab,d);

    if(hint == 0){
        nextHod();
        if(hod != 0){
            sendHod(tab, hod);
        }else{
            sendEndRound(tab);
            timer1_val = 4;
            timer1.start(1000);
        }
    }
}

void pokerserver::sendShowCards(int tab)
{
    QJsonDocument d;
    QJsonObject o;
    o["type"] = "showCards";
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::whoWin()
{
    for(int i=0; i<=5; i++){win[i] = 0;}

    int z = 0;
    int r = 0;
    QTcpSocket* ss;
    PosData* dd;
    foreach(ss, table_1.keys()){
        dd = table_1[ss];
        if((dd->pl > 0)or(dd->pl == -1)){
            z++;
            r = dd->pos;
        }
    }
    if(z == 1){win[r] = 1;}
    else{

    QString comb[6];

    QString alf[14];
    alf[0] = "a"; alf[1] = "b"; alf[2] = "c"; alf[3] = "d"; alf[4] = "e";
    alf[5] = "f"; alf[6] = "g"; alf[7] = "h"; alf[8] = "i"; alf[9] = "j";
    alf[10] = "k"; alf[11] = "l"; alf[12] = "m"; alf[13] = "n";

    int flush[6];
    int other[6];
    int cards[7];

    for(int i = 0; i<=6; i++){cards[i] = 0;}


    foreach(ss, table_1.keys()){
        dd = table_1[ss];
        if((dd->pl > 0) or (dd->pl == -1)){
            cards[0] = cards_number[2*dd->pos - 1];
            cards[1] = cards_number[2*dd->pos];
            cards[2] = cards_number[11];
            cards[3] = cards_number[12];
            cards[4] = cards_number[13];
            cards[5] = cards_number[14];
            cards[6] = cards_number[15];
        }else{
            comb[dd->pos] = "aaaaaa";

        }

            qDebug() << dd->pos << "cards" << cards[0] << cards[1] << cards[2] << cards[3] << cards[4] << cards[5] << cards[6];



        if(comb[dd->pos] != "aaaaaa"){

        checkFlush(cards,flush);
        qDebug() << "flush" << flush[0] << flush[1] << flush[2] << flush[3] << flush[4] << flush[5];

        int street = checkStreet(cards);
        qDebug() << "street" << street;


        checkOther(cards,other);
        qDebug() << "other" << other[0] << other[1] << other[2] << other[3] << other[4] << other[5];

        if((flush[0] == 1)and(flush[1]-flush[5] == 4)){
            comb[dd->pos] = alf[9] + alf[flush[1]] + "aaaa";
            qDebug() << dd->pos << comb[dd->pos];

        }else
        if(other[0] == 6){
            comb[dd->pos] = alf[8] + alf[other[1]] + alf[other[2]] + "aaa";
            qDebug() << dd->pos << comb[dd->pos];
        }else
        if(other[0] == 5){
            comb[dd->pos] = alf[7] + alf[other[1]] + alf[other[2]] + "aaa";
            qDebug() << dd->pos << comb[dd->pos];
        }else
        if(flush[0] == 1){
            comb[dd->pos] = alf[6] + alf[flush[1]] + alf[flush[2]] + alf[flush[3]] + alf[flush[4]] + alf[flush[5]];
            qDebug() << dd->pos << comb[dd->pos];
        }else
        if(street > 0){
            comb[dd->pos] = alf[5] + alf[street] + "aaaa";
            qDebug() << dd->pos << comb[dd->pos];
        }else{
        comb[dd->pos] = alf[other[0]] + alf[other[1]] + alf[other[2]] + alf[other[3]] + alf[other[4]] + alf[other[5]];
            qDebug() << dd->pos << comb[dd->pos];
        }
        }

    }

    int k = 1;
    QString maxi = "b";

    while(maxi != "a"){
        maxi = "a";

        for(int i = 1; i<= 5; i++){
            if((comb[i] > maxi)and(comb[i] != "aaaaaa")){
                maxi = comb[i];
            }
        }
        for(int i = 1; i<=5; i++){
            if(comb[i] == maxi){
                win[i] = k;
                comb[i] = "aaaaaa";
            }
        }
        k++;
    }


    }


    foreach(ss,table_1.keys()){
        dd = table_1[ss];
        if(dd->pl != -2){dd->pl = 1;}
    }

}

void pokerserver::sendWin(int tab,int* win)
{
    QJsonArray w0;
    QJsonArray w1;
    QJsonArray w2;
    QJsonArray w3;
    QJsonArray w4;

    QJsonArray arr[5] = {w0, w1, w2, w3, w4};

    for(int i = 0; i<=4; i++){
        Bank bbb = bank[i];
        int k;
        int max = 10;
        foreach(k, bbb.list){
            if((win[k] != 0)and(win[k] < max)){
                max = win[k];
            }
        }
        foreach(k, bbb.list){
            if(win[k] == max){
                arr[i].append(k);
            }
        }
    }

    for(int i = 0; i<=4; i++){
        if(arr[i].size() != 0){
            int k;
            QJsonValue v;
            foreach(v, arr[i]){
                k = v.toInt();
                QTcpSocket* ss = sockets[k];
                PosData* dd = table_1[ss];
                dd->stack += bank[i].sum/arr[i].size();
            }
            bank[i].sum = 0;
            bank[i].list.clear();
        }

    }

    QJsonDocument d;
    QJsonObject o;
    o["type"] = "win";
    o["win0"] = arr[0];
    o["win1"] = arr[1];
    o["win2"] = arr[2];
    o["win3"] = arr[3];
    o["win4"] = arr[4];
    d.setObject(o);
    sendJsonDocument(0,tab,d);
}

void pokerserver::nextPos()
{
    big_blind = newBlind;

    int i;
    QTcpSocket* ss;
    PosData* dd;
    foreach(ss,table_1.keys()){
        dd = table_1[ss];
        if(dd->pl != -2){dd->pl = 1;}
   }


    dealer --;
    for( i = 1; i<=5; i++){
        if(dealer == 0){dealer = 5;}
        ss = sockets[dealer];
        dd = table_1[ss];
        if(dd->pl == 1){
            dealer = dd->pos;
            goto deal;
        }
        dealer--;
    }
    deal:;

    mb --;
    for( i = 1; i<=5; i++){
        if(mb == 0){mb = 5;}
        ss = sockets[mb];
        dd = table_1[ss];
        if(dd->pl == 1){
            mb = dd->pos;
            goto mbb;
        }
        mb--;
    }
    mbb:;
    ss = sockets[mb];
    dd = table_1[ss];
    if(dd->stack > big_blind/2){
        dd->bet = big_blind/2;
        dd->stack -= big_blind/2;
    }else{
        dd->stack = big_blind/2;
        dd->stack = 0;
        dd->pl = -1;
    }


    bb --;
    for( i = 1; i<=5; i++){
        if(bb == 0){bb = 5;}
        ss = sockets[bb];
        dd = table_1[ss];
        if(dd->pl == 1){
            bb = dd->pos;
            goto bbb;
        }
        bb--;
    }
    bbb:;
    ss = sockets[bb];
    dd = table_1[ss];
    if(dd->stack > big_blind){
        dd->bet = big_blind;
        dd->stack -= big_blind;
    }else{
        dd->stack = big_blind;
        dd->stack = 0;
        dd->pl = -1;
    }

    qDebug() << hod << bb;
    hod = bb - 1;
    for(int i = 1; i<=6; i++){
        if(hod == 0){hod = 5;}
        ss = sockets[hod];
        dd = table_1[ss];
        if(dd->pl == 1){
            hod = dd->pos;
            goto hhh;
        }
        hod--;
    }
    hhh:;
    qDebug() << hod;
}

void pokerserver::onTimer1()
{
    timer1.stop();
    switch(timer1_val){
        case 1: sendFlop(1,0); break;
        case 2: sendTurn(1,0); break;
        case 3: sendRiver(1,0); break;
        case 4: sendShowCards(1);
        timer1_val = 5; timer1.start(2000);
        break;
        case 5: whoWin(); sendWin(1,win);
        timer1_val = 6; timer1.start(3000);
        break;
        case 6:
        nextPos();
        randomHands(cards_number);
        sendHands(1,cards_number[1],cards_number[2],cards_number[3],cards_number[4],cards_number[5],cards_number[6],cards_number[7],cards_number[8],cards_number[9],cards_number[10],t1_players);
        break;
    }
}

void pokerserver::onTimer2()
{
    timer2.stop();
    switch(timer2_val){
        case 1: sendFlop(1,1);
        timer2_val = 2; timer2.start(1000);
        break;
        case 2: sendTurn(1,1);
        timer2_val = 3; timer2.start(1000);
        break;
        case 3: sendRiver(1,1);
        timer2_val = 5; timer2.start(2000);
        break;
        case 5: whoWin(); sendWin(1,win);
        timer2_val = 6; timer2.start(3000);
        break;
        case 6:
        nextPos();
        randomHands(cards_number);
        sendHands(1,cards_number[1],cards_number[2],cards_number[3],cards_number[4],cards_number[5],cards_number[6],cards_number[7],cards_number[8],cards_number[9],cards_number[10],t1_players);
        break;
    }
}

void pokerserver::sendWinner(int tab)
{
    QTcpSocket* ss;
    PosData* dd;
    ConnData* cd;
    int i;
    for(i = 1; i<=5; i++){
        ss = sockets[i];
        dd = table_1[ss];
        cd = m_connections[ss];
        if(cd->table == tab){cd->table = 0;}

        if(dd->pl != -2){
            QJsonDocument d;
            QJsonObject o;
            o["type"] = "winner";
            o["name"] = cd->name;
            d.setObject(o);
            sendJsonDocument(0,0,d);
        }
    }

    table_1.clear();
    for(i = 0; i<=6; i++){
        sockets[i] = 0;
    }
    t1_players = 0;

    busy = false;
}

void pokerserver::checkFlush(int* arr, int* result) //OK
{
  int suits[5];
  QList<int> List;
  int i,k,j;
  j=1;

  for(i = 0; i<=5; i++){result[i] = 0;}
  for(i = 0; i<=4; i++){suits[i] = 0;}

  for(i = 0; i <= 6; i++){
        suits[arr[i]%4 +1]++;
        List.append(arr[i]);
  }
  for(i = 1; i<=4; i++){
      if(suits[i] >= 5){
          result[0] = 1;
          qSort(List);
          for(k=13; k>=0; k--){
              if(List.at(k)%4 +1 == i){
                  if(List.at(k)%4 == 1){result[j] = List.at(k)/4;}
                  else{
                  result[j] = (List.at(k)-List.at(k)%4 + 4)/4;
                  }
                  j++;
                  if(j == 6){return;}
              }
          }

      }
  }
  return;
}

int pokerserver::checkStreet(int* arr) //OK
{
    QList<int> List;
    int max = 0;
    int i;
    int ser = 0;
    int last = 0;
    for(i = 0; i<=6; i++){
        if(arr[i]%4 == 0){ List.append(arr[i]/4); }
        else{List.append((arr[i] - arr[i]%4 + 4)/4);}
    }
    qSort(List);

    if(List.at(6) == 13){ ser = 1; }

    for(i = 0; i<=6; i++){
        if(List.at(i) - last == 1){
            ser++;
            last = List.at(i);
            if(ser >= 5){ max = List.at(i);}

        }else if(List.at(i) - last == 0){}
        else{
            ser = 1;
            last = List.at(i);
        }
    }
    return max;
}

void pokerserver::checkOther(int* arr, int* result) //OK
{
    int num[14];
    int i, j, k;
    for(i = 0; i<=5; i++){ result[i] = 0;}
    for(i = 0; i<=13; i++){ num[i] = 0;}
    for(i = 0; i<=6; i++){
        if(arr[i]%4 == 0){ num[arr[i]/4]++; }
        else{ num[(arr[i] - arr[i]%4 + 4)/4]++; }
    }
    for(i = 1; i<=13; i++){
        if(num[i] == 4){
            result[0] = 6;
            result[1] = i;
            for(j = 13; j>=1; j--){
                if((num[j] > 0)and(num[j] != 4)){
                    result[2] = j;
                    goto comb;
                }
            }
        }
    }

    for(i = 13; i >=1; i--){
        if(num[i] == 3){
            result[1] = i;

            for(j = 13; j>=1; j--){
                if((num[j] > 1)and (j != i)){
                    result[2] = j;
                    result[0] = 5;
                    goto comb;
                }
            }
            for(j = 13; j>=1; j--){
                if((num[j] > 0)and (j != i)){
                    if(result[2] == 0){
                        result[2] = j;
                    }else{
                        result[3] = j;
                        goto comb;
                    }
                    result[0] = 4;
                }
            }
        }
    }
    for(i = 13; i>=1; i--){
        if(num[i] == 2){
            result[1] = i;
            for(j = 13; j>=1; j--){
                if((num[j] > 1)and(j != i)){
                    result[2] = j;
                    result[0] = 3;
                    for(k = 13; k>=1; k--){
                        if(num[k] == 1){
                            result[3] = k;
                            goto comb;
                        }
                    }
                }
            }
            for(j=13; j>=1; j--){
                if(num[j] == 1){
                    if(result[2] == 0){
                        result[2] = j;
                    }else if(result[3] == 0){
                        result[3] =j;
                    }else{
                        result[4] = j;
                        goto comb;
                    }
                }
                result[0] = 2;
            }
        }
    }
    k = 1;
    result[0] = 1;
    for(i = 13; i>=1; i--){
        if(num[i] == 1){
            if(k != 6){
                result[k] = i;
                k++;
            }
        }
    }
    comb:;
    return;
}

void pokerserver::sendTableBusy(QTcpSocket* sock)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "Busy";

    d.setObject(o);

    sendJsonDocument(sock,0,d);
}

void pokerserver::onBlindTimer()
{
    blind_timer.stop();
    switch(newBlind){
        case 40: newBlind = 80; break;
        case 80: newBlind = 150; break;
        case 150: newBlind = 300; break;
        case 300: newBlind = 500; break;
        case 500: newBlind = 750; break;
        case 750: newBlind = 1000; break;
        case 1000: newBlind = 1500; break;
        case 1500: newBlind = 2000; break;
    }
    blind_timer.start(420000);
}

void pokerserver::sendDisconnect(int tab, int pos)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "Disconnect";
    o["pos"] = pos;
    o["table"] = tab;

    d.setObject(o);
    sendJsonDocument(0,tab,d);
}




pokerserver::~pokerserver()
{

}
































