#include "clipview.h"

#include <QBrush>
#include <QPainter>
#include <QPen>

ClipView::ClipView(QWidget *parent)
    : QWidget{parent}
{

    rect = QRect(0,0,200,30);
    setGeometry(rect);
    m_name = "clip";
}

void ClipView::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    //Q_UNUSED(option);Q_UNUSED(widget);
    QPainter* painter = new QPainter(this);
    painter->save();
    QBrush brush = QBrush(Qt::white);

    QPen pen = QPen();
    pen.setColor(Qt::darkGray);

    pen.setWidth(2);


    painter->setPen(Qt::PenStyle::NoPen);
    //
    painter->setBrush(brush);
    painter->drawRect(rect);
    painter->setPen(pen);
    painter->drawText(rect,m_name);
    //QString t = &"Track: " [ m_track];
    QString t = "track 1";
    //painter->drawText(rect.topLeft() - QPoint(0,-30),t);

    painter->restore();
    delete painter;
}
