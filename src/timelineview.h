#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QAbstractItemView>

class TimelineView : public QAbstractItemView
{
    Q_OBJECT
public:
    TimelineView();

    // QAbstractItemView interface
public:
    /*
    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint) override;
    QModelIndex indexAt(const QPoint &point) const override;

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    int horizontalOffset() const override;
    int verticalOffset() const override;
    bool isIndexHidden(const QModelIndex &index) const override;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override;
    QRegion visualRegionForSelection(const QItemSelection &selection) const override;


    void paintEvent(QPaintEvent *event) override;
*/
};

#endif // TIMELINEVIEW_H
