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
    int trackHeight = 25;
    int rulerHeight = 40;
    horizontalScrollBar()->setSingleStep(10);
    horizontalScrollBar()->setPageStep(100);
    verticalScrollBar()->setSingleStep(trackHeight);
    verticalScrollBar()->setPageStep(trackHeight * 5);
    viewport()->setMinimumHeight(trackHeight + rulerHeight);


    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode( QAbstractItemView::InternalMove );
    setDefaultDropAction( Qt::MoveAction );
    setDropIndicatorShown(true);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectItems);
    m_scrollOffset = QPoint(0,0);

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff ) ;

    setEditTriggers(EditTrigger::AllEditTriggers);
    setItemDelegateForColumn(0,new TrackDelegate);

    setMouseTracking(true);
}

void TracklistView::updateScrollBars()
{
    if(!model())
        return;

    int max =0;
    int trackHeight = 25;
    int rulerHeight = 40;

    verticalScrollBar()->setRange(0,model()->rowCount() * trackHeight + rulerHeight - viewport()->height());
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

    QStyleOptionViewItem option;
    QAbstractItemView::initViewItemOption(&option);
    for (int i = 0; i < model()->rowCount(); ++i){
        QModelIndex trackIndex = model()->index(i,0);
        openPersistentEditor(model()->index(i,0));
        if(selectionModel()->isSelected(trackIndex)){
            option.state |= QStyle::State_Selected;
        }else{
            option.state &= ~QStyle::State_Selected;
        }
        if(m_hoverIndex==trackIndex && !selectionModel()->isSelected(trackIndex)){
            option.state |= QStyle::State_MouseOver;
            if(isDragging){
                option.state |= QStyle::State_Raised;
            }
        }else{
            option.state &= ~QStyle::State_MouseOver;
            if(!isDragging){
                option.state &=~QStyle::State_Raised;
            }
        }



        option.text = QString("" + QString::number(model()->data(model()->index(i,0),TimelineModel::TrackNumberRole).toInt()));
        option.rect = visualRect(trackIndex);
        painter.save();
        itemDelegateForIndex(trackIndex)->paint(&painter,option,trackIndex);
        painter.restore();
        /*



        int trackSplitter = i * trackHeight + rulerHeight - m_scrollOffset.y() ;
        trackRect = QRect(0,(i * trackHeight)+ rulerHeight - m_scrollOffset.y(),
                          viewportWidth,trackHeight);
        painter.setPen(fillColour);
        painter.setBrush(fillColour.lighter(150));
        painter.drawRect(trackRect);


        painter.drawLine(0, trackSplitter, event->rect().width(), trackSplitter);
        painter.setPen(Qt::white);
        painter.drawText(trackRect,Qt::AlignCenter,QString("track:" +
QString::number(model()->data(model()->index(i,0),TimelineModel::TrackNumberRole).toInt()
                                                                                        )));

*/
    }

    painter.restore();

    painter.save();

    painter.setBrush(QBrush(bgColour));
    painter.drawRect(0,0,viewportWidth,rulerHeight);

    painter.restore();





}

void TracklistView::mousePressEvent(QMouseEvent *event)
{
    m_mouseStart = event->pos();
    m_mouseEnd = m_mouseStart;
    m_mouseHeld = true;
    m_hoverIndex = QModelIndex();
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
    isDragging = false;
    m_mouseEnd = event->pos();


    QAbstractItemView::mouseReleaseEvent(event);


}

void TracklistView::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseEnd = event->pos();
    m_hoverIndex = indexAt(m_mouseEnd);
    if(!selectionModel()->selectedIndexes().isEmpty() && m_mouseHeld){
        QDrag* drag = new QDrag(this);
        QMimeData *mimeData = model()->mimeData(selectionModel()->selectedIndexes());
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
        isDragging = true;
    }

    QAbstractItemView::mouseMoveEvent(event);
    m_hoverIndex = indexAt(m_mouseEnd);
    viewport()->update();

}

void TracklistView::leaveEvent(QEvent *event)
{
    m_mouseHeld = false;
    selectionModel()->clear();
    m_hoverIndex = QModelIndex();
    QAbstractItemView::leaveEvent(event);
}

void TracklistView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QAbstractItemView::selectionChanged(selected,deselected);
    viewport()->update();
}

void TracklistView::dropEvent(QDropEvent *event)
{
    m_hoverIndex = indexAt(m_mouseEnd);
    QModelIndex index = indexAt(event->position().toPoint());
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
    selectionModel()->clear();
    isDragging = false;
    m_mouseHeld = false;


}

void TracklistView::dragMoveEvent(QDragMoveEvent *event)
{
    m_mouseEnd = event->pos();
    m_hoverIndex = indexAt(m_mouseEnd);
    if (event->source() != this || !(event->possibleActions() & Qt::MoveAction))
        return;


    QAbstractItemView::dragMoveEvent(event);

}

void TracklistView::resizeEvent(QResizeEvent *event)
{
    updateScrollBars();
    QAbstractItemView::resizeEvent(event);
}

void TracklistView::showEvent(QShowEvent *event)
{
    updateScrollBars();
    QAbstractItemView::showEvent(event);
}

void TracklistView::scrollContentsBy(int dx, int dy)
{
    m_scrollOffset -= QPoint(dx, dy);
    QAbstractItemView::scrollContentsBy(dx, dy);
    updateEditorGeometries();

    emit scrolled(dx,dy);
}

void TracklistView::updateEditorGeometries()
{
    QAbstractItemView::updateEditorGeometries();

    for (int i = 0; i < model()->rowCount(); ++i){
        QModelIndex trackIndex = model()->index(i,0);

        QWidget *editor = indexWidget(trackIndex);
        if(!editor){
            continue;
        }

        QRect rect  = editor->rect();
        QPoint topInView = editor->mapToParent(rect.topLeft());
        if(topInView.y()<40){
            int offset = 40-topInView.y();
            editor->setMask(QRegion(0,offset,editor->width(),editor->height()));
        }

    }
}


