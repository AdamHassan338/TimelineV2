#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QObject>
#include "types.h"

class TimelineModel;
class ClipModel;



class TrackModel
{

public:
    explicit TrackModel(int number, MediaType type = MediaType::VIDEO);
    TrackModel() = default;

    void addClip(ClipModel* clip);
    void removeClip(ClipModel* clip);
    int m_number;

    const std::vector<ClipModel*>& getClips(){return m_clips;}

    MediaType type() const;

private:

    int m_id;
    MediaType m_type;
    TimelineModel* m_parent;
    std::vector<ClipModel*> m_clips;

};

#endif // TRACKMODEL_H
