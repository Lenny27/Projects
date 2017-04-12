#include "lobby.h"
#include "ui_lobby.h"

#include <QDebug>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonDocument>

Lobby::Lobby(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Lobby)
{
    ui->setupUi(this);
    ui->textEdit->setEnabled(false);
    ui->label->setPixmap(QPixmap(":/Images/lobby label.png").scaled(ui->label->width(),ui->label->height()));
    ui->textEdit_2->setEnabled(false);

//    this->setFixedSize(750, 400);
}

Lobby::~Lobby()
{
    delete ui;
}

