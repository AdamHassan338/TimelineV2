#include "clipdelegate.h"

#include <QPainter>
#include "timelinemodel.h"




ClipDelegate::ClipDelegate(QObject *parent)
    : QAbstractItemDelegate{parent}
{}

void ClipDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QColor clipColour("#729ACC");
    QColor cornerColour("#c8c8c8");
    //if(option.state & QStyle::State_Selected)
       //    clipColour = Qt::red;
    int trackHeight = 25;
    int rulerHeight = 40;
    painter->save();
    painter->setClipRect(option.rect);
    painter->setClipping(true);
    const TimelineModel* model     = static_cast<const TimelineModel*>(index.model());
    int start;
    int end;
    int pos;
    //not a clip
    if(index.parent()==QModelIndex()){
        return;
    }

    start = model->data(index,TimelineModel::ClipInRole).toInt();
    end = model->data(index,TimelineModel::ClipOutRole).toInt();
    pos = model->data(index,TimelineModel::ClipPosRole).toInt();
    int track = index.parent().row();

    int length = start-end;

    QPoint topLeft(pos,track*trackHeight + rulerHeight);
    if(option.state & QStyle::State_Selected){
        painter->save();
        int penWidth = 4;
        painter->setPen(QPen(Qt::white,penWidth));
        painter->setBrush(QBrush(clipColour));
        painter->drawRect(option.rect.left()+penWidth/2,option.rect.top()+penWidth/2,
                          option.rect.width()-penWidth,option.rect.height()-penWidth);



        painter->restore();
    }else{
        painter->fillRect(option.rect,QBrush(clipColour));
    }

    QString text("Video.mp4");
    QRect   textRect = painter->fontMetrics().boundingRect(text);
    int textOffset = textRect.height();
    textRect.translate(0,textOffset);
    if(option.rect.left()<0){
        textRect.translate(0,option.rect.top());
    }else{
        textRect.translate(option.rect.topLeft());
    }    painter->drawText(textRect, text);
    painter->setPen(Qt::red);
    //painter->drawRect(textRect);
    painter->restore();

    QPoint points[6] = {
        option.rect.topLeft(),
        QPoint(option.rect.left() + 10,option.rect.top()),
        QPoint(option.rect.left(),option.rect.top()+10),
        QPoint(option.rect.topRight().x()+1,option.rect.topRight().y()),
        QPoint(option.rect.right()+1 - 10,option.rect.top()),
        QPoint(option.rect.right()+1,option.rect.top()+10),
    };

    if(option.state &~ QStyle::State_Selected){
        painter->save();
        painter->setBrush(cornerColour);
        painter->setPen(Qt::NoPen);
        painter->drawConvexPolygon(points, 3);
        painter->drawConvexPolygon(points+3, 3);
        painter->restore();
    }




}

QSize ClipDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return option.rect.size();
}
