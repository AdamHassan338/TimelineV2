#include "qtreeview.h"
#include "timelinemodel.h"
#include "timelinewidget.h"
#include "timelineview.h"
#include "tracklistview.h"
#include "types.h"

#include <QApplication>
#include <QToolBar>
#include<QStyleFactory>
#include <QSplitter>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle(QStyleFactory::create("fusion"));
    qRegisterMetaType<MediaType>("MediaType");

    //TimelineWidget w;
    //w.show();
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    TimelineModel* model = new TimelineModel();
    model->createTrack(MediaType::VIDEO);
    model->createTrack(MediaType::AUDIO);
    //model->createTrack();
    //model->createTrack();
    //model->createTrack();
//
    //model->createTrack();
    //model->createTrack();
    //model->createTrack();

    // track pos in out
    model->addClip(0,0,0,30);
    model->addClip(1,0,0,30);
    //model->addClip(1,20,0,30);
    //model->addClip(1,51,0,30);
//
    //model->addClip(2,5,0,30);
    //model->addClip(2,40,0,25);
    //model->addClip(2,80,0,20);
//
    //model->addClip(3,70,0,30);
    //model->addClip(5,60,0,30);
    //model->addClip(5,100,0,30);
//
    //model->addClip(4,80,0,30);
    //model->addClip(4,131,0,25);
    //model->addClip(4,165,0,50);



    TimelineView* view = new TimelineView();
    view->resize(880,230);
    TracklistView* tracklist = new TracklistView();
    //view->setSelectionMode(QAbstractItemView::NoSelection);
    //view->setHeaderHidden(true);

    view->setModel(model);
    tracklist->setModel(model);
    //view->show();

    QToolBar* toolbar = new QToolBar("zoom slider",view);
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(2, 50);
    slider->setValue(5);
    toolbar->addWidget(slider);
    //toolbar->show();
    splitter->addWidget(tracklist);
    splitter->addWidget(view);
    splitter->setHandleWidth(0);
    QList<int> sizes({120,780});
    splitter->setMouseTracking(true);



    splitter->resize(880,230);
    splitter->setSizes(sizes);
    splitter->show();

    QObject::connect(slider,&QSlider::valueChanged,view,&TimelineView::setScale);
    QObject::connect(view,&TimelineView::scrolled,tracklist,&TracklistView::scroll);
    QObject::connect(tracklist,&TracklistView::scrolled,view,&TimelineView::scroll);

    QObject::connect(model,&TimelineModel::newClip,view,&TimelineView::addClipToMap);
    QObject::connect(model,&TimelineModel::trackMoved,view,&TimelineView::TrackMoved);

    QObject::connect(model,&TimelineModel::playheadMoved,tracklist,&TracklistView::setTime);
    QObject::connect(model,&TimelineModel::tracksChanged,tracklist,&TracklistView::updateViewport);

    return a.exec();

}
