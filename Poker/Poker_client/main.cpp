#include "mainwindow.h"
#include "enterwindow.h"
#include "table.h"
#include "lobby.h"
#include "game.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
     QApplication a(argc, argv);

   Game* g = new Game();

//     QString str = "YTbjhbivbsi3875";

//    Table* t = new Table(0,str,str,str,str,str, 1, str );
//    t->show();




   return a.exec();
}
