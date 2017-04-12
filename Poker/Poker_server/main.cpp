#include "pokerserver.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    pokerserver *srv;

    srv = new pokerserver();

    return a.exec();
}

