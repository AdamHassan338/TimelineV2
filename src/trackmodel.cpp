#include "trackmodel.h"

TrackModel::TrackModel(int number) : m_number(number)
{

}

void TrackModel::addClip(ClipModel *clip)
{
    m_clips.push_back(clip);
}
