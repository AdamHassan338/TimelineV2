#ifndef TRACKLISTVIEW_H
#define TRACKLISTVIEW_H

#include <QAbstractItemView>

class TracklistView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit TracklistView(QWidget *parent = nullptr);

signals:
private:
    QRect itemRect(const QModelIndex &index) const;

    QPoint m_scrollOffset;

    QPoint m_mouseStart;
    QPoint m_mouseEnd;
    QPoint m_mouseOffset;
    bool m_mouseHeld = false;
    QItemSelectionModel* m_selectionmodel;

    QModelIndex m_hoverIndex = QModelIndex();

    //QDrag* drag = nullptr;


    // QAbstractItemView interface
public:
    void setModel(QAbstractItemModel *model) override;
    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint) override{};
    QModelIndex indexAt(const QPoint &point) const override;

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override {return QModelIndex();};
    int horizontalOffset() const override {return 0;}
    int verticalOffset() const override {return 0;}
    bool isIndexHidden(const QModelIndex &index) const override { return false; }
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override{};
    QRegion visualRegionForSelection(const QItemSelection &selection) const override {return QRegion();};


    void paintEvent(QPaintEvent *event) override;


    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    // QAbstractItemView interface
protected:

    // QAbstractItemView interface
protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    // QWidget interface
protected:
    void dropEvent(QDropEvent *event) override;
};

#endif // TRACKLISTVIEW_H
