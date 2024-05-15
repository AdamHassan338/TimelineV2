#ifndef CLIPMODEL_H
#define CLIPMODEL_H
#include "types.h"

class TrackModel;


class ClipModel
{
public:
    ClipModel(int pos,int in, int out,TrackModel* parent,MediaType type = MediaType::VIDEO);

    //copy constructor
    ClipModel(const ClipModel& other)
        : m_pos(other.m_pos), m_in(other.m_in), m_out(other.m_out), m_parent(other.m_parent), m_length(other.m_length), m_type(other.m_type) {
    }

    //copy assignment operator
    ClipModel& operator=(const ClipModel& other) {
        if (this != &other) {
            m_pos = other.m_pos;
            m_in = other.m_in;
            m_out = other.m_out;
            m_parent = other.m_parent;
            m_length = other.m_length;
            m_type = other.m_type;

        }
        return *this;
    }

    TrackModel* Parent(){
        return m_parent;
    };



    int pos() const;
    void setPos(int newPos);

    void setParent(TrackModel *newParent);
    int in() const;
    void setIn(int newIn);
    int out() const;
    void setOut(int newOut);

    ~ClipModel(){

    }

    int length() const;

    MediaType type() const;

private:
    TrackModel* m_parent;
    MediaType m_type;
    //store resoruce here
    int m_pos;//position on timeline of first frame
    int m_in;//start time of clip from resource
    int m_out;//end time of clip from resource
    int m_length;//length of orignal media


};

#endif // CLIPMODEL_H
