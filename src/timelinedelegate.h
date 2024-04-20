#ifndef TIMELINEDELEGATE_H
#define TIMELINEDELEGATE_H

#include <QStyledItemDelegate>
class TrackView;
class TimelineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    TimelineDelegate();

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QRect m_rect;

    //private:

    int m_number;
    int m_width;
    int m_height;

    //TrackView* track;
};

#endif // TIMELINEDELEGATE_H
