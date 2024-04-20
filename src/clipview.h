#ifndef CLIPVIEW_H
#define CLIPVIEW_H

#include <QWidget>

class ClipView : public QWidget
{
    Q_OBJECT
public:
    explicit ClipView(QWidget *parent = nullptr);

    int width;
    int heigth;
    int start;
    QString m_name;
    QRect rect;

private:


signals:


    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CLIPVIEW_H
