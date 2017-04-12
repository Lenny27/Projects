#ifndef ENTERWINDOW_H
#define ENTERWINDOW_H

#include "lobby.h"

#include <QDialog>


namespace Ui {
class EnterWindow;
}

class EnterWindow : public QDialog
{
    Q_OBJECT

    //Lobby* lob;

public:
    explicit EnterWindow(QWidget *parent = 0);
    ~EnterWindow();



    Ui::EnterWindow *ui;
private:


private slots:
    void OkEnabled();
//    void ExecEnter();

};

#endif // ENTERWINDOW_H
