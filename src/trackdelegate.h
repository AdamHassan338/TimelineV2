#ifndef TRACKDELEGATE_H
#define TRACKDELEGATE_H

#include <QAbstractItemDelegate>
#include <QObject>

class TrackDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit TrackDelegate(QObject *parent = nullptr);

signals:

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // QAbstractItemDelegate interface
public:
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QWidget* m_editor;
};

#endif // TRACKDELEGATE_H
