#include "cards.h"

Cards::Cards(QWidget* wgt, int n, int p)
{
    rects[1].setRect(25,70,70,90);
    rects[2].setRect(95,70,70,90);
    rects[3].setRect(100,310,70,90);
    rects[4].setRect(170,310,70,90);
    rects[5].setRect(485,390,70,90);
    rects[6].setRect(555,390,70,90);
    rects[7].setRect(860,310,70,90);
    rects[8].setRect(930,310,70,90);
    rects[9].setRect(930,70,70,90);
    rects[10].setRect(1000,70,70,90);
    rects[11].setRect(360,170,70,90);
    rects[12].setRect(440,170,70,90);
    rects[13].setRect(520,170,70,90);
    rects[14].setRect(600,170,70,90);
    rects[15].setRect(680,170,70,90);

    number = n;
    card_label = new QLabel(wgt);
    card_label->setPixmap(QPixmap(card_str[n]).scaled(70,90));

    card_label->setGeometry(rects[p]);
    card_label->hide();

}

