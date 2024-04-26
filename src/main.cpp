#include "qtreeview.h"
#include "timelinemodel.h"
#include "timelinewidget.h"
#include "timelineview.h"

#include <QApplication>
#include <QToolBar>
#include<QStyleFactory>
int main(int argc, char *argv[])
{
    QColor bg = QColor("#262626");
    QColor fill = QColor("#202020");
    QColor seperator = QColor("#313131");

    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("fusion"));
    //TimelineWidget w;
    //w.show();

    TimelineModel* model = new TimelineModel();
    model->createTrack();
    model->createTrack();
    model->createTrack();
    model->createTrack();
    model->createTrack();

    model->createTrack();
    model->createTrack();
    model->createTrack();

    // track pos in out
    model->addClip(0,0,0,30);
    model->addClip(1,20,0,30);
    model->addClip(1,51,0,30);

    model->addClip(2,5,0,30);
    model->addClip(2,40,0,25);
    model->addClip(2,80,0,20);

    model->addClip(3,70,0,30);
    model->addClip(5,60,0,30);
    model->addClip(5,100,0,30);

    model->addClip(4,80,0,30);
    model->addClip(4,131,0,25);
    model->addClip(4,165,0,50);


    //model->add(0);
    TimelineView* view = new TimelineView();
    view->resize(880,230);
    //view->setSelectionMode(QAbstractItemView::NoSelection);
    //view->setHeaderHidden(true);

    view->setModel(model);
    view->show();

    QToolBar* toolbar = new QToolBar("zoom slider",view);
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(2, 50);
    slider->setValue(5);
    toolbar->addWidget(slider);
    toolbar->show();

    QObject::connect(slider,&QSlider::valueChanged,view,&TimelineView::setScale);

    return a.exec();
}
