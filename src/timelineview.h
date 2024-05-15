#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QAbstractItemView>
#include "clipdelegate.h"
#include <unordered_map>

//cache rect if needed;
struct Clip{
    int pos;
    int in;
    int out;//add 1 when drawring :D
    //int orginalIn;
    int originalOut;
    int track;
    QString name;
    Clip() : pos(0), in(0), out(0), originalOut(0), track(0), name("") {}

    Clip(int pos,int in, int out,int track) : pos(pos),in(in),out(out),track(track), originalOut(out),name("")
    {}
};

struct track{
    int number;
};

enum  hoverState {LEFT,RIGHT,NONE};

class TimelineView : public QAbstractItemView
{
    Q_OBJECT
public:
    TimelineView(QWidget *parent = nullptr);
    void updateScrollBars();

private:

    std::unordered_map<quint64,Clip*> clipMap;
    Clip* getClipFromMap(quint64 id) const{
        const auto clipIter = clipMap.find(id);
        return clipMap.at(id);
    }



    QRect itemRect(const QModelIndex &index) const;

    QPoint m_scrollOffset;
    double m_scale = 1;
    int timescale = 5;
    int trackwidth;//50*timescale;
    int pointToFrame(int point) const;
    int frameToPoint(int frame) const;

    //cuts selected clip under playhead;
    void cutClip();
    ClipDelegate clipDelegate;

    QPoint m_mouseStart;
    QPoint m_mouseEnd;
    QPoint m_mouseOffset;
    bool mouseHeld = false;
    bool m_playheadSelected = false;
    hoverState m_mouseUnderClipEdge = NONE;

    int playheadwidth = 5;//real width is 2x this
    int playheadheight = 16;
    int playheadCornerHeight = playheadheight/4;

    QModelIndex m_hoverIndex = QModelIndex();


    int getTrackWdith() const;
    int getPlayheadPos();

    //this function moves clips by frame, not viewport pixels
    void moveSelectedClip(int dx, int dy,bool isMouse = true);

    bool m_isDroppingMedia = false;
    QPoint m_lastDragPos;

    bool isVideoFile(const QString& filePath);

signals:
    void scrolled(int dx,int dy);
public slots:
    void scroll(int dx, int dy){    m_scrollOffset -= QPoint(dx, dy);
        QAbstractItemView::scrollContentsBy(dx, dy);};

    void setScale(double value);

    void addClipToMap(int row,int track);

    void TrackMoved(int src,int dest);

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

    void leaveEvent(QEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;


};

#endif // TIMELINEVIEW_H
