#include "timelinewidget.h"
#include "./ui_timelinewidget.h"

#include "clipview.h"
#include "trackview.h"
#include "QTreeView"

#include "timelinemodel.h"

TimelineWidget::TimelineWidget(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::TimelineWidget)
{
    ui->setupUi(this);
    //setFrameStyle(QFrame::Panel | QFrame::Raised);
    //rect().setRect(0,0,600,600);

    //TrackView* track = new TrackView(0,500,50,this);
    //ClipView* clip = new ClipView(this);


    //clip->heigth

}

TimelineWidget::~TimelineWidget()
{
    delete ui;
}

