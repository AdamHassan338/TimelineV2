#ifndef CLIPMODEL_H
#define CLIPMODEL_H

class TrackModel;

class ClipModel
{
public:
    ClipModel(int pos,int in, int out,TrackModel* parent);

    //copy constructor
    ClipModel(const ClipModel& other)
        : m_pos(other.m_pos), m_in(other.m_in), m_out(other.m_out), m_parent(other.m_parent) {
    }

    //copy assignment operator
    ClipModel& operator=(const ClipModel& other) {
        if (this != &other) {
            m_pos = other.m_pos;
            m_in = other.m_in;
            m_out = other.m_out;
            m_parent = other.m_parent;

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

private:
    TrackModel* m_parent;
    //store resoruce here
    int m_pos;//position on timeline of first frame
    int m_in;//start time of clip from resource
    int m_out;//end time of clip from resource


};

#endif // CLIPMODEL_H
