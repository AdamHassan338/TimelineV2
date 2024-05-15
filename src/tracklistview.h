#ifndef TRACKLISTVIEW_H
#define TRACKLISTVIEW_H

#include <QAbstractItemView>

#include "trackdelegate.h"

class TracklistView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit TracklistView(QWidget *parent = nullptr);
    void updateScrollBars();

signals:
    void scrolled(int dx,int dy);

public slots:
    void scroll(int dx, int dy){    m_scrollOffset -= QPoint(0, dy);
        updateEditorGeometries();
        QAbstractItemView::scrollContentsBy(dx, dy);};

    void setTime(int frame){
        m_time=frame;
        viewport()->update();
    };

    void updateViewport(){
        viewport()->update();
    }

private:

    TrackDelegate delegate;

    int m_time = 0;

    QRect itemRect(const QModelIndex &index) const;

    QPoint m_scrollOffset;

    QPoint m_mouseStart;
    QPoint m_mouseEnd;
    QPoint m_mouseOffset;
    bool m_mouseHeld = false;
    QItemSelectionModel* m_selectionmodel;

    QModelIndex m_hoverIndex = QModelIndex();

    bool isDragging = false;

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
    void leaveEvent(QEvent *event) override;

    // QAbstractItemView interface
protected:

    // QAbstractItemView interface
protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    // QWidget interface
protected:
    void dropEvent(QDropEvent *event) override;
        void dragMoveEvent(QDragMoveEvent *event) override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

    // QAbstractScrollArea interface
protected:
    void scrollContentsBy(int dx, int dy) override;

    // QAbstractItemView interface
protected slots:
    void updateEditorGeometries() override;

    // QWidget interface
protected:


    // QWidget interface
protected:

};

#endif // TRACKLISTVIEW_H
