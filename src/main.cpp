#include "qtreeview.h"
#include "timelinemodel.h"
#include "timelinewidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //TimelineWidget w;
    //w.show();

    TimelineModel* model = new TimelineModel();
    model->createTrack();
    model->createTrack();
    model->createTrack();
    model->createTrack();
    model->createTrack();
    model->addClip(0);
    model->addClip(1);
    model->addClip(1);

    model->addClip(2);
    model->addClip(2);
    model->addClip(2);
    //model->add(0);
    QTreeView* view = new QTreeView();
    view->setHeaderHidden(true);

    view->setModel(model);
    view->show();
    return a.exec();
}
