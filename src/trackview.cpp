#include "trackview.h"

#include "clipview.h"
#include <QPainter>
TrackView::TrackView(int number,int width, int height,QWidget *parent)
    : QFrame{parent}, m_number(number), m_width(width), m_height(height)
{
    QRect rect(0,0,width,height);
    setGeometry(rect);
    setFrameStyle(QFrame::Box);
    setLineWidth(1);
}

void TrackView::paintEvent(QPaintEvent *event)
{

    QPainter* painter = new QPainter(this);
    painter->save();
    QBrush brush = QBrush(Qt::lightGray);

    QPen pen = QPen();
    pen.setColor(Qt::black);

    pen.setWidth(2);


    painter->setPen(Qt::PenStyle::NoPen);
    //
    painter->setBrush(brush);
    //painter->setBrush(Qt::NoBrush);


    painter->drawRect(geometry());
    painter->restore();
    delete painter;

    QFrame::paintEvent(event);


}
