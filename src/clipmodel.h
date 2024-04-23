#ifndef CLIPMODEL_H
#define CLIPMODEL_H

class TrackModel;

class ClipModel
{
public:
    ClipModel(int pos,int in, int out,TrackModel* parent);

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

private:
    TrackModel* m_parent;
    //store resoruce here
    int m_pos;//position on timeline of first frame
    int m_in;//start time of clip from resource
    int m_out;//end time of clip from resource


};

#endif // CLIPMODEL_H
