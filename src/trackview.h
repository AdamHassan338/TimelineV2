#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QFrame>
class ClipView;

class TrackView : public QFrame
{
    Q_OBJECT
public:
    explicit TrackView(int number,int width, int height, QWidget *parent = nullptr);

    QRect m_rect;

//private:

    int m_number;
    int m_width;
    int m_height;

signals:


    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // TRACKVIEW_H
