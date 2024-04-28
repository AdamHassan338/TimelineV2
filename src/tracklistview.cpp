#include "tracklistview.h"


#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QDrag>
#include "timelinemodel.h"

TracklistView::TracklistView(QWidget *parent)
    : QAbstractItemView{parent}
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode( QAbstractItemView::InternalMove );
    setDefaultDropAction( Qt::MoveAction );
    setDropIndicatorShown(true);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectItems);
    m_scrollOffset = QPoint(0,0);
}

void TracklistView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
}

QRect TracklistView::visualRect(const QModelIndex &index) const
{
    return itemRect(index).translated(-m_scrollOffset);
}

QRect TracklistView::itemRect(const QModelIndex &index) const
{
    int trackHeight = 25;
    int rulerHeight = 40;

    //if(!index.isValid()||index.parent()!=QModelIndex())
      //  return QRect();

    return QRect(0, (index.row() * trackHeight) + rulerHeight, viewport()->width(), trackHeight);

}


QModelIndex TracklistView::indexAt(const QPoint &point) const
{
    int trackHeight = 25;
    int rulerHeight = 40;
    QModelIndex index;
    QModelIndex parent;

    QRect rullerRect(0,0,viewport()->width(),rulerHeight);
    if(point.y()<0)//if above the ruler
        return index;
    if(rullerRect.contains(point)){
        qDebug()<< "Clicked ruller: ";
        return index;}


    int columnIndex = model()->columnCount()-1;
    for(int i = 0; i < model()->rowCount(); i++){

        if (visualRect(model()->index(i, columnIndex,QModelIndex())).contains(point))
        {
            index = model()->index(i,columnIndex,QModelIndex());
            qDebug()<< "Clicked Track list: " << i;
            return index;
        }
    }
    index = ((TimelineModel*)model())->createFakeIndex();
    return index;
}

void TracklistView::paintEvent(QPaintEvent *event)
{
    int trackHeight = 25;
    int rulerHeight = 40;

    QColor bgColour = QColor("#262626");
    QColor fillColour = QColor("#202020");
    QColor seperatorColour = QColor("#313131");
    QColor rulerColour = QColor("#4F4F4F");
    int textoffset = 13;

    int baseTimeScale = 50;

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing,true);

    painter.save();

    painter.setBrush(QBrush(bgColour));
    painter.drawRect(event->rect());

    painter.restore();

    painter.save();


    painter.setBrush(QBrush(rulerColour));
    //draw tracklist

    int viewportWidth = viewport()->width();

    QRect trackRect;

    for (int i = 0; i < model()->rowCount(); ++i){

        int trackSplitter = i * trackHeight + rulerHeight ;
        trackRect = QRect(0,(i * trackHeight)+ rulerHeight,
                          viewportWidth,trackHeight);
        painter.setPen(fillColour);
        painter.setBrush(fillColour.lighter(150));
        painter.drawRect(trackRect);


        painter.drawLine(0, trackSplitter, event->rect().width(), trackSplitter);
        painter.setPen(Qt::white);
        painter.drawText(trackRect,Qt::AlignCenter,QString("track:" +
QString::number(model()->data(model()->index(i,0),TimelineModel::TrackNumberRole).toInt()
                                                                                        )));


    }

    painter.restore();





}

void TracklistView::mousePressEvent(QMouseEvent *event)
{
    m_mouseStart = event->pos();
    m_mouseEnd = m_mouseStart;
    m_mouseHeld = true;
    selectionModel()->clear();
    QModelIndex item = indexAt( event->pos());

    if(item.isValid() && model()->hasIndex(item.row(),item.column(),item.parent())){
        selectionModel()->select(item,QItemSelectionModel::Select);

    }

    QAbstractItemView::mousePressEvent(event);
}

void TracklistView::mouseReleaseEvent(QMouseEvent *event)
{

    m_mouseHeld = false;
    m_mouseEnd = event->pos();


    QAbstractItemView::mouseReleaseEvent(event);


}

void TracklistView::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseEnd = event->pos();
    qDebug()<<selectionModel()->selectedIndexes().size();
    if(!selectionModel()->selectedIndexes().isEmpty()){
        QDrag* drag = new QDrag(this);
        QMimeData *mimeData = model()->mimeData(selectionModel()->selectedIndexes());
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
    }

    QAbstractItemView::mouseMoveEvent(event);
    viewport()->update();

}

void TracklistView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QAbstractItemView::selectionChanged(selected,deselected);
    viewport()->update();
}

void TracklistView::dropEvent(QDropEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (dragDropMode() == InternalMove) {
        if (event->source() != this || !(event->possibleActions() & Qt::MoveAction))
            return;
    }


        const Qt::DropAction action = dragDropMode() == InternalMove ? Qt::MoveAction : event->dropAction();
        if (model()->dropMimeData(event->mimeData(), action, index.row(), index.column(), index)) {
            if (action != event->dropAction()) {
                event->setDropAction(action);
                event->accept();
            } else {
                event->acceptProposedAction();
            }
        }

    stopAutoScroll();
    setState(NoState);
    viewport()->update();


}

