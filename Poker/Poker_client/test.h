#ifndef TEST
#define TEST

#include <QObject>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QGraphicsScene *scene, QWidget *parent = 0) : QGraphicsView(scene, parent)
    {
    }

    virtual void resizeEvent(QResizeEvent *event)
    {
        fitInView(sceneRect(), Qt::KeepAspectRatio);
    }
};


void createStates(const QObjectList &objects,
                  const QRect &selectedRect, QState *parent)
{
    for (int i = 0; i < objects.size(); ++i) {
        QState *state = new QState(parent);
        state->assignProperty(objects.at(i), "geometry", selectedRect);
        parent->addTransition(objects.at(i), SIGNAL(clicked()), state);
    }
}

void createAnimations(const QObjectList &objects, QStateMachine *machine)
{
    for (int i=0; i<objects.size(); ++i)
        machine->addDefaultAnimation(new QPropertyAnimation(objects.at(i), "geometry"));
}

class Pixmap : public QGraphicsWidget
{
    Q_OBJECT

public:
    Pixmap(const QPixmap &pix, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), orig(pix), p(pix)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->drawPixmap(QPointF(), p);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * )
    {
        emit clicked();
    }

    virtual void setGeometry(const QRectF &rect)
    {
        QGraphicsWidget::setGeometry(rect);

        if (rect.size().width() > orig.size().width())
            p = orig.scaled(rect.size().toSize());
        else
            p = orig;
    }

Q_SIGNALS:
    void clicked();

private:
    QPixmap orig;
    QPixmap p;
};

#endif // TEST

