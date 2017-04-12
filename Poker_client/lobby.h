#ifndef LOBBY_H
#define LOBBY_H

#include "enterwindow.h"

#include <QMainWindow>
#include <QList>
#include <QString>
#include <QLabel>

namespace Ui {
class Lobby;
}

class Lobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit Lobby(QWidget *parent = 0);
    ~Lobby();

    QString my_name;
    QList<QString> user_list;




     Ui::Lobby *ui;



private:


};

#endif // LOBBY_H
