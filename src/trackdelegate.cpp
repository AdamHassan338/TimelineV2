#include "trackdelegate.h"

#include<QPainter>
#include <QPushButton>
#include <QLayout>
#include <QSpacerItem>
#include <QLabel>
TrackDelegate::TrackDelegate(QObject *parent)
    : QAbstractItemDelegate{parent}
{}

void TrackDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int trackHeight = 25;
    int rulerHeight = 40;

    QColor bgColour = QColor("#262626");
    QColor fillColour = QColor("#202020");
    QColor seperatorColour = QColor("#313131");
    QColor rulerColour = QColor("#4F4F4F");

    painter->save();

    painter->setPen(fillColour);
    painter->setBrush(fillColour.lighter(150));

    if(option.state & QStyle::State_MouseOver)
        painter->setBrush(fillColour);
    painter->drawRect(option.rect);
    painter->save();
    if(option.state & QStyle::State_MouseOver && option.state & QStyle::State_Raised)
        painter->setPen(QPen(fillColour.darker(200),4));
    painter->drawLine(0, option.rect.top(), option.rect.width(), option.rect.top());

    painter->restore();
    painter->setPen(Qt::white);
    painter->drawText(option.rect,Qt::AlignLeft,option.text);


    painter->restore();



}

QSize TrackDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return option.rect.size();

}

QWidget *TrackDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget* editor = new QWidget(parent);
    QHBoxLayout* layout = new QHBoxLayout(editor);

layout->setContentsMargins(5, 5, 5, 5);
    QPushButton* a = new QPushButton();
    a->setMaximumHeight(option.rect.height());
    a->setMaximumWidth(option.rect.height());
    QPushButton* b = new QPushButton();
    b->setMaximumHeight(option.rect.height()*.9);
    b->setMaximumWidth(option.rect.height()*.9);



    QSpacerItem* leftSpacer = new QSpacerItem(1,1,QSizePolicy::Expanding, QSizePolicy::Minimum);
    QLabel* spacerPlaceholder = new QLabel();
    spacerPlaceholder->setStyleSheet("background-color: red;");
    spacerPlaceholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(leftSpacer);
    a->setText("M");
    b->setText("S");
    //layout->addWidget(spacerPlaceholder);
    layout->addWidget(a);
    layout->addWidget(b);

    editor->show();
    editor->setMouseTracking(true);

    return editor;

}

void TrackDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int trackHeight = 25;
    int rulerHeight = 40;
    editor->setGeometry(option.rect);
    QRegion mask = QRegion(0,rulerHeight,option.rect.right(),option.rect.bottom()-rulerHeight);
    editor->clearMask();

}

