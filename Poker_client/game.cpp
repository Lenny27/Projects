#include "game.h"
#include "enterwindow.h"
#include "lobby.h"
#include "table.h"
#include "ui_enterwindow.h"
#include "ui_lobby.h"
#include "ui_table.h"
#include "dialog.h"
#include "ui_dialog.h"
#include "math.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QMessageBox>
#include <QFont>
#include <QPropertyAnimation>

Game::Game()
{


    m_socket = 0;
    number_t1 = 0;
    m_EntWin = new EnterWindow();
    qDebug() << "connect clicked" <<
                QObject::connect(m_EntWin->ui->OkButton,SIGNAL(clicked()),
                this, SLOT(onConnectButton()));
    message = "";

    m_EntWin->show();
}

void Game::onConnectButton()
{
    if( !m_socket) {
        serverConnect();
    }else{
        connectedSlot();
    }
}

void Game::serverConnect()
{
    m_socket = new QTcpSocket();


    qDebug() << "connectedSlot" << QObject::connect(m_socket, SIGNAL(connected()),
            this,SLOT(connectedSlot()));

    qDebug() << "readyReadSlot" << QObject::connect(m_socket, SIGNAL(readyRead()),
            this, SLOT(readyReadSlot()));
    qDebug() << "fold_timer" << QObject::connect(&fold_timer, SIGNAL(timeout()),
            this, SLOT(onFoldTimer()));
    qDebug() << "blinds_timer" << QObject::connect(&create_banks, SIGNAL(timeout()),
            this, SLOT(onBlindsTimer()));
    qDebug() << "banks_timer" << QObject::connect(&destroy_banks, SIGNAL(timeout()),
            this, SLOT(onBanksTimer()));


    m_socket->connectToHost( "127.0.0.1", 8888);
}

void Game::connectedSlot()
{
    qDebug() << "connected to server!!!";
    QString name = m_EntWin->ui->lineEdit->text();

    sendName(name);
}

void Game::sendName(QString &name)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "name";
    o["data"] = name;

    d.setObject(o);

    sendJsonDocument(d);
}

void Game::sendJsonDocument(QJsonDocument &d)
{
    QString s;

    s = d.toJson(QJsonDocument::Compact) + "\n";
    m_socket->write(s.toLocal8Bit());
}

void Game::readyReadSlot()
{
    QByteArray ba;
    ba = m_socket->readAll();
    QString s = QString::fromLocal8Bit(ba);

    for(int i = 0; i<s.length(); i++) {
        if( s[i] != '\n') {
            message.append(s[i]);
        } else {
            messageReady();
        }
    }
}

void Game::messageReady()
{
    QJsonDocument d;
    QJsonObject o;
    QString s = "";

    d = QJsonDocument::fromJson(message.toLocal8Bit());

    o = d.object();

    if( o["type"] == "new_user") {
       QString str = "";
       str = str + o["data"].toString();
       m_lobby->ui->textEdit->append(str);

    } else if (o["type"] == "action") {


    }else if (o["type"] == "BadName"){

        QMessageBox msgbox;
        msgbox.setText("Данное имя уже используется. Пожалуйста выбирете другое имя");
        msgbox.exec();

    }else if (o["type"] == "joinTable"){
        m_lobby->ui->textEdit_2->append(o["name"].toString());
        number_t1++;
        QFont font = m_lobby->ui->label_4->font();
        font.setPointSize(16);
        m_lobby->ui->label_4->setFont(font);

        switch(number_t1){
            case 0:
            m_lobby->ui->label_4->setText("0 из 5");
            break;
            case 1:
            m_lobby->ui->label_4->setText("1 из 5");
            break;
            case 2:
            m_lobby->ui->label_4->setText("2 из 5");
            break;
            case 3:
            m_lobby->ui->label_4->setText("3 из 5");
            break;
            case 4:
            m_lobby->ui->label_4->setText("4 из 5");
            break;
            case 5:
            m_lobby->ui->label_4->setText("5 из 5");
            break;
        }

    }else if (o["type"] == "GoodName"){
        my_name = o["data"].toString();
        m_EntWin->close();
        m_lobby = new Lobby();
        m_lobby->ui->label_5->setText(my_name);
        m_lobby->ui->label_5->setAlignment(Qt::AlignCenter);
        QFont font = m_lobby->ui->label_5->font();
        font.setPointSize(20);
        m_lobby->ui->label_5->setFont(font);
        m_lobby->show();
        qDebug() << "joinButton" << QObject::connect(m_lobby->ui->joinButton,SIGNAL(clicked()),
                                                      this, SLOT(sendJoinTable()));
        m_lobby->ui->cancelButton->setEnabled(false);
        qDebug() << "cancelButton" << QObject::connect(m_lobby->ui->cancelButton,SIGNAL(clicked()),
                                                      this, SLOT(sendCancelTable()));

    }else if (o["type"] == "Table"){
        m_lobby->ui->textEdit_2->append(o["player"].toString());

    }else if (o["type"] == "ClearList"){
        m_lobby->ui->textEdit->clear();

    }else if (o["type"] == "ClearTable"){
        m_lobby->ui->textEdit_2->clear();

    }else if(o["type"] == "BlockTable"){
        m_lobby->ui->cancelButton->setEnabled(false);
        m_lobby->ui->joinButton->setEnabled(false);
        m_lobby->ui->label_3->setText("Идет Игра");
        m_lobby->ui->label_3->setAlignment(Qt::AlignCenter);
        QFont font2 = m_lobby->ui->label_3->font();
        font2.setPointSize(14);
        m_lobby->ui->label_3->setFont(font2);
        m_lobby->ui->label_4->hide();

    }else if (o["type"] == "TableNumber"){
        number_t1 = o["number"].toInt();
        QFont font = m_lobby->ui->label_4->font();
        font.setPointSize(16);
        m_lobby->ui->label_4->setFont(font);

        switch(number_t1){
            case 0:
            m_lobby->ui->label_4->setText("0 из 5");
            break;
            case 1:
            m_lobby->ui->label_4->setText("1 из 5");
            break;
            case 2:
            m_lobby->ui->label_4->setText("2 из 5");
            break;
            case 3:
            m_lobby->ui->label_4->setText("3 из 5");
            break;
            case 4:
            m_lobby->ui->label_4->setText("4 из 5");
            break;
            case 5:
            m_lobby->ui->label_4->setText("5 из 5");
            break;
        }

    }else if(o["type"] == "Start"){
        qDebug() << "begin";
        QString s1 = o["p1"].toString();
        QString s2 = o["p2"].toString();
        QString s3 = o["p3"].toString();
        QString s4 = o["p4"].toString();
        QString s5 = o["p5"].toString();
        int num = o["number"].toInt();
        qDebug() << "before";
        m_table = new Table(0,s1,s2,s3,s4,s5,num,my_name);
        m_table->show();
        qDebug() << "after";

        rects[1].setRect(25,70,70,90);
        rects[2].setRect(95,70,70,90);
        rects[3].setRect(100,310,70,90);
        rects[4].setRect(170,310,70,90);
        rects[5].setRect(485,390,70,90);
        rects[6].setRect(555,390,70,90);
        rects[7].setRect(860,310,70,90);
        rects[8].setRect(930,310,70,90);
        rects[9].setRect(930,70,70,90);
        rects[10].setRect(1000,70,70,90);
        rects[11].setRect(360,170,70,90);
        rects[12].setRect(440,170,70,90);
        rects[13].setRect(520,170,70,90);
        rects[14].setRect(600,170,70,90);
        rects[15].setRect(680,170,70,90);

//        for(int i=0; i<=9; i++){
//            extraLabel[i] = 0;
//        }

        m_table->ui->groupBox->hide();

        for(int i = 1; i<= 5; i++){m_table->player_now[i] = 1;}

        qDebug() << "FoldButton" <<
                    QObject::connect(m_table->ui->FoldButton,SIGNAL(clicked()),this,SLOT(onFoldButton()));
        qDebug() << "CallButton" <<
                    QObject::connect(m_table->ui->CallButton,SIGNAL(clicked()),this,SLOT(onCallButton()));
        qDebug() << "RaiseButton" <<
                    QObject::connect(m_table->ui->RaiseButton,SIGNAL(clicked()),this,SLOT(onRaiseButton()));

//        m_dialog = new Dialog();
//        m_dialog->ui->label->setText("Добро пожаловать за стол! \n Игра начнется через 30 секунд.");
//        m_dialog->ui->label->setAlignment(Qt::AlignCenter);
//        QFont font = m_dialog->ui->label->font();
//        font.setPointSize(14);
//        m_dialog->ui->label->setFont(font);
//        m_dialog->show();

          t1_player = 5;


    }else if(o["type"] == "hands"){

        int b = o["blindValue"].toInt();
        m_table->big_blind = b;

        qDebug() << "tesst";
        int i;
        QJsonArray arr = o["data"].toArray();
        for(int i = 1; i<=5; i++){m_table->bets[i] = 0;}
        for(int i = 1; i<= 5; i++){
            if(m_table->player_now[i] != -2){
                m_table->player_now[i] = 1;
            }
        }
           m_table->current_bet = m_table->big_blind;

        for(i=0;i<16;i++){m_table->cards_number[i] = 0;}

        for( i=0; i<10; i++){
            qDebug() << arr[i].toInt();
            m_table->cards_number[i+1] = arr[i].toInt();
        }

        setHands(m_table->cards_number, m_table->my_position);

        int deal = o["deal"].toInt();
        for(int i = 1; i<=5; i++){
            if(i == deal){
                m_table->users[i]->dealer->show();
            }
            else{ m_table->users[i]->dealer->hide(); }
        }

        int mb = o["m_blind"].toInt();
        int bb = o["b_blind"].toInt();

        if(b/2 >= m_table->stacks[mb]){
            m_table->bets[mb] = m_table->stacks[mb];
            m_table->stacks[mb] = 0;
            m_table->player_now[mb] = -1;
        }else{
            m_table->bets[mb] = b/2;
            m_table->stacks[mb] -= b/2;
        }
        m_table->users[mb]->money->setText(QString::number(m_table->stacks[mb]));
        m_table->users[mb]->blind->setText(QString::number(m_table->bets[mb]));
        m_table->users[mb]->blind->show();

        if(b >= m_table->stacks[bb]){
            m_table->bets[bb] = m_table->stacks[bb];
            m_table->stacks[bb] = 0;
            m_table->player_now[bb] = -1;
        }else{
            m_table->bets[bb] = b;
            m_table->stacks[bb] -= b;
        }
        m_table->users[bb]->money->setText(QString::number(m_table->stacks[bb]));
        m_table->users[bb]->blind->setText(QString::number(m_table->bets[bb]));
        m_table->users[bb]->blind->show();


        int hod = o["hod"].toInt();
        if(hod == m_table->my_position){
            if(m_table->current_bet == m_table->bets[m_table->my_position]){
                m_table->ui->CallButton->setText("Check");
            }else{
                m_table->ui->CallButton->setText("Call");
            }
            m_table->ui->Slider->setMaximum(m_table->stacks[m_table->my_position]+m_table->bets[m_table->my_position]);
            m_table->ui->spinBox->setMaximum(m_table->stacks[m_table->my_position]+m_table->bets[m_table->my_position]);
            if(m_table->current_bet != 0){
                if(2*m_table->current_bet > m_table->stacks[hod]){
                    m_table->ui->Slider->setMinimum(m_table->stacks[hod]);
                    m_table->ui->spinBox->setMinimum(m_table->stacks[hod]);
                }else{
                    m_table->ui->Slider->setMinimum(2*m_table->current_bet);
                    m_table->ui->spinBox->setMinimum(2*m_table->current_bet);
                }
            }else{
                if(m_table->big_blind > m_table->stacks[hod]){
                    m_table->ui->Slider->setMinimum(m_table->stacks[hod]);
                    m_table->ui->spinBox->setMinimum(m_table->stacks[hod]);
                }else{
                    m_table->ui->Slider->setMinimum(m_table->big_blind);
                    m_table->ui->spinBox->setMinimum(m_table->big_blind);
                }
            }
            m_table->ui->groupBox->show();
            m_table->ui->Slider->setValue(m_table->ui->Slider->minimum());
            if(m_table->current_bet > m_table->stacks[hod]){
                m_table->ui->RaiseButton->setEnabled(false);
            }else{
                m_table->ui->RaiseButton->setEnabled(true);
            }
        }else{ m_table->ui->groupBox->hide(); }

        for(int i = 1; i<=5; i++){
            if(i == hod){
                m_table->users[i]->name->setStyleSheet("color: yellow");
                m_table->users[i]->money->setStyleSheet("color: yellow");
            }else{
            m_table->users[i]->name->setStyleSheet("color: white");
            m_table->users[i]->money->setStyleSheet("color: white");}
        }
        m_table->pot = m_table->big_blind * 3/ 2;
        qDebug() << "pot" << m_table->pot;
        m_table->ui->label_2->setText(QString::number(m_table->pot));

    }else if(o["type"] == "fold"){
        int p = o["pos"].toInt();
        m_table->player_now[p] = 0;
        m_table->ui->groupBox->hide();
        foldPos = p;

        QPropertyAnimation* anim = new QPropertyAnimation(m_table->table_cards[2*p-1]->card_label, "geometry");
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::Linear);
        anim->setEndValue(QRectF(520, 50, 70, 90));
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        QPropertyAnimation* anim1 = new QPropertyAnimation(m_table->table_cards[2*p]->card_label, "geometry");
        anim1->setDuration(500);
        anim1->setEasingCurve(QEasingCurve::Linear);
        anim1->setEndValue(QRectF(520, 50, 70, 90));
        anim1->start(QAbstractAnimation::DeleteWhenStopped);

        fold_timer.start(510);

    }else if(o["type"] == "call"){
        int p = o["pos"].toInt();
        m_table->player_now[p] = 2;
        if(m_table->stacks[p] + m_table->bets[p] > m_table->current_bet){
            m_table->stacks[p] -= m_table->current_bet - m_table->bets[p];
            m_table->pot += (m_table->current_bet - m_table->bets[p]);
            m_table->bets[p] = m_table->current_bet;
        }else{
            m_table->bets[p] = m_table->stacks[p] + m_table->bets[p];
            m_table->pot += m_table->stacks[p];
            m_table->stacks[p] = 0;
            m_table->player_now[p] = -1;
        }

        if(m_table->current_bet != 0){
        m_table->users[p]->money->setText(QString::number(m_table->stacks[p]));
        m_table->users[p]->blind->setText(QString::number(m_table->bets[p]));
        qDebug() << "show";
        m_table->users[p]->blind->show();
        }
        m_table->ui->groupBox->hide();
        m_table->ui->label_2->setText(QString::number((m_table->pot)));


    }else if(o["type"] == "raise"){
        int p = o["pos"].toInt();
        int v = o["value"].toInt();
        m_table->player_now[p] = 2;
        m_table->current_bet = v;
        m_table->stacks[p] -= m_table->current_bet - m_table->bets[p];
        m_table->pot = m_table->pot + m_table->current_bet - m_table->bets[p];
        m_table->bets[p] = m_table->current_bet;


        m_table->users[p]->money->setText(QString::number(m_table->stacks[p]));
        m_table->users[p]->blind->setText(QString::number(v));
        qDebug() << "show";
        m_table->users[p]->blind->show();

        if(m_table->stacks[p] == 0){
            m_table->player_now[p] = -1;
        }
        m_table->ui->groupBox->hide();
        m_table->ui->label_2->setText(QString::number(m_table->pot));

    }else if(o["type"] == "hod"){
        int hod = o["hod"].toInt();
        if(hod == m_table->my_position){
            if(m_table->current_bet == m_table->bets[m_table->my_position]){
                m_table->ui->CallButton->setText("Check");
            }else{
                m_table->ui->CallButton->setText("Call");
            }
            m_table->ui->Slider->setMaximum(m_table->stacks[m_table->my_position]+m_table->bets[m_table->my_position]);
            m_table->ui->spinBox->setMaximum(m_table->stacks[m_table->my_position]+m_table->bets[m_table->my_position]);
            if(m_table->current_bet != 0){
                if(2*m_table->current_bet > m_table->stacks[hod]){
                    m_table->ui->Slider->setMinimum(m_table->stacks[hod]);
                    m_table->ui->spinBox->setMinimum(m_table->stacks[hod]);
                }else{
                    m_table->ui->Slider->setMinimum(2*m_table->current_bet);
                    m_table->ui->spinBox->setMinimum(2*m_table->current_bet);
                }
            }else{
                if(m_table->big_blind> m_table->stacks[hod]){
                    m_table->ui->Slider->setMinimum(m_table->stacks[hod]);
                    m_table->ui->spinBox->setMinimum(m_table->stacks[hod]);
                }else{
                    m_table->ui->Slider->setMinimum(m_table->big_blind);
                    m_table->ui->spinBox->setMinimum(m_table->big_blind);
                }
            }
            m_table->ui->groupBox->show();
            m_table->ui->Slider->setValue(m_table->ui->Slider->minimum());
            if(m_table->current_bet >= m_table->stacks[hod] + m_table->bets[hod]){
                m_table->ui->RaiseButton->setEnabled(false);
            }else{
                m_table->ui->RaiseButton->setEnabled(true);
            }
        }else{ m_table->ui->groupBox->hide(); }

        for(int i = 1; i<=5; i++){
            if(i == hod){
                m_table->users[i]->name->setStyleSheet("color: yellow");
                m_table->users[i]->money->setStyleSheet("color: yellow");
            }else{
            m_table->users[i]->name->setStyleSheet("color: white");
            m_table->users[i]->money->setStyleSheet("color: white");}
        }

    }else if(o["type"] == "endRound"){
        int sum[5];
        sum[0] = o["sum0"].toInt();
        sum[1] = o["sum1"].toInt();
        sum[2] = o["sum2"].toInt();
        sum[3] = o["sum3"].toInt();
        sum[4] = o["sum4"].toInt();
        for(int i=0; i<=4; i++){
            m_table->bank1[i] += sum[i];
        }

        for(int i = 1; i <=5; i++){
            if(m_table->player_now[i] == 2){m_table->player_now[i] = 1;}
            m_table->bets[i] = 0;
        }
        m_table->round++;
        m_table->current_bet = 0;

        for(int i = 1; i<=5; i++){
            set_banks[i] = new QPropertyAnimation(m_table->users[i]->blind, "geometry");
            set_banks[i]->setDuration(350);
            set_banks[i]->setEasingCurve(QEasingCurve::Linear);
            set_banks[i]->setEndValue(QRect(530,275,50,25));
            set_banks[i]->start(QAbstractAnimation::DeleteWhenStopped);
        }
        create_banks.start(360);


    }else if(o["type"] == "flop"){
        int c1 = o["c1"].toInt();
        int c2 = o["c2"].toInt();
        int c3 = o["c3"].toInt();
        m_table->cards_number[11] = c1;
        m_table->cards_number[12] = c2;
        m_table->cards_number[13] = c3;

        m_table->table_cards[11]->card_label->setGeometry(QRect(520,50,70,90));
        m_table->table_cards[12]->card_label->setGeometry(QRect(520,50,70,90));
        m_table->table_cards[13]->card_label->setGeometry(QRect(520,50,70,90));

        m_table->table_cards[11]->card_label->setPixmap(QPixmap(card_str[c1]).scaled(70,90));
        m_table->table_cards[11]->card_label->show();
        m_table->table_cards[12]->card_label->setPixmap(QPixmap(card_str[c2]).scaled(70,90));
        m_table->table_cards[12]->card_label->show();
        m_table->table_cards[13]->card_label->setPixmap(QPixmap(card_str[c3]).scaled(70,90));
        m_table->table_cards[13]->card_label->show();

        for(int i = 11; i<=13; i++){
            set_hands_anim[i] = new QPropertyAnimation(m_table->table_cards[i]->card_label, "geometry");
            set_hands_anim[i]->setDuration(400);
            set_hands_anim[i]->setEasingCurve(QEasingCurve::Linear);
            set_hands_anim[i]->setEndValue(rects[i]);
            set_hands_anim[i]->start(QAbstractAnimation::DeleteWhenStopped);
        }

    }else if(o["type"] == "turn"){
        int c4 = o["c4"].toInt();
        m_table->cards_number[14] = c4;

        m_table->table_cards[14]->card_label->setGeometry(QRect(520,50,70,90));
        m_table->table_cards[14]->card_label->setPixmap(QPixmap(card_str[c4]).scaled(70,90));
        m_table->table_cards[14]->card_label->show();

        set_hands_anim[14] = new QPropertyAnimation(m_table->table_cards[14]->card_label, "geometry");
        set_hands_anim[14]->setDuration(400);
        set_hands_anim[14]->setEasingCurve(QEasingCurve::Linear);
        set_hands_anim[14]->setEndValue(rects[14]);
        set_hands_anim[14]->start(QAbstractAnimation::DeleteWhenStopped);

    }else if(o["type"] == "river"){
        int c5 = o["c5"].toInt();
        m_table->cards_number[15] = c5;

        m_table->table_cards[15]->card_label->setGeometry(QRect(520,50,70,90));
        m_table->table_cards[15]->card_label->setPixmap(QPixmap(card_str[c5]).scaled(70,90));
        m_table->table_cards[15]->card_label->show();

        set_hands_anim[15] = new QPropertyAnimation(m_table->table_cards[15]->card_label, "geometry");
        set_hands_anim[15]->setDuration(400);
        set_hands_anim[15]->setEasingCurve(QEasingCurve::Linear);
        set_hands_anim[15]->setEndValue(rects[15]);
        set_hands_anim[15]->start(QAbstractAnimation::DeleteWhenStopped);

    }else if(o["type"] == "showCards"){
        for(int i = 1; i <= 5; i++){
            if((m_table->player_now[i] > 0) or (m_table->player_now[i] == -1)){
                m_table->table_cards[2*i - 1]->card_label->setPixmap(QPixmap(card_str[m_table->cards_number[2*i-1]]).scaled(70,90));
                m_table->table_cards[2*i - 1]->card_label->show();
                m_table->table_cards[2*i]->card_label->setPixmap(QPixmap(card_str[m_table->cards_number[2*i]]).scaled(70,90));
                m_table->table_cards[2*i]->card_label->show();
            }
        }

    }else if(o["type"] == "win"){
        qDebug() << "i am win?";
        QJsonArray win[5];
        win[0] = o["win0"].toArray();
        win[1] = o["win1"].toArray();
        win[2] = o["win2"].toArray();
        win[3] = o["win3"].toArray();
        win[4] = o["win4"].toArray();

        int g;
        for(int i = 0; i<=4; i++){
            if(win[i].size() != 0){
                if(win[i].size() == 1){
                    drop_banks[2*i] = new QPropertyAnimation(m_table->bank[i], "geometry");
                    drop_banks[2*i]->setDuration(350);
                    drop_banks[2*i]->setEasingCurve(QEasingCurve::Linear);
                    g = win[i].at(0).toInt();
                    switch(g){
                        case 1: drop_banks[2*i]->setEndValue(QRect(240,190,50,25)); break;
                        case 2: drop_banks[2*i]->setEndValue(QRect(280,350,50,25)); break;
                        case 3: drop_banks[2*i]->setEndValue(QRect(525,360,50,25)); break;
                        case 4: drop_banks[2*i]->setEndValue(QRect(790,350,50,25)); break;
                        case 5: drop_banks[2*i]->setEndValue(QRect(835,190,50,25)); break;
                    }
                    drop_banks[2*i]->start(QAbstractAnimation::DeleteWhenStopped);
                }
                if(win[i].size() == 2){
                    m_table->bank[i]->hide();

//                    extraLabel[2*i] = new QLabel(m_table);
//                    extraLabel[2*i]->setAlignment(Qt::AlignCenter);
//                    extraLabel[2*i]->setStyleSheet("background-color: yellow");
//                    QFont fontb1 = extraLabel[2*i]->font();
//                    fontb1.setPointSize(14);
//                    extraLabel[2*i]->setFont(fontb1);
//                    extraLabel[2*i]->setGeometry(m_table->bank[i]->geometry());
                    m_table->extraLabel[2*i]->setText(QString::number(m_table->bank1[i]/2));
                    m_table->extraLabel[2*i]->show();

//                    extraLabel[2*i+1] = new QLabel(m_table);
//                    extraLabel[2*i+1]->setAlignment(Qt::AlignCenter);
//                    extraLabel[2*i+1]->setStyleSheet("background-color: yellow");
//                    QFont fontb2 = extraLabel[2*i]->font();
//                    fontb2.setPointSize(14);
//                    extraLabel[2*i+1]->setFont(fontb2);
//                    extraLabel[2*i+1]->setGeometry(m_table->bank[i]->geometry());
                    m_table->extraLabel[2*i+1]->setText(QString::number(m_table->bank1[i]/2));
                    m_table->extraLabel[2*i+1]->show();

                    drop_banks[2*i] = new QPropertyAnimation(m_table->extraLabel[2*i], "geometry");
                    drop_banks[2*i]->setDuration(350);
                    drop_banks[2*i]->setEasingCurve(QEasingCurve::Linear);
                    g = win[i].at(0).toInt();
                    switch(g){
                        case 1: drop_banks[2*i]->setEndValue(QRect(240,190,50,25)); break;
                        case 2: drop_banks[2*i]->setEndValue(QRect(280,350,50,25)); break;
                        case 3: drop_banks[2*i]->setEndValue(QRect(525,360,50,25)); break;
                        case 4: drop_banks[2*i]->setEndValue(QRect(790,350,50,25)); break;
                        case 5: drop_banks[2*i]->setEndValue(QRect(835,190,50,25)); break;
                    }
                    drop_banks[2*i]->start(QAbstractAnimation::DeleteWhenStopped);

                    drop_banks[2*i+1] = new QPropertyAnimation(m_table->extraLabel[2*i+1], "geometry");
                    drop_banks[2*i+1]->setDuration(350);
                    drop_banks[2*i+1]->setEasingCurve(QEasingCurve::Linear);
                    g = win[i].at(1).toInt();
                    switch(g){
                        case 1: drop_banks[2*i+1]->setEndValue(QRect(240,190,50,25)); break;
                        case 2: drop_banks[2*i+1]->setEndValue(QRect(280,350,50,25)); break;
                        case 3: drop_banks[2*i+1]->setEndValue(QRect(525,360,50,25)); break;
                        case 4: drop_banks[2*i+1]->setEndValue(QRect(790,350,50,25)); break;
                        case 5: drop_banks[2*i+1]->setEndValue(QRect(835,190,50,25)); break;
                    }
                    drop_banks[2*i+1]->start(QAbstractAnimation::DeleteWhenStopped);
                }
            }

        }
        destroy_banks.start(370);


        for(int i=0; i<=4; i++){
            if(win[i].size() != 0){
                int k;
                QJsonValue v;
                foreach(v, win[i]){
                    k = v.toInt();
                    m_table->stacks[k] += m_table->bank1[i]/win[i].size();
                }
            }
            m_table->bank1[i] = 0;
//            m_table->bank[i]->hide();
        }
//        for(int i = 1; i<=5; i++){
//            m_table->users[i]->money->setText(QString::number(m_table->stacks[i]));
//        }

        m_table->round = 1;

//        for(int i = 1; i<=15;i++){
//            m_table->table_cards[i]->card_label->hide();
//        }
//        for(int ii = 1; ii<=5; ii++){
//            m_table->users[ii]->blind->hide();
//            if((m_table->stacks[ii] == 0)and(m_table->player_now[ii] != -2)){
//                m_table->users[ii]->main_label->hide();
//                m_table->users[ii]->name->hide();
//                m_table->users[ii]->money->hide();
//                m_table->player_now[ii] = -2;

//                if(ii == m_table->my_position){
//                    sendLose(ii);
//                    m_table->close();
//                    m_dialog = new Dialog();
//                    m_dialog->ui->label->setAlignment(Qt::AlignCenter);
//                    QFont font = m_dialog->ui->label->font();
//                    font.setPointSize(14);
//                    m_dialog->ui->label->setFont(font);
//                    switch(t1_player){
//                        case 5: m_dialog->ui->label->setText("Вы заняли 5-ое место"); break;
//                        case 4: m_dialog->ui->label->setText("Вы заняли 4-ое место"); break;
//                        case 3: m_dialog->ui->label->setText("Вы заняли 3-ое место"); break;
//                        case 2: m_dialog->ui->label->setText("Вы заняли 2-ое место"); break;
//                    }
//                    m_dialog->show();
//                }
//                qDebug() << "pl" << t1_player;
//                t1_player--;
//            }
//        }
//        m_table->pot = 0;
//        m_table->ui->label_2->setText(QString::number(m_table->pot));

    }else if(o["type"] == "winner"){
        QString str = o["name"].toString();
        if(my_name == str){
            qDebug() << "similar" << str << my_name;
            m_table->close();
            m_dialog = new Dialog();
            m_dialog->ui->label->setAlignment(Qt::AlignCenter);
            QFont fontt = m_dialog->ui->label->font();
            fontt.setPointSize(14);
            m_dialog->ui->label->setFont(fontt);
            m_dialog->ui->label->setText("Поздравляем!!! \n Вы заняли 1-ое место");
            m_dialog->show();
        }
        m_lobby->ui->label_3->setText("Свободные места:");
        number_t1 = 0;
        t1_player = 0;
        m_lobby->ui->label_4->setText("0 из 5");
        m_lobby->ui->label_4->show();
        m_lobby->ui->textEdit_2->clear();
        m_lobby->ui->cancelButton->setEnabled(false);
        m_lobby->ui->joinButton->setEnabled(true);

    }else if(o["type"] == "Busy"){
        m_lobby->ui->cancelButton->setEnabled(false);
        m_lobby->ui->joinButton->setEnabled(false);
        m_lobby->ui->label_3->setText("Идет Игра");
        m_lobby->ui->label_3->setAlignment(Qt::AlignCenter);
        QFont font2 = m_lobby->ui->label_3->font();
        font2.setPointSize(14);
        m_lobby->ui->label_3->setFont(font2);
        m_lobby->ui->label_4->hide();

    }else if(o["type"] == "Disconnect"){
        int pos = o["pos"].toInt();
        int t = o["table"].toInt();
        m_table->player_now[pos] = -2;
        m_table->stacks[pos] = 0;
        m_table->table_cards[2*pos]->card_label->hide();
        m_table->table_cards[2*pos - 1]->card_label->hide();
        m_table->users[pos]->main_label->hide();
        m_table->users[pos]->money->hide();
        m_table->users[pos]->name->hide();
    }






    message = "";

}

void Game::sendAction(QString &msg, int val, QString &from) {
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "action";
    o["data"] = msg;
    o["size"] = val;
    o["from"] = from;

    d.setObject(o);

    sendJsonDocument(d);
}

void Game::sendJoinTable()
{
    m_lobby->ui->joinButton->setEnabled(false);
    m_lobby->ui->cancelButton->setEnabled(true);

    QJsonDocument d;
    QJsonObject o;

    o["type"] = "joinTable";
    o["table"] = 1;
    o["name"] = my_name;

    d.setObject(o);

    sendJsonDocument(d);

    if(number_t1+1 == 5 )
    {
        QJsonDocument doc;
        QJsonObject ob;
        ob["type"] = "startGame";
        ob["table"] = 1;
        doc.setObject(ob);
        sendJsonDocument(doc);
    }
}

void Game::sendCancelTable()
{
    m_lobby->ui->joinButton->setEnabled(true);
    m_lobby->ui->cancelButton->setEnabled(false);

    QJsonDocument d;
    QJsonObject o;

    o["type"] = "cancelTable";
    o["table"] = 1;
    o["name"] = my_name;

    d.setObject(o);

    sendJsonDocument(d);
}

void Game::setHands(int* arr, int my_num)
{
    for(int i = 1; i<=10; i++){
        if(arr[i] != -1){
            m_table->table_cards[i]->card_label->setGeometry(QRect(520,50,70,90));
            if((2*my_num - 1 == i) or (2*my_num == i)){
                m_table->table_cards[i]->card_label->setPixmap(QPixmap(card_str[arr[i]]).scaled(70,90));
            }else{
                 m_table->table_cards[i]->card_label->setPixmap(QPixmap(card_str[0]).scaled(70,90));
            }
            m_table->table_cards[i]->card_label->show();

            set_hands_anim[i] = new QPropertyAnimation(m_table->table_cards[i]->card_label, "geometry");
            set_hands_anim[i]->setDuration(500);
            set_hands_anim[i]->setEasingCurve(QEasingCurve::Linear);
            set_hands_anim[i]->setEndValue(rects[i]);
            set_hands_anim[i]->start(QAbstractAnimation::DeleteWhenStopped);
        }

    }
}

void Game::onCallButton()
{
    qDebug() << "i am call";
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "call";
    o["table"] = 1;
    o["pos"] = m_table->my_position;
    o["round"] = m_table->round;
    o["current"] = m_table->current_bet;

    d.setObject(o);
    sendJsonDocument(d);
}

void Game::onFoldButton()
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "fold";
    o["table"] = 1;
    o["pos"] = m_table->my_position;
    o["round"] = m_table->round;
    o["current"] = m_table->current_bet;

    d.setObject(o);
    sendJsonDocument(d);
}

void Game::onRaiseButton()
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "raise";
    o["table"] = 1;
    o["pos"] = m_table->my_position;
    o["value"] = m_table->ui->Slider->value();
    o["round"] = m_table->round;
    o["current"] = m_table->current_bet;

    d.setObject(o);
    sendJsonDocument(d);
}

void Game::sendLose(int p)
{
    QJsonDocument d;
    QJsonObject o;

    o["type"] = "lose";
    o["pos"] = p;

    d.setObject(o);
    sendJsonDocument(d);
}

void Game::onFoldTimer()
{
    fold_timer.stop();
    m_table->table_cards[2*foldPos-1]->card_label->hide();
    m_table->table_cards[2*foldPos]->card_label->hide();
    m_table->table_cards[2*foldPos-1]->card_label->setGeometry(rects[2*foldPos-1]);
    m_table->table_cards[2*foldPos]->card_label->setGeometry(rects[2*foldPos]);
}

void Game::onBlindsTimer()
{
    create_banks.stop();
    for(int i = 1; i <=5; i++){
        m_table->users[i]->blind->hide();
    }

    m_table->users[1]->blind->setGeometry(QRect(240,190,50,25));
    m_table->users[2]->blind->setGeometry(QRect(280,350,50,25));
    m_table->users[3]->blind->setGeometry(QRect(525,360,50,25));
    m_table->users[4]->blind->setGeometry(QRect(790,350,50,25));
    m_table->users[5]->blind->setGeometry(QRect(835,190,50,25));

    for(int i=0; i<=4; i++){
        if(m_table->bank1[i] != 0){
            m_table->bank[i]->setText(QString::number(m_table->bank1[i]));
            m_table->bank[i]->show();
        }
    }


}

void Game::onBanksTimer()
{
    destroy_banks.stop();
    for(int i=0; i<=4; i++){
        m_table->bank[i]->hide();
    }
    for(int i=0; i<=9; i++){
        m_table->extraLabel[i]->hide();
        m_table->extraLabel[i]->setGeometry(m_table->bank[0]->geometry());
    }
    m_table->bank[0]->setGeometry(530,275,50,25);
    m_table->bank[1]->setGeometry(600,275,50,25);
    m_table->bank[2]->setGeometry(460,275,50,25);
    m_table->bank[3]->setGeometry(670,275,50,25);
    m_table->bank[4]->setGeometry(390,275,50,25);

    for(int i = 1; i<=15;i++){
        m_table->table_cards[i]->card_label->hide();
    }
    for(int i = 1; i<=5; i++){
        m_table->users[i]->money->setText(QString::number(m_table->stacks[i]));
    }

    for(int ii = 1; ii<=5; ii++){
        m_table->users[ii]->blind->hide();
        if((m_table->stacks[ii] == 0)and(m_table->player_now[ii] != -2)){
            m_table->users[ii]->main_label->hide();
            m_table->users[ii]->name->hide();
            m_table->users[ii]->money->hide();
            m_table->player_now[ii] = -2;

            if(ii == m_table->my_position){
                sendLose(ii);
                m_table->close();
                m_dialog = new Dialog();
                m_dialog->ui->label->setAlignment(Qt::AlignCenter);
                QFont font = m_dialog->ui->label->font();
                font.setPointSize(14);
                m_dialog->ui->label->setFont(font);
                switch(t1_player){
                    case 5: m_dialog->ui->label->setText("Вы заняли 5-ое место"); break;
                    case 4: m_dialog->ui->label->setText("Вы заняли 4-ое место"); break;
                    case 3: m_dialog->ui->label->setText("Вы заняли 3-ое место"); break;
                    case 2: m_dialog->ui->label->setText("Вы заняли 2-ое место"); break;
                }
                m_dialog->show();
            }
            t1_player--;
        }
    }
    m_table->pot = 0;
    m_table->ui->label_2->setText(QString::number(m_table->pot));
}

//Game::~Game()
//{

//}



































