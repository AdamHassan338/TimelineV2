#include "timelineview.h"


#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>

int trackHeight = 25;
int rulerHeight = 40;

QColor bgColour = QColor("#262626");
QColor fillColour = QColor("#202020");
QColor seperatorColour = QColor("#313131");
QColor rulerColour = QColor("#4F4F4F");
int textoffset = 13;

TimelineView::TimelineView()
{
    horizontalScrollBar()->setSingleStep(10);
    horizontalScrollBar()->setPageStep(100);
    verticalScrollBar()->setSingleStep(trackHeight);
    verticalScrollBar()->setPageStep(trackHeight * 5);
    viewport()->setMinimumHeight(trackHeight + rulerHeight);
    setMinimumHeight(trackHeight + rulerHeight);


}

void TimelineView::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();
    painter.setBrush(QBrush(bgColour));
    painter.drawRect(event->rect());
    painter.setPen(rulerColour);

    QRect  rect = event->rect();

    for(int i = static_cast<int>(m_scrollOffset.x()/ timescale)*timescale +1;i < event->rect().width() + m_scrollOffset.x(); i+=timescale){
        QString text     = tr("%1").arg(pointToFrame(i));
        QRect   textRect = painter.fontMetrics().boundingRect(text);

        textRect.translate(-m_scrollOffset.x(), 0);
        textRect.translate(i - textRect.width() / 2, rulerHeight - textoffset);

        painter.drawLine(i  - m_scrollOffset.x(),rulerHeight - textoffset +5,
                         i  - m_scrollOffset.x(),rulerHeight);

        painter.drawText(textRect, text);
    }

    painter.restore();
    painter.save();


    for (int i = 0; i < model()->rowCount(); ++i){

        int trackSplitter = i * trackHeight + rulerHeight;

        QModelIndex index = model()->index(i, 0);



        painter.setPen(seperatorColour);
        painter.fillRect(QRect(0,(i * trackHeight)+ rulerHeight,
                               trackwidth,trackHeight),
                         fillColour);

        painter.drawLine(0, trackSplitter, rect.width(), trackSplitter);

    }

    painter.restore();
    painter.save();
    QPen pen(seperatorColour);
    //pen.setStyle(Qt::DotLine);
    //QList<qreal> dashes;

    //dashes << 10 << 10 ;

    //pen.setDashPattern(dashes);
    painter.setPen(pen);

    int lineheight = model()->rowCount() * trackHeight + rulerHeight;

    // draws the vertical lines

    for (int i = static_cast<int>(m_scrollOffset.x() / timescale) * timescale +1;
         i < event->rect().right() + m_scrollOffset.x();
         i += timescale)
    {
        painter.drawLine(i - m_scrollOffset.x(), std::max(rulerHeight, event->rect().top()),
                         i - m_scrollOffset.x(), lineheight);
    }
    painter.restore();
}


void TimelineView::updateScrollBars()
{
    if (!model())
        return;

    int max = 0;
    for (int i = 0; i < model()->rowCount(); ++i)
    {
        QModelIndex index = model()->index(i, model()->columnCount() - 1);
        if (!index.isValid())
        {
            continue;
        }
        max = std::max(max, itemRect(index).right() - viewport()->width());
    }

    horizontalScrollBar()->setRange(0, max);
    verticalScrollBar()->setRange(0, model()->rowCount() * trackHeight + rulerHeight - viewport()->height());
}





QModelIndex TimelineView::indexAt(const QPoint &point) const{ //Currently only works with tracks change when you add clips

    QModelIndex index;
    QModelIndex parent;

    int columnIndex = model()->columnCount()-1;
    for(int i = 0; i < model()->rowCount(); i++){
        if (visualRect(model()->index(i, columnIndex,parent)).contains(point))
        {
            index = model()->index(i,columnIndex,parent);
            qDebug()<< "Track: " << i;
            return index;
        }
    }
    qDebug()<< "Track: " << -1;
    return QModelIndex();
}

QRect TimelineView::itemRect(const QModelIndex &index) const //Currently only works with tracks change when you add clips
{

    //if track
    if(index.parent()==QModelIndex()){
        return QRect(0, (index.row() * trackHeight) + rulerHeight, trackwidth, trackHeight);
    }
    //if Clip
    else{
        return QRect();
    }

}

QRect TimelineView::visualRect(const QModelIndex &index) const
{
    return itemRect(index);
}

int TimelineView::pointToFrame(int point)
{
    return point/timescale ;
}


void TimelineView::mousePressEvent(QMouseEvent *event)
{
    //indexAt(event->pos());
    QAbstractItemView::mousePressEvent(event);
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);
}

int TimelineView::horizontalOffset() const{return 0;}

int TimelineView::verticalOffset() const{return 0;}

bool TimelineView::isIndexHidden(const QModelIndex& index) const { return false; }


void TimelineView::scrollContentsBy(int dx, int dy)
{
    m_scrollOffset -= QPoint(dx, dy);
    QAbstractItemView::scrollContentsBy(dx, dy);
}

void TimelineView::resizeEvent(QResizeEvent *event)
{
    updateScrollBars();
    QAbstractItemView::resizeEvent(event);
}

void TimelineView::showEvent(QShowEvent *event)
{
    updateScrollBars();
    QAbstractItemView::showEvent(event);
}

