#include "trackmodel.h"

TrackModel::TrackModel(int number) : m_number(number)
{

}

void TrackModel::addClip(ClipModel *clip)
{
    m_clips.push_back(clip);
}

void TrackModel::removeClip(ClipModel *clip)
{
    auto it = std::find(m_clips.begin(), m_clips.end(), clip);
    if(it!=m_clips.end())
        m_clips.erase(it);

}

