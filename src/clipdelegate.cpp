#include "clipdelegate.h"

#include <QPainter>
#include "timelinemodel.h"




ClipDelegate::ClipDelegate(QObject *parent)
    : QAbstractItemDelegate{parent}
{}

void ClipDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QColor clipColour("#729ACC");
    QColor videoColour("#729ACC");
    QColor audioColour("#cc72c7");
    QColor cornerColour("#c8c8c8");
    if(option.state & QStyle::State_MouseOver)
           clipColour = clipColour.darker(150);
    int trackHeight = 25;
    int rulerHeight = 40;

    QString text;
    switch (index.data(TimelineModel::ClipTypeRole).value<MediaType>()){
    case MediaType::VIDEO:
        text = "Video";
        clipColour = videoColour;
        break;
    case MediaType::AUDIO:
        text = "Audio";
        clipColour = audioColour;
        break;

    }

    painter->save();
    painter->setClipRect(option.rect);
    painter->setClipping(true);

    //not a clip
    if(index.parent()==QModelIndex()){
        return;
    }
    int track = index.parent().row();

    if(option.state & QStyle::State_Selected){
        painter->save();
        int penWidth = 4;
        painter->setPen(QPen(Qt::white,penWidth));
        painter->setBrush(QBrush(clipColour));
        painter->drawRect(option.rect.left()+penWidth/2,option.rect.top()+penWidth/2,
                          option.rect.width()-penWidth,option.rect.height()-penWidth);



        painter->restore();
    }else{
        painter->setPen(clipColour.darker(300));
        painter->setBrush(QBrush(clipColour));
        painter->drawRect(option.rect);
    }


    QRect   textRect = painter->fontMetrics().boundingRect(text);
    int textOffset = textRect.height();
    textRect.translate(0,textOffset);
    if(option.rect.left()<0){
        textRect.translate(0,option.rect.top());
    }else{
        textRect.translate(option.rect.topLeft());
    }   painter->drawText(textRect, text);
    painter->setPen(Qt::red);
    //painter->drawRect(textRect);
    painter->restore();

    QPoint points[6] = {
        option.rect.topLeft(),
        QPoint(option.rect.left() + 5,option.rect.top()),
        QPoint(option.rect.left(),option.rect.top()+5),
        QPoint(option.rect.topRight().x()+1,option.rect.topRight().y()),
        QPoint(option.rect.right()+1 - 5,option.rect.top()),
        QPoint(option.rect.right()+1,option.rect.top()+5),
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
