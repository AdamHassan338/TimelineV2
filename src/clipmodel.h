#ifndef CLIPMODEL_H
#define CLIPMODEL_H

class TrackModel;

class ClipModel
{
public:
    ClipModel(int pos,TrackModel* parent);

    TrackModel* Parent(){
        return m_parent;
    };

private:
    TrackModel* m_parent;
    //store resoruce here
    int m_pos;//position on timeline of first frame
    int m_in;//start time of clip from resource
    int m_out;//end time of clip from resource


};

#endif // CLIPMODEL_H
