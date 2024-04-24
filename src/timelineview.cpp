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
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectItems);
    setAutoScroll(true);
    setAutoScrollMargin(5);



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
            if(selectionModel()->isSelected(clipIndex)){
                //qDebug()<<"clips is selected";
                option.state |= QStyle::State_Selected;
            }else{
                option.state &= ~QStyle::State_Selected;
            }
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
    painter.drawRect(-m_scrollOffset.x(),0,event->rect().width() + m_scrollOffset.x(),rulerHeight);

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


    //draw playhead
    painter.save();



    QPoint kite[5]{
        QPoint(0,0),QPoint(-playheadwidth,-playheadCornerHeight),QPoint(-playheadwidth,-playheadheight),QPoint(playheadwidth,-playheadheight),QPoint(playheadwidth,-playheadCornerHeight)
    };
    int playheadPos = frameToPoint(((TimelineModel*)model())->getPlayheadPos());
    for(QPoint &p:kite){
        p.setX(p.x()+playheadPos);
        p.setY(p.y()+rulerHeight);
    }
    painter.setBrush(Qt::white);
    painter.drawConvexPolygon(kite,5);
    painter.setPen(Qt::white);
    painter.drawLine(QPoint(playheadPos,rulerHeight),QPoint(playheadPos,viewport()->height()));


    painter.restore();

}

void TimelineView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug()<<"Selection Changed";
    QAbstractItemView::selectionChanged(selected,deselected);
    viewport()->update();
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





QModelIndex TimelineView::indexAt(const QPoint &point) const{


    QModelIndex index;
    QModelIndex parent;

    QRect rullerRect(-m_scrollOffset.x(),0,viewport()->width() + m_scrollOffset.x(),rulerHeight);
    if(point.y()<0)//if above the ruler
        return index;
    if(rullerRect.contains(point)){
            qDebug()<< "Clicked ruller: ";
        return index;}


    int columnIndex = model()->columnCount()-1;
    for(int i = 0; i < model()->rowCount(); i++){

        if (visualRect(model()->index(i, columnIndex,parent)).contains(point))
        {
            index = model()->index(i,columnIndex,parent);
            parent = index;
            //selection priority should happen in reverse of paint order
            for(int j=model()->rowCount(index)-1; j>=0; j--){

                if(visualRect(model()->index(j, columnIndex,index)).contains(point)){
                    return model()->index(j,columnIndex,index);
                    //return index;
                }

            }

            qDebug()<< "Clicked Track: " << i;
            return index;
        }
    }
    qDebug()<< "Clicked Track: " << -1;
    index = ((TimelineModel*)model())->createFakeIndex();
    if(!index.isValid()){
        qDebug()<<"dont worry still invalid";
    }
    return index;
}

QRect TimelineView::itemRect(const QModelIndex &index) const
{
    int trackwidth = getTrackWdith() + 500;//500 is for extra width outside of current timeline length;
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

void TimelineView::setScale(double value)//late make this scale from point of mouse
{

    qDebug()<<"value " << value;
    int left = -m_scrollOffset.x();

    int right = viewport()->width() + m_scrollOffset.x();
    int total = (left + right) / 2;
    qDebug() << "old "<<pointToFrame( left ) << " " << pointToFrame(right);
    //get middpoint


    int copy = total;
    total = pointToFrame(total);

    qDebug() << "mid "<< total;

    timescale=value;
 //updateScrollBars();

    if(0){
        int newtotal = copy;


        newtotal = pointToFrame(newtotal);
        int diff = frameToPoint(total-newtotal);

        qDebug() << "new mid "<< newtotal << "moved" << pointToFrame(diff);

        qDebug()<<"scroll" << m_scrollOffset.x();
        if (m_scrollOffset.x() + diff < 0) {
            qDebug()<<"DOING THE FIX !!!!!!!!!!!!!!!!";
            diff = -m_scrollOffset.x();
        }
        scrollContentsBy(-diff,0);
        qDebug()<<"newscroll" << m_scrollOffset.x();


    }

    updateScrollBars();

    viewport()->update();
}

void TimelineView::setModel(QAbstractItemModel *model)
{


    //QItemSelectionModel* oldSelectionModel = selectionModel();
   // delete oldSelectionModel;
   // oldSelectionModel = nullptr;

    QAbstractItemView::setModel(model);

    //QItemSelectionModel* selectionModel = new QItemSelectionModel(model);

    //setSelectionModel(selectionModel);


}


void TimelineView::mousePressEvent(QMouseEvent *event)
{
    m_mouseStart = event->pos();
    m_mouseEnd = m_mouseStart;
    mouseHeld = true;
    m_playheadSelected = false;
selectionModel()->clearSelection();

    int playheadPos = frameToPoint(((TimelineModel*)model())->getPlayheadPos());
    QRect playheadHitBox(QPoint(playheadPos-3,rulerHeight),QPoint(playheadPos+2,viewport()->height()));
    QRect playheadHitBox2(QPoint(playheadPos-playheadwidth,-playheadheight + rulerHeight),QPoint(playheadPos+playheadwidth,rulerHeight));

    if(playheadHitBox.contains(m_mouseStart)||playheadHitBox2.contains(m_mouseStart)){
        qDebug()<<"playheadSelected";
        m_playheadSelected = true;
    }else{
        m_playheadSelected = false;

        QModelIndex item = indexAt(event->pos());
        selectionModel()->clearSelection();

        //item pressed was a clip
        if(item.parent().isValid()){
            selectionModel()->select(item,QItemSelectionModel::Select);
            m_mouseOffset.setX(frameToPoint(model()->data(item,TimelineModel::ClipPosRole).toInt()) - m_mouseStart.x());
        }
        if(selectionModel()->selectedIndexes().isEmpty()){
            ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x())));
            viewport()->update();
        }

    }




    QAbstractItemView::mousePressEvent(event);
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseHeld){
        m_mouseEnd = event->pos();
        if(!selectionModel()->selectedIndexes().isEmpty()&&m_mouseEnd.x()>=0){
            moveSelectedClip(pointToFrame(m_mouseEnd.x()+m_mouseOffset.x()),m_mouseEnd.y()+m_mouseOffset.y());
            viewport()->update();
        }else{
            ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x())));
            viewport()->update();
        }

        if(m_playheadSelected){

            ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x())));
            viewport()->update();
        }

    }
    QAbstractItemView::mouseMoveEvent(event);
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    mouseHeld = false;
    m_playheadSelected = false;
    m_mouseEnd = event->pos();


    //pressed outside of selection
    QAbstractItemView::mouseReleaseEvent(event);
}

void TimelineView::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList list = selectionModel()->selectedIndexes();

    TimelineModel* timelinemodel = (TimelineModel*)model();

    switch (event->key()){
    case Qt::Key_Right:
        if(list.isEmpty())
            timelinemodel->movePlayhead(1);
        moveSelectedClip(1,0,false);
        viewport()->update();
        break;
    case Qt::Key_Left:
        if(list.isEmpty())
            timelinemodel->movePlayhead(-1);
        moveSelectedClip(-1,0,false);
        viewport()->update();
        break;
    case Qt::Key_Down :
        moveSelectedClip(0,1,false);
        break;
    case Qt::Key_Up:
        moveSelectedClip(0,-1,false);
        break;
    defualt:
        break;
    }
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

void TimelineView::moveSelectedClip(int dx, int dy, bool isMouse)
{
    QModelIndexList list = selectionModel()->selectedIndexes();
    if(list.isEmpty())
        return;
    int newPos;
    if(isMouse){
        newPos = dx;
    }else{
        newPos = model()->data(list[0],TimelineModel::ClipPosRole).toInt() + dx;
    }


    model()->setData(list[0],newPos,TimelineModel::ClipPosRole);




    if(isMouse && indexAt(m_mouseEnd)!=list[0].parent()){
        ((TimelineModel*)model())->moveClipToTrack(list[0],indexAt(m_mouseEnd));
    }

    if(dy!=0){
        QModelIndex nextTrack;
        if( dy>0 && list[0].parent().row()==model()->rowCount()-1){
            nextTrack = ((TimelineModel*)model())->createFakeIndex();
        }else{
            nextTrack = list[0].parent().siblingAtRow(list[0].parent().row()+dy);
        }
        ((TimelineModel*)model())->moveClipToTrack(list[0],nextTrack);
    }

    ((TimelineModel*)model())->reCalculateLength();
    updateScrollBars();


}



