#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QFrame>

class TrackView;

QT_BEGIN_NAMESPACE
namespace Ui { class TimelineWidget; }
QT_END_NAMESPACE

class TimelineWidget : public QFrame
{
    Q_OBJECT

public:
    TimelineWidget(QWidget *parent = nullptr);
    ~TimelineWidget();

private:
    Ui::TimelineWidget *ui;
};
#endif // TIMELINEWIDGET_H
