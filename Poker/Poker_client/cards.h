#ifndef CARDS_H
#define CARDS_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QRect>


class Cards : public QWidget
{
    //Q_OBJECT

public:

    int number;

    QRect rects[16];

    QString card_str[54] = {":/Images/reverse_card.png", ":/Images/2piki.png", ":/Images/2cher.png", ":/Images/2krest.png", ":/Images/2bubi.png",
                            ":/Images/3piki.png", ":/Images/3cher.png", ":/Images/3krest.png", ":/Images/3bubi.png",
                            ":/Images/4piki.png", ":/Images/4cher.png", ":/Images/4krest.png", ":/Images/4bubi.png",
                            ":/Images/5piki.png", ":/Images/5cher.png", ":/Images/5krest.png", ":/Images/5bubi.png",
                            ":/Images/6piki.png", ":/Images/6cher.png", ":/Images/6krest.png", ":/Images/6bubi.png",
                            ":/Images/7piki.png", ":/Images/7cher.png", ":/Images/7krest.png", ":/Images/7bubi.png",
                            ":/Images/8piki.png", ":/Images/8cher.png", ":/Images/8krest.png", ":/Images/8bubi.png",
                            ":/Images/9piki.png", ":/Images/9cher.png", ":/Images/9krest.png", ":/Images/9bubi.png",
                            ":/Images/10piki.png", ":/Images/10cher.png", ":/Images/10krest.png", ":/Images/10bubi.png",
                            ":/Images/Jpiki.png", ":/Images/Jcher.png", ":/Images/Jkrest.png", ":/Images/Jbubi.png",
                            ":/Images/Qpiki.png", ":/Images/Qcher.png", ":/Images/Qkrest.png", ":/Images/Qbubi.png",
                            ":/Images/Kpiki.png", ":/Images/Kcher.png", ":/Images/Kkrest.png", ":/Images/Kbubi.png",
                            ":/Images/Apiki.png", ":/Images/Acher.png", ":/Images/Akrest.png", ":/Images/Abubi.png"
                           };



    Cards(QWidget* wgt, int n, int p);


    QLabel* card_label;
signals:

public slots:
};

#endif // CARDS_H
