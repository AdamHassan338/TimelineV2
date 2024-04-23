#include "timelinewidget.h"
#include "./ui_timelinewidget.h"

#include "QTreeView"

#include "timelinemodel.h"

TimelineWidget::TimelineWidget(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::TimelineWidget)
{
    ui->setupUi(this);


}

TimelineWidget::~TimelineWidget()
{
    delete ui;
}

