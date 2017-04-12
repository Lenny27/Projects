#include "users.h"

#include <QFont>

Users::  Users(QWidget* wgt, QString str, QString n, QString m, int pl, qreal x, qreal y, qreal w, qreal h)
{
    main_label = new QLabel(wgt);
    main_label->setPixmap(QPixmap(str).scaled(w,h));
    main_label->setGeometry(x,y,w,h);

    name = new QLabel(wgt);
    name->setText(n);
    name->setStyleSheet("color: rgb(255,255,255)");
    name->setAlignment(Qt::AlignCenter);
    name->setGeometry(x+w/8, y+h/8, 6*w/8, 3*h/8);
    QFont font1 = name->font();
    font1.setPointSize(14);
    name->setFont(font1);

    money = new QLabel(wgt);
    money->setText(m);
    money->setStyleSheet("color: rgb(255,255,255)");
    money->setAlignment(Qt::AlignCenter);
    money->setGeometry(x+w/8, y+4*h/8, 6*w/8, 3*h/8 );
    QFont font2 = money->font();
    font2.setPointSize(14);
    money->setFont(font2);

    blind = new QLabel(wgt);
    blind->setAlignment(Qt::AlignCenter);
//    blind->setStyleSheet("color: rgb(255,255,255)");
    QFont font3 = blind->font();
    font3.setPointSize(14);
    blind->setFont(font3);
    blind->setStyleSheet("background-color: yellow");

    dealer = new QLabel(wgt);
    dealer->setPixmap(QPixmap(":/Images/D.png").scaled(30,30));

    if(pl == 1){
        blind->setGeometry(x+w+50,y+30,50,25);
        dealer->setGeometry(200,230,30,30);
    }
    if(pl == 2){
        blind->setGeometry(x+w+20,y-50,50,25);
        dealer->setGeometry(320,390,30,30);
    }
    if(pl == 3){
        blind->setGeometry(x+65,y-120,50,25);
        dealer->setGeometry(625,390,30,30);
    }
    if(pl == 4){
        blind->setGeometry(x-50,y-50,50,25);
        dealer->setGeometry(800,380,30,30);
    }
    if(pl == 5){
        blind->setGeometry(x-75,y+30,50,25);
        dealer->setGeometry(880,220,30,30);
    }



    main_label->show();
    name->show();
    money->show();
    blind->hide();
    dealer->hide();
}

