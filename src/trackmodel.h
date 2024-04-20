#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QObject>

class TimelineModel;
class ClipModel;
class TrackModel
{

public:
    explicit TrackModel(int number);
    TrackModel() = default;

    void addClip(ClipModel* clip);
    int m_number;

    const std::vector<ClipModel*>& getClips(){return m_clips;};

private:

    int m_id;

    TimelineModel* m_parent;
    std::vector<ClipModel*> m_clips;

};

#endif // TRACKMODEL_H
