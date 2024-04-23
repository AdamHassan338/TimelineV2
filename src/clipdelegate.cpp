#include "clipdelegate.h"

#include <QPainter>
#include "timelinemodel.h"


QColor clipColour("#729ACC");

ClipDelegate::ClipDelegate(QObject *parent)
    : QAbstractItemDelegate{parent}
{}

void ClipDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
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

    painter->fillRect(option.rect,QBrush(clipColour));


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



}

QSize ClipDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return option.rect.size();
}
