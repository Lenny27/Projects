#include "enterwindow.h"
#include "ui_enterwindow.h"
#include "mainwindow.h"
#include "lobby.h"
#include "ui_lobby.h"

#include <QMessageBox>

EnterWindow::EnterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterWindow)
{
    ui->setupUi(this);
    ui->OkButton->setEnabled(false);

    QRegExp exp("[a-zA-Z0-9]{3,20}");
    ui->lineEdit->setValidator(new QRegExpValidator(exp, this));

    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(OkEnabled()));
    //connect(ui->OkButton,SIGNAL(clicked()),this,SLOT(ExecEnter()));

    this->setFixedSize(400,200);

}

//void EnterWindow::ExecEnter()
//{


//   Lobby *lobby = new Lobby;
//   lobby->ui->label->setText(ui->lineEdit->text());
//   lobby->show();


//    QMessageBox msg;
//    msg.setText(ui->lineEdit->text());
//    msg.exec();
//}

void EnterWindow::OkEnabled()
{
    ui->OkButton->setEnabled(ui->lineEdit->hasAcceptableInput());
}

EnterWindow::~EnterWindow()
{
    delete ui;
}
