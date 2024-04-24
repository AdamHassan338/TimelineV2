#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QAbstractItemView>
#include "clipdelegate.h"


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
    int timescale = 5;
    int trackwidth;//50*timescale;
    int pointToFrame(int point) const;
    int frameToPoint(int frame) const;
    ClipDelegate clipDelegate;

    QPoint m_mouseStart;
    QPoint m_mouseEnd;
    QPoint m_mouseOffset;
    bool mouseHeld;
    bool m_playheadSelected = false;

    int playheadwidth = 5;//real width is 2x this
    int playheadheight = 16;
    int playheadCornerHeight = playheadheight/4;


    int getTrackWdith() const;
public slots:
    void setScale(double value);


    // QAbstractItemView interface
public:
    void setModel(QAbstractItemModel *model) override;
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


protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;

    // QAbstractScrollArea interface
protected:
    void scrollContentsBy(int dx, int dy) override;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;


    //this function moves clips by frame, not viewport pixels
    void moveSelectedClip(int dx, int dy,bool isMouse = true);
};

#endif // TIMELINEVIEW_H
