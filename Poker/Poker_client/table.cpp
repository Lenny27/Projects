#include "table.h"
#include "ui_table.h"
#include "users.h"
#include "cards.h"
#include <time.h>

#include <QObject>
#include <QBrush>
#include <QImage>
#include <QPalette>
#include <Qlabel>
#include <QRectF>
#include <QPropertyAnimation>
#include <QTimer>
#include <QDebug>

void setBackground(QWidget * wgt)
{
    wgt->setFixedSize(1100, 650);
    QBrush br(Qt::TexturePattern);
    br.setTextureImage(QImage(":/Images/t1.png"));
    QPalette plt = wgt->palette();
    plt.setBrush(QPalette::Background, br);
    wgt->setPalette(plt);
}

Table::Table(QWidget *parent, QString &p1,QString &p2,
             QString &p3, QString &p4, QString &p5, int pos,
             QString &name)
    :
    QWidget(parent),
    ui(new Ui::Table)
{
    ui->setupUi(this);
    setBackground(this);
    big_blind = 40;
    round = 1;
    ui->label_2->setText(QString::number(pot));
    bank1[0] = 0;
    bank1[1] = 0;
    bank1[2] = 0;
    bank1[3] = 0;
    bank1[4] = 0;
    current_bet = big_blind;
    my_position = pos;
    pot = 0;
    stacks[1] = 1500;   bets[1] = 0;
    stacks[2] = 1500;   bets[2] = 0;
    stacks[3] = 1500;   bets[3] = 0;
    stacks[4] = 1500;   bets[4] = 0;
    stacks[5] = 1500;   bets[5] = 0;
    names[pos] = name;
    setPlayers(this, p1,p2,p3,p4,p5);
    setCards(this);

    for(int i = 0; i <= 15; i++){cards_number[i] = 0;}

    for(int i = 0; i <=4; i++ ){
        bank[i] = new QLabel(this);
        bank[i]->setAlignment(Qt::AlignCenter);
        QFont font3 = bank[0]->font();
        font3.setPointSize(14);
        bank[i]->setFont(font3);
        bank[i]->setStyleSheet("background-color: yellow");
        switch(i){
            case 0: bank[i]->setGeometry(530,275,50,25); break;
            case 1: bank[i]->setGeometry(600,275,50,25); break;
            case 2: bank[i]->setGeometry(460,275,50,25); break;
            case 3: bank[i]->setGeometry(670,275,50,25); break;
            case 4: bank[i]->setGeometry(390,275,50,25); break;
        }
        bank[i]->hide();
    }

    ui->label->setStyleSheet("color: rgb(255,255,255)");
    ui->label_2->setStyleSheet("color: rgb(255,255,255)");
    ui->label_2->setAlignment(Qt::AlignCenter);
    QFont font11 = ui->label_2->font();
    font11.setPointSize(14);
    ui->label_2->setFont(font11);


    for(int i = 0; i<=9; i++){
        extraLabel[i] = new QLabel(this);
        extraLabel[i]->setAlignment(Qt::AlignCenter);
        extraLabel[i]->setStyleSheet("background-color: yellow");
        QFont fontb1 = extraLabel[i]->font();
        fontb1.setPointSize(14);
        extraLabel[i]->setFont(fontb1);
        extraLabel[i]->setGeometry(bank[0]->geometry());
        extraLabel[i]->hide();
    }



    ui->Slider->setMaximum(stacks[my_position]);
    ui->spinBox->setMaximum(stacks[my_position]);
    ui->Slider->setMinimum(2*current_bet);
    ui->spinBox->setMinimum(2*current_bet);

    qDebug() << "slider" <<
                QObject::connect(ui->Slider,SIGNAL(valueChanged(int)),ui->spinBox,SLOT(setValue(int)));

    qDebug() << "spinbox" <<
                QObject::connect(ui->spinBox,SIGNAL(valueChanged(int)),ui->Slider,SLOT(setValue(int)));







}


void Table::setPlayers(QWidget* wgt, QString &p1,QString &p2,
                       QString &p3, QString &p4, QString &p5)
{
    for(int i = 1; i<=5; i++)
    {
        switch (i)
        {
            case 1:
            users[i] = new Users(wgt, ":/Images/user1.png", p1, "1460",1,10,160,180,80);
            break;
            case 2:
            users[i] = new Users(wgt, ":/Images/user1.png", p2, "1480",2,80,400,180,80);
            break;
            case 3:
            users[i] = new Users(wgt, ":/Images/user1.png", p3, "1500",3,460,480,180,80);
            break;
            case 4:
            users[i] = new Users(wgt, ":/Images/user1.png", p4, "1500",4,840,400,180,80);
            break;
            case 5:
            users[i] = new Users(wgt, ":/Images/user1.png", p5, "1500",5,910,160,180,80);
            break;
        }
    }
}

void Table::setCards(QWidget* wgt)
{
    for(int i = 1; i < 16; i++ ){
         table_cards[i] = new Cards(wgt, 0, i);
    }
}




Table::~Table()
{
    delete ui;
}





