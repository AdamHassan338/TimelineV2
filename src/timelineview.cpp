#include "timelineview.h"


#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include "timelinemodel.h"

int trackHeight = 25;
int rulerHeight = 40;

QColor bgColour = QColor("#262626");
QColor fillColour = QColor("#202020");
QColor seperatorColour = QColor("#313131");
QColor rulerColour = QColor("#4F4F4F");
int textoffset = 13;

int baseTimeScale = 50;

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
    int trackwidth = getTrackWdith();
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();
    painter.setBrush(QBrush(bgColour));
    painter.drawRect(event->rect());

    painter.restore();
    painter.save();

    //draw tracks
    for (int i = 0; i < model()->rowCount(); ++i){

        int trackSplitter = i * trackHeight + rulerHeight -  m_scrollOffset.y();

        painter.setPen(seperatorColour);
        painter.fillRect(QRect(0,(i * trackHeight)+ rulerHeight -m_scrollOffset.y(),
                               trackwidth,trackHeight),
                         fillColour);

        painter.drawLine(0, trackSplitter, event->rect().width(), trackSplitter);



    }

    painter.restore();
    painter.save();

    QPen pen(seperatorColour);
    painter.setPen(pen);

    int lineheight = model()->rowCount() * trackHeight + rulerHeight;

    // draws the vertical lines

    int frameStep;

    if (timescale > 20) {
        frameStep = 1;  // Draw text at every frame
    } else if (timescale > 10) {
        frameStep = 5;  // Draw text every 5 frames
    } else if (timescale > 5) {
        frameStep = 10; // Draw text every 10 frames
    } else {
        frameStep = 25; // Draw text every 25 frames
    }


    int startMarker = static_cast<int>(pointToFrame(m_scrollOffset.x()))*timescale +1;
    int endMarker = event->rect().width() + m_scrollOffset.x();
    startMarker = pointToFrame(startMarker);
    startMarker-= (startMarker%frameStep);
    startMarker = frameToPoint(startMarker);

    for (int i = startMarker; i<endMarker; i += timescale*frameStep)
    {
        painter.drawLine(i - m_scrollOffset.x(), std::max(rulerHeight, event->rect().top()),
                         i - m_scrollOffset.x(), lineheight);
    }
    painter.restore();

    painter.save();

    //draw clips
    QStyleOptionViewItem option;
    QAbstractItemView::initViewItemOption(&option);
    for(int i=0;i<model()->rowCount();i++){
        QModelIndex trackIndex = model()->index(i, 0);

        for(int j = 0; j<model()->rowCount(trackIndex);j++){
            QModelIndex clipIndex = model()->index(j,0,trackIndex);
            option.rect = visualRect(clipIndex);
            painter.save();
            clipDelegate.paint(&painter,option,clipIndex);
            painter.restore();
        }
    }
    painter.restore();
    painter.save();

    //draw ruler

    painter.setBrush(QBrush(bgColour));
    painter.drawRect(m_scrollOffset.x(),0,event->rect().width(),rulerHeight);

    painter.setPen(rulerColour);


    static int jump = 1;
    if(  baseTimeScale%timescale == 0){
        jump =  baseTimeScale/timescale;
    }




    for(int i = startMarker;i < endMarker; i+=timescale*frameStep){

        int number = pointToFrame(i);
        QString text     = tr("%1").arg(number);
        QRect   textRect = painter.fontMetrics().boundingRect(text);

        textRect.translate(-m_scrollOffset.x(), 0);
        textRect.translate(i - textRect.width() / 2, rulerHeight - textoffset);

        painter.drawLine(i  - m_scrollOffset.x(),rulerHeight - textoffset +5,
                         i  - m_scrollOffset.x(),rulerHeight);
        /*
50px / F every 1

10 /f every 5


5px /F every 10


2px /F every 25




*/
        /*if(timescale>20){

        }else if(timescale>10){
            if(number%5)
                text = "";
        }else if(timescale>5){
            if(number%10)
                text = "";
        }else{
            if(number%25)
               text = "";
        }*/
        painter.drawText(textRect, text);
    }



    //draw inbetween
    for(int i = startMarker;i < endMarker; i+=timescale){
        int number = pointToFrame(i);
        int boost = 0 ;

        if (timescale > 20) {
            break;  // Draw text at every frame
        } else if (timescale > 10) {
            if(number%5==0)
                continue;// skip every 5th marker

        } else if (timescale > 5) {
            if(number%10==0)
                continue; //skip every 10th marker

            if(number%5==0)
                boost =5; //hilight every 5th
        } else {
            if(number%25==0)
                continue; //skip every 25th marker

            if(number%5==0)
                boost =5; //hilight every 5th

        }

        painter.drawLine(i  - m_scrollOffset.x(),rulerHeight - textoffset +10 -boost ,
                         i  - m_scrollOffset.x(),rulerHeight);
    }

    painter.restore();

}


void TimelineView::updateScrollBars()
{
    if (!model())
        return;

    int max = 0;
    max = frameToPoint(model()->data(QModelIndex(),TimelineModel::TimelineLengthRole).toInt()) -  viewport()->width();


    horizontalScrollBar()->setRange(0, max);
    verticalScrollBar()->setRange(0, model()->rowCount() * trackHeight + rulerHeight - viewport()->height());
}





QModelIndex TimelineView::indexAt(const QPoint &point) const{ //Currently only works with tracks change when you add clips


    QModelIndex index;
    QModelIndex parent;

    QRect rullerRect(m_scrollOffset.x(),0,viewport()->width(),rulerHeight);

    if(rullerRect.contains(point)){
            qDebug()<< "Clicked ruller: ";
        return index;}


    int columnIndex = model()->columnCount()-1;
    for(int i = 0; i < model()->rowCount(); i++){
        if (visualRect(model()->index(i, columnIndex,parent)).contains(point))
        {
            index = model()->index(i,columnIndex,parent);
            qDebug()<< "Clicked Track: " << i;
            return index;
        }
    }
    qDebug()<< "Clicked Track: " << -1;
    return index;
}

QRect TimelineView::itemRect(const QModelIndex &index) const
{
    int trackwidth = getTrackWdith();
    //if track
    if(index.parent()==QModelIndex()){
        return QRect(0, (index.row() * trackHeight) + rulerHeight, trackwidth, trackHeight);
    }
    //if Clip
    else{

        int in = model()->data(index,TimelineModel::ClipInRole).toInt();
        int out = model()->data(index,TimelineModel::ClipOutRole).toInt();
        int pos = frameToPoint(model()->data(index,TimelineModel::ClipPosRole).toInt());
        int track = index.parent().row();

        int length = frameToPoint(out-in);


        QPoint topLeft(pos,track*trackHeight + rulerHeight);



        return QRect(topLeft.x(),topLeft.y(),length,trackHeight);

    }
 return QRect();
}

QRect TimelineView::visualRect(const QModelIndex &index) const
{
    return itemRect(index).translated(-m_scrollOffset);
}

int TimelineView::pointToFrame(int point) const
{
    return point/timescale ;
}

int TimelineView::frameToPoint(int frame) const
{
    return frame*timescale;
}

int TimelineView::getTrackWdith() const{
    return frameToPoint(model()->data(QModelIndex(),TimelineModel::TimelineLengthRole).toInt());
}

void TimelineView::setScale(double value)
{
    timescale=value;
    updateScrollBars();
    viewport()->update();
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



