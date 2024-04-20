#include "timelinemodel.h"

#include "clipmodel.h"
#include "trackmodel.h"
TimelineModel::TimelineModel()
{

}

void TimelineModel::addClip(int trackIndex)
{

    TrackModel* track = m_tracks.at(trackIndex);
    if (!track) {
        // Further validation if necessary, depending on your application's logic.
        return;
    }

    QModelIndex parentIndex = index(trackIndex, 0, QModelIndex());
    int rows = rowCount(parentIndex);


    ClipModel* clip = new ClipModel(0,track);

    beginInsertRows(parentIndex,rows,rows);
    m_clips.push_back(clip);
    m_tracks.at(trackIndex)->addClip(clip);
    quint64 id = assignIdToClip(clip);

    endInsertRows();

}

void TimelineModel::createTrack()
{
    int rows = rowCount(QModelIndex());
    //

    TrackModel* track = new TrackModel(rowCount(QModelIndex()));

    beginInsertRows(QModelIndex(),rows,rows); //zero indexed
    m_tracks.push_back(track);
    quint64 id = assignIdToTrack(track);
    endInsertRows();
}

TrackModel *TimelineModel::findParentTrackOfClip(ClipModel *clip) const {
    if(!clip){
        qDebug("how clip not here???");
    }
    if(!clip->Parent())
        qDebug("how its not here??? %i",0);
    for (TrackModel* track : m_tracks) {
        if(std::find(track->getClips().cbegin(),track->getClips().cend(),clip)!=track->getClips().cend()){

            return track;
        }
    }
    qDebug("returning null");
    return nullptr;
}

int TimelineModel::findTrackRow(TrackModel *track) const {
    // Assuming yourTracksList is a QList<Track*>
    auto it = std::find(m_tracks.begin(),m_tracks.end(),track);

    return it-m_tracks.begin();
}

int TimelineModel::findClipRow(TrackModel *track, ClipModel *clip) const{
    std::vector<ClipModel*> clips = track->getClips();
    auto it = std::find(clips.begin(),clips.end(),clip);
    qDebug()<<it-clips.begin();
    return it-clips.begin();
}

QModelIndex TimelineModel::index(int row, int column, const QModelIndex &parent) const
{

    /*
    // Check if the index is valid within the model.
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    // If the parent is invalid, we are at the top level.
    // Assuming you want to treat top level differently, i.e., returning tracks.
    if (!parent.isValid()) {
        // Make sure the row number is valid for the tracks container.
        if (row >= 0 && row < m_tracks.size()) {
            TrackModel* track = m_tracks.at(row);
            // Use nullptr or a specific identifier as the third parameter if tracks don't have a parent.
            return createIndex(row, column, track); // Assuming tracks are at the top level.
        } else {
            return QModelIndex(); // Invalid row for tracks.
        }
    }

    // Below this point, we deal with children of tracks, presumably clips.
    if (row >= 0 && parent.row() < m_tracks.size()) {
        TrackModel* track = m_tracks.at(parent.row());
        const auto& clips = track->getClips(); // Store the result to avoid multiple calls.
        if (row < clips.size()) {
            ClipModel* clip = clips.at(row);
            return createIndex(row, column, clip);
        }
    }

    // Return an invalid QModelIndex if we can't match the conditions above.
    return QModelIndex();
*/


    //not in model
    if (hasIndex(row, column, parent)==false) {
        return QModelIndex();
    }

    TrackModel* track;


    //track item
    if(parent.isValid()==false){
        TrackModel* track = m_tracks.at(row);
        if(track){
            quint64 trackID = findID(track);

            return createIndex(row, column,  quintptr(trackID));
        }
            //return createIndex(row,column)
            //return createIndex(row,column, track);
        qDebug("TimelineModel::index - could not make track at row: %i",row);
        return QModelIndex();
    }

    track = m_tracks.at(parent.row());
    //track = static_cast<TrackModel*>(parent.internalPointer());
    if(!track){
        qDebug()<<"NOT VALID";
        return QModelIndex();
    }

    ClipModel* clip = track->getClips().at(row);

    if(clip){
        quint64 clipID = findID(clip);
        //row = findClipRow(track,clip);
        return createIndex(row, column, quintptr(clipID));
    }
    qDebug("CANT MAKE CLIPZ INDEX");

    return QModelIndex();
}

QModelIndex TimelineModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return {QModelIndex()};

    //QModelIndex().internalId();



    //ClipModel* clip = static_cast<ClipModel*>(child.internalPointer());

    quint64 id = child.internalId();

    if(isTrack(id))
        return QModelIndex();

    if(isClip(id)){
        ClipModel* clip = (ClipModel*)(FromID(id));

        if (!clip) {
            qDebug("TimelineModel::parent - could not make child at row: %i",child.row());
            return QModelIndex();
        }
        TrackModel* track = findParentTrackOfClip(clip);

        if (!track) {
            // If the clip doesn't have an associated track, return an invalid QModelIndex.
            qDebug("TimelineModel::parent - could not make track at row: %i",0);
            return QModelIndex();
        }


        //Q_ASSERT(parentItem);
        qDebug("number clips on track %i : %zu", findTrackRow(track),track->getClips().size());
        return createIndex(findTrackRow(track), 0,quintptr(findID(track)));
    }


    return QModelIndex();


}



int TimelineModel::rowCount(const QModelIndex &parent) const
{

    TrackModel* track;
    // Case 1: Top-level items (tracks)
    if (!parent.isValid()) {
        return m_tracks.size();
    }


    //if parent of clip
    quint64 id = parent.internalId();

    if(isTrack(id)){
        track = (TrackModel*)FromID(id);//static_cast<TrackModel*>(parent.internalPointer());
        if(!track){
            qDebug("NOT A TRAZK");
            return 1;
        }

        //track = m_tracks.at(parent.row());

        return track->getClips().size();
    }

    if(isClip(id)){
        return 0;
    }



    //Q_UNUSED(parent);
    return 1;

}

int TimelineModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if(!index.parent().isValid()){
        return "track";
    }
    return "clip";
}


/*QHash<int, QByteArray> TimelineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DurationRole] = "duration";
    roles[TrackNumberRole] = "trackNumber";
    roles[SelectedRole] = "selected";
    roles[InRole] = "in";
    roles[OutRole] = "out";


    return roles;
}*/
