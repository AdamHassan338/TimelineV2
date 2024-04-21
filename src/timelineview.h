#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QAbstractItemView>



class TimelineView : public QAbstractItemView
{
    Q_OBJECT
public:
    TimelineView();
    void updateScrollBars();

private:
    QRect itemRect(const QModelIndex &index) const;

    QPoint m_scrollOffset;
    double m_scale = 1;
    int timescale = 50;
    int trackwidth = 50*timescale;
    int pointToFrame(int point);
public slots:
    void setScale(double value)
    {
        //m_scale = (double)value/(double)10;
        //50
        //1
        timescale=value;//m_scale;
        trackwidth = 50*timescale;
        updateScrollBars();
        viewport()->update();
    }


    // QAbstractItemView interface
public:

    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint) override{};
    QModelIndex indexAt(const QPoint &point) const override;;

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override {return QModelIndex();};
    int horizontalOffset() const override;
    int verticalOffset() const override;
    bool isIndexHidden(const QModelIndex &index) const override;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override{};
    QRegion visualRegionForSelection(const QItemSelection &selection) const override {return QRegion();};


    void paintEvent(QPaintEvent *event) override;

    // QAbstractScrollArea interface
protected:
    void scrollContentsBy(int dx, int dy) override;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
};

#endif // TIMELINEVIEW_H
