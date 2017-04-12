#ifndef CARD_H
#define CARD_H

#include <QWidget>
#include <QGraphicsItem>
#include <QPainter>

namespace Ui {
class Card;
}

class Card : public QGraphicsItem
{
    qreal m_radius;
    qreal m_x;
    qreal m_y;
    qreal m_vx;
    qreal m_vy;




};

#endif // CARD_H
