#ifndef USERS_H
#define USERS_H

#include <QWidget>
#include <QString>
#include <QLabel>

class Users : public QWidget
{
    //Q_OBJECT

    QString user_name;
    QString picture;

    qreal t_x;
    qreal t_y;
    qreal t_w;
    qreal t_h;
   // qreal size;

    qreal m_x;
    qreal m_y;
    qreal m_w;
    qreal m_h;



public:
    //explicit Users(QWidget *parent = 0);
   Users(QWidget* wgt, QString str, QString n, QString m,int pl, qreal x, qreal y, qreal w, qreal h);

   QLabel* main_label;
   QLabel* name;
   QLabel* money;
   QLabel* blind;
   QLabel* dealer;

private:


signals:

public slots:
};

#endif // USERS_H
