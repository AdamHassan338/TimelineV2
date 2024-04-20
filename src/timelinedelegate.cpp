#include "timelinedelegate.h"

#include "trackview.h"
TimelineDelegate::TimelineDelegate()
{

}

void TimelineDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::paint(painter,option,index);
}

QSize TimelineDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    return QStyledItemDelegate::sizeHint(option,index);
}

QWidget *TimelineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TrackView* track = new TrackView(index.row(),0,0,parent);
    return QStyledItemDelegate::createEditor(parent,option,index);
}
