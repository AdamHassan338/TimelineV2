#include "timelineview.h"


#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include "timelinemodel.h"
#include <unordered_map>

int trackHeight = 25;
int rulerHeight = 40;

QColor bgColour = QColor("#262626");
QColor fillColour = QColor("#202020");
QColor seperatorColour = QColor("#313131");
QColor rulerColour = QColor("#4F4F4F");
int textoffset = 13;

int baseTimeScale = 50;

TimelineView::TimelineView(QWidget *parent) : QAbstractItemView{parent}
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
    setMouseTracking(true);

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
        // Start timer before painting

        QModelIndex trackIndex = model()->index(i, 0);

        for(int j = 0; j<model()->rowCount(trackIndex);j++){

            QModelIndex clipIndex = model()->index(j,0,trackIndex);
            if(selectionModel()->isSelected(clipIndex)){
                option.state |= QStyle::State_Selected;
            }else{
                option.state &= ~QStyle::State_Selected;
            }
            if(m_hoverIndex==clipIndex){
                option.state |= QStyle::State_MouseOver;
            }else{
                option.state &= ~QStyle::State_MouseOver;
            }
            //option.text = QString(QString::number(model()->data(clipIndex,TimelineModel::ClipInRole).toInt()) + "-" + QString::number(model()->data(clipIndex,TimelineModel::ClipOutRole).toInt()));

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
    int playheadPos = frameToPoint(((TimelineModel*)model())->getPlayheadPos()) -m_scrollOffset.x();
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
    QAbstractItemView::selectionChanged(selected,deselected);
    viewport()->update();
}


void TimelineView::updateScrollBars()
{
    if (!model())
        return;

    int max = 0;
    max = getTrackWdith() -  viewport()->width();


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
        return index;
    }


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

            qDebug()<< "IndexAt Track: " << i;
            return index;
        }
    }
    qDebug()<< "IndexAt Track: " << -1;
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

        Clip* clip = clipMap.at(index.internalId()) ;

        int in = clip->in;
        int out = clip->out +1;
        int pos = frameToPoint(clip->pos);
        int track = clip->track;


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

void TimelineView::cutClip()
{
    QModelIndexList list = selectionModel()->selectedIndexes();
    if(list.isEmpty())
        return;

    //IF SELECTED IS NOT A CLIP JUST IN CASE
    if(!list[0].isValid() || !list[0].parent().isValid())
        return;

    int playheadPos = pointToFrame(getPlayheadPos());
    int clipIn = model()->data(list[0],TimelineModel::ClipInRole).toInt();
    int clipOut = model()->data(list[0],TimelineModel::ClipOutRole).toInt();
    int clipPos = model()->data(list[0],TimelineModel::ClipPosRole).toInt();

    int clipLength = clipOut - clipIn ;

    if(playheadPos> clipPos  && playheadPos<=clipLength + clipPos){
        ((TimelineModel*)model())->cutClip(list[0],playheadPos);
    }

    Clip* clip = getClipFromMap(list[0].internalId());
    clip->out = model()->data(list[0],TimelineModel::ClipOutRole).toInt();

    viewport()->update();;

}

int TimelineView::getTrackWdith() const{
    return frameToPoint(model()->data(QModelIndex(),TimelineModel::TimelineLengthRole).toInt() + 1);
}

int TimelineView::getPlayheadPos() {return frameToPoint(((TimelineModel*)model())->getPlayheadPos());}

void TimelineView::setScale(double value)//late make this scale from point of mouse
{

    qDebug()<<"scale value: " << value;
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

void TimelineView::addClipToMap(int row, int track){
    QModelIndex clipIndex = model()->index(row,0,model()->index(track, 0));
    int clipIn = this->model()->data(clipIndex,TimelineModel::ClipInRole).toInt();
    int clipOut = this->model()->data(clipIndex,TimelineModel::ClipOutRole).toInt();
    int clipPos = this->model()->data(clipIndex,TimelineModel::ClipPosRole).toInt();
    Clip* clip = new Clip(clipPos,clipIn,clipOut,track);

    clipMap[clipIndex.internalId()] = clip;
}

void TimelineView::TrackMoved(int src, int dest)
{
    qDebug()<<"src:" <<src <<" dest" <<dest;

    auto it = clipMap.begin();
    if (src > dest) {
        while (it != clipMap.end()) {
            if (it->second->track >= dest && it->second->track < src)
                it->second->track++;
            else if (it->second->track == src)
                it->second->track = dest;
            it++;
        }
    } else {
        while (it != clipMap.end()) {
            if (it->second->track > src && it->second->track <= dest)
                it->second->track--;
            else if (it->second->track == src)
                it->second->track = dest;
            it++;
        }
    }

}

void TimelineView::setModel(QAbstractItemModel *model)
{



    QAbstractItemView::setModel(model);
    clipMap.clear();
    for(int i=0;i<this->model()->rowCount();i++){
        QModelIndex trackIndex = model->index(i, 0);

        for(int j = 0; j<this->model()->rowCount(trackIndex);j++){

            QModelIndex clipIndex = this->model()->index(j,0,trackIndex);
            int clipIn = this->model()->data(clipIndex,TimelineModel::ClipInRole).toInt();
            int clipOut = this->model()->data(clipIndex,TimelineModel::ClipOutRole).toInt();
            int clipPos = this->model()->data(clipIndex,TimelineModel::ClipPosRole).toInt();
            Clip* clip = new Clip(clipPos,clipIn,clipOut,i);

            clipMap[clipIndex.internalId()] = clip;

        }
    }



}


void TimelineView::mousePressEvent(QMouseEvent *event)
{
    m_mouseStart = event->pos();
    m_mouseEnd = m_mouseStart;
    mouseHeld = true;
    m_playheadSelected = false;
    m_hoverIndex = QModelIndex();
    selectionModel()->clearSelection();

    int playheadPos = frameToPoint(((TimelineModel*)model())->getPlayheadPos());
    QRect playheadHitBox(QPoint(playheadPos-3,rulerHeight),QPoint(playheadPos+2,viewport()->height()));
    QRect playheadHitBox2(QPoint(playheadPos-playheadwidth,-playheadheight + rulerHeight),QPoint(playheadPos+playheadwidth,rulerHeight));

    if(playheadHitBox.contains(m_mouseStart)||playheadHitBox2.contains(m_mouseStart)){
        qDebug()<<"playheadSelected";
        m_playheadSelected = true;
        return QAbstractItemView::mousePressEvent(event);
    }

    m_playheadSelected = false;


    QModelIndex item = indexAt(event->pos());
    selectionModel()->clearSelection();

    //item pressed was a clip
    if(item.parent().isValid()){
        selectionModel()->select(item,QItemSelectionModel::Select);
        m_mouseOffset.setX(frameToPoint(getClipFromMap(item.internalId())->pos) - m_mouseStart.x());

    }
    if(selectionModel()->selectedIndexes().isEmpty()){
        ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x()+m_scrollOffset.x())));
        viewport()->update();
    }






    QAbstractItemView::mousePressEvent(event);
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    if(mouseHeld){
        m_mouseEnd = event->pos();
        if(m_mouseUnderClipEdge!=hoverState::NONE && !selectionModel()->selectedIndexes().isEmpty()){
            QModelIndex clip = selectedIndexes().at(0);

            Clip* c = clipMap.at(clip.internalId());
            int in = c->in;
            int out = c->out;
            int pos = c->pos;
            int length = c->originalOut;
            if(m_mouseUnderClipEdge==hoverState::LEFT){
                int newIn = std::clamp(in+ pointToFrame(m_mouseEnd.x() + m_scrollOffset.x()) - pos,0,out);
                model()->setData(clip,newIn,TimelineModel::ClipInRole);
                //clamp to prevent clip moveing when resizing
                moveSelectedClip(std::clamp(pointToFrame(m_mouseEnd.x() + m_scrollOffset.x()) - pos,-in,out-in),0+0,false);
                Clip* c = getClipFromMap(clip.internalId());
                c->in = model()->data(clip,TimelineModel::ClipInRole).toInt();
                viewport()->update();
            }else if(m_mouseUnderClipEdge==hoverState::RIGHT){
                //clamped to not go over clipin or src media length
                int newOut = std::clamp(out + pointToFrame(m_mouseEnd.x() + m_scrollOffset.x()) - pos+in-out,in,length);
                model()->setData(clip,newOut,TimelineModel::ClipOutRole);
                Clip* c = getClipFromMap(clip.internalId());
                c->out = model()->data(clip,TimelineModel::ClipOutRole).toInt();
                viewport()->update();
            }

        }else if(!selectionModel()->selectedIndexes().isEmpty()&&m_mouseEnd.x()>=0){
            moveSelectedClip(pointToFrame(m_mouseEnd.x()+m_mouseOffset.x()),m_mouseEnd.y()+m_mouseOffset.y());
            viewport()->update();
        }else{
            ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x() + m_scrollOffset.x())));
            viewport()->update();
        }

        if(m_playheadSelected){

            ((TimelineModel*)model())->setPlayheadPos(pointToFrame(std::max(0,m_mouseEnd.x() + m_scrollOffset.x())));
            viewport()->update();
        }
        return QAbstractItemView::mouseMoveEvent(event);
    }

    QPoint pos = event->pos();
    m_hoverIndex = indexAt(event->pos());
    QRect rect = visualRect(m_hoverIndex);
    m_mouseUnderClipEdge = hoverState::NONE;
    //5 is hitbox size + -5px
    //see if item is a clip
    if((m_hoverIndex.isValid() && m_hoverIndex.parent().isValid())){
        if(abs(pos.x() - rect.left())<=5){
            m_mouseUnderClipEdge=hoverState::LEFT;
        }else if(abs(pos.x() - rect.right())<=5){
            m_mouseUnderClipEdge=hoverState::RIGHT;
        }

    }
    if (m_mouseUnderClipEdge != hoverState::NONE) {
        setCursor(Qt::SizeHorCursor);
    }else {
        unsetCursor();
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

void TimelineView::leaveEvent(QEvent *event)
{
    mouseHeld = false;
    selectionModel()->clear();
    m_hoverIndex = QModelIndex();
    QAbstractItemView::leaveEvent(event);
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
    case Qt::Key_S:
        cutClip();
        break;
    case Qt::Key_Delete:
        if(list.isEmpty())
            break;
        timelinemodel->deleteClip(list[0]);
        clipMap.erase(list[0].internalId());
        clearSelection();
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
    emit scrolled(dx,dy);
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
    Clip* c = getClipFromMap(list[0].internalId());
    c->pos = model()->data(list[0],TimelineModel::ClipPosRole).toInt();



    if(isMouse && indexAt(m_mouseEnd)!=list[0].parent()){
        int newTrack = ((TimelineModel*)model())->moveClipToTrack(list[0],indexAt(m_mouseEnd));
        if(newTrack!=-1)
           c->track = newTrack;

    }else if(!isMouse && dy!=0){
        QModelIndex nextTrack;
        if( dy>0 && list[0].parent().row()==model()->rowCount()-1){
            nextTrack = ((TimelineModel*)model())->createFakeIndex();
        }else{
            nextTrack = list[0].parent().siblingAtRow(list[0].parent().row()+dy);
        }
        int newTrack =((TimelineModel*)model())->moveClipToTrack(list[0],nextTrack);
        if(newTrack!=-1)
            c->track = newTrack;
    }


    ((TimelineModel*)model())->reCalculateLength();
    updateScrollBars();


}



