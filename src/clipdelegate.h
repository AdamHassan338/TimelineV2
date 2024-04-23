#ifndef CLIPDELEGATE_H
#define CLIPDELEGATE_H

#include <QAbstractItemDelegate>

class ClipDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit ClipDelegate(QObject *parent = nullptr);
    ~ClipDelegate() = default;

signals:

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // CLIPDELEGATE_H
