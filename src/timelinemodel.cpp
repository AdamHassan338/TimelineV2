#include "timelinemodel.h"

#include "clipmodel.h"
#include "trackmodel.h"
#include <stdlib.h>
#include <string>
#include <QMimeData>
#include <QIODevice>
#include <algorithm>
TimelineModel::TimelineModel()
{

}

void TimelineModel::addClip(int trackIndex, int pos, int in, int out)
{

    TrackModel* track = m_tracks.at(trackIndex);
    if (!track) {
        return;
    }

    QModelIndex parentIndex = index(trackIndex, 0, QModelIndex());


    ClipModel* clip = new ClipModel(pos,in,out,track);

    insertClip(clip,parentIndex);

    reCalculateLength();

    emit timelineUpdated();


    return;

}

void TimelineModel::deleteClip(QModelIndex clipIndex)
{
    if(m_clipIDs.find(clipIndex.internalId())==m_clipIDs.cend())
        return;

    if(!clipIndex.isValid())
        return;

    beginRemoveRows(clipIndex.parent(),clipIndex.row(),clipIndex.row());
    quint64 id = clipIndex.internalId();
    {
        std::unordered_map<quint64, void*>::iterator it = m_idToObjectMap.begin();
        ClipModel* clip = (ClipModel*)FromID(clipIndex.internalId());

        m_tracks.at(clipIndex.parent().row())->removeClip(clip);

        auto clipsIt = std::find(m_clips.begin(),m_clips.end(),clip);
        if(clipsIt!=m_clips.end())
            m_clips.erase(clipsIt);

        delete clip;

        for(;it!=m_idToObjectMap.end();){
            if(it->first==id){
                it = m_idToObjectMap.erase(it);
                break;
            }
            else {
                ++it;
            }
        }
    }


    for (auto it = m_clipIDs.begin(); it != m_clipIDs.end();) {
        if (*it == id) {
            it = m_clipIDs.erase(it);
            break;
        } else {
            ++it;
        }
    }


    endRemoveRows();


}

void TimelineModel::insertClip(ClipModel *clip, QModelIndex parentTrackIndex){

    int rows = rowCount(parentTrackIndex);

    beginInsertRows(parentTrackIndex,rows,rows);
    m_clips.push_back(clip);
    m_tracks.at(parentTrackIndex.row())->addClip(clip);
    quint64 id = assignIdToClip(clip);

    endInsertRows();
}

void TimelineModel::moveClipToTrack(QModelIndex clipIndex, QModelIndex newTrackIndex)
{
    if(!clipIndex.isValid()){
        qDebug()<<"invalid clipindex when moving clip to track";
        return;
    }
    const QModelIndex parentTrackIndex = clipIndex.parent();

    if(!parentTrackIndex.isValid()){
        qDebug()<<"invalid parentTrackIndex when moving clip to track";
        return;
    }

    if(!newTrackIndex.isValid() ){
        return;

    }

    //if index is of a clip
    if(newTrackIndex.parent().isValid()){
        return;
    }

    //index is for a new track
    if(!hasIndex(newTrackIndex.row(),newTrackIndex.column(),newTrackIndex.parent())){
        if(newTrackIndex!=QModelIndex()){
            createTrack();
            int x = rowCount(QModelIndex());
            newTrackIndex = index(rowCount(QModelIndex())-1,columnCount(QModelIndex())-1,QModelIndex());
        }
    }



    TrackModel* parentTrack = (TrackModel*)FromID(parentTrackIndex.internalId());
    ClipModel* clip = (ClipModel*)FromID(clipIndex.internalId());
    TrackModel* newTrack = (TrackModel*)FromID(newTrackIndex.internalId());

    beginMoveRows(parentTrackIndex,clipIndex.row(),clipIndex.row(),newTrackIndex,rowCount(newTrackIndex));

    parentTrack->removeClip(clip);
    newTrack->addClip(clip);

    endMoveRows();

    emit timelineUpdated();


}

void TimelineModel::reCalculateLength()
{
    for(const ClipModel* clip : m_clips){
        m_length =  std::max(m_length,clip->out() - clip->in() + clip->pos());
    }

}

void TimelineModel::cutClip(QModelIndex clipIndex, int cutFrame)
{
    //IF SELECTED IS NOT A CLIP JUST IN CASE
    if(!clipIndex.isValid() || !clipIndex.parent().isValid() )
        return;


    //ClipModel* copy = (ClipModel*)malloc(sizeof(ClipModel));
    //memcpy(copy,FromID(clipIndex.internalId()),sizeof(ClipModel));


    // original in = in; original out = cutframe-1-pos + in;
    // copy pos = cutfrmame; copy in = original out + 1


    // calculate new in out and positions
    ClipModel* copy = new ClipModel(*(ClipModel*)FromID(clipIndex.internalId()));
    int offset = copy->pos();
    int originalIn = copy->in();
    int originalOut = copy->out();
    int length = copy->out() - copy->in();

    int originalNewOut = cutFrame-offset-1  + originalIn;

    ((ClipModel*)FromID(clipIndex.internalId()))->setOut(originalNewOut);


    copy->setIn(originalNewOut+1);
    copy->setPos(cutFrame);


    QModelIndex parentIndex = clipIndex.parent();

    insertClip(copy,parentIndex);


    emit timelineUpdated();



}

void TimelineModel::createTrack()
{
    int rows = rowCount(QModelIndex());

    TrackModel* track = new TrackModel(rowCount(QModelIndex()));

    beginInsertRows(QModelIndex(),rows,rows); //zero indexed
    m_tracks.push_back(track);
    quint64 id = assignIdToTrack(track);
    endInsertRows();

    emit timelineUpdated();
}

TrackModel *TimelineModel::findParentTrackOfClip(ClipModel *clip) const {
    if(!clip){
        qDebug("Clip not in Model");
    }
    if(!clip->Parent())
        qDebug("Clip HAS NO PARENT?! %i",0);
    for (TrackModel* track : m_tracks) {
        if(std::find(track->getClips().cbegin(),track->getClips().cend(),clip)!=track->getClips().cend()){

            return track;
        }
    }
    qDebug("returning null");
    return nullptr;
}

int TimelineModel::findTrackRow(TrackModel *track) const {
    auto it = std::find(m_tracks.begin(),m_tracks.end(),track);

    return it-m_tracks.begin();
}

int TimelineModel::findClipRow(TrackModel *track, ClipModel *clip) const{
    std::vector<ClipModel*> clips = track->getClips();
    auto it = std::find(clips.begin(),clips.end(),clip);
    qDebug()<<it-clips.begin();
    return it-clips.begin();
}

QModelIndex TimelineModel::createFakeIndex()
{
        return createIndex(rowCount(QModelIndex()),columnCount(QModelIndex()),nullptr);

}

void TimelineModel::movePlayhead(int dx)
{
    if(playheadPos>=0)
        playheadPos+=dx;
    if(playheadPos>m_length)
        playheadPos-=dx;
    if(playheadPos<0)
        playheadPos=0;

    qDebug()<<"frame: " <<playheadPos;
    emit timelineUpdated();
}

void TimelineModel::moveTrack(QModelIndex track, QModelIndex dest)
{

}

int TimelineModel::getPlayheadPos() const
{
    return playheadPos;
}

void TimelineModel::setPlayheadPos(int newPlayheadPos)
{
    if(newPlayheadPos>m_length)
        return;
    playheadPos = newPlayheadPos;

    emit timelineUpdated();
}

QStringList TimelineModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.track";
    return types;
   //return QAbstractItemModel::mimeTypes();
}

QMimeData *TimelineModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            //int track = data(index,TrackNumberRole ).toInt(); // Assuming tracks are stored as QString
            int track = index.row();
            stream << track;
        }
    }

    mimeData->setData("application/vnd.track", encodedData);
    return mimeData;
}

bool TimelineModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(parent);

    if (!data->hasFormat("application/vnd.track"))
        return false;

    if (column > 0)
        return false;

    return true;

}

bool TimelineModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    int newRow = parent.row();

   /* if(parent.row() ==-1){
        newRow=0;
    }//else if(!parent.isValid()){
       // newRow=0;}
    else if(!hasIndex(parent.row(),parent.column(),parent.parent())){
        newRow = parent.row()-1;
    }
    if(row!=-1){
        newRow =row;
    }*/

    newRow = parent.row();
    if(!parent.isValid()){
        newRow=0;
    }
    if(!hasIndex(parent.row(),parent.column(),parent.parent())){
         newRow = parent.row()-1;
    }

    QByteArray encodedData = data->data("application/vnd.track");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<int> newItems;
    int rows = 0;

    while (!stream.atEnd()) {
        int track;
        stream >> track;
        newItems << track;
        ++rows;
    }

    for (int &track : newItems) {

        if (track < 0 || track >= m_tracks.size() || newRow < 0 || newRow > m_tracks.size()) {
            return false;
        }

        if(track + 1 == newRow){
            std::swap(track,newRow);
        }

        if(track==newRow)
            return false;
        beginMoveRows(QModelIndex(),track,track,QModelIndex(),newRow);
        std::vector<TrackModel*> itemsToMove;
        itemsToMove.push_back(m_tracks.at(track));
        m_tracks.erase(m_tracks.begin()+ track);
        m_tracks.insert(m_tracks.begin() + newRow,itemsToMove.begin(),itemsToMove.end());
        endMoveRows();


    }
    return true;


}

Qt::DropActions TimelineModel::supportedDropActions() const
{
return Qt::MoveAction;
}

Qt::ItemFlags TimelineModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) |= Qt::ItemIsDropEnabled;
    //return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex TimelineModel::index(int row, int column, const QModelIndex &parent) const
{

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

        qDebug("TimelineModel::index - could not make track at row: %i",row);
        return QModelIndex();
    }

    track = m_tracks.at(parent.row());
    if(!track){
        qDebug()<<"NOT VALID";
        return QModelIndex();
    }

    ClipModel* clip = track->getClips().at(row);

    if(clip){
        quint64 clipID = findID(clip);
        return createIndex(row, column, quintptr(clipID));
    }
    qDebug("CANT MAKE CLIP INDEX");

    return QModelIndex();
}

QModelIndex TimelineModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) return {QModelIndex()};

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
        //qDebug("number clips on track %i : %zu", findTrackRow(track),track->getClips().size());
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
        track = (TrackModel*)FromID(id);
        if(!track){
            qDebug("NOT A TRAZK");
            return 1;
        }

        return track->getClips().size();
    }

    if(isClip(id)){
        return 0;
    }


    return 1;

}

int TimelineModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        if(role==TimelineLengthRole)
            return QVariant::fromValue(m_length);
        return QVariant();
    }


    //if (role != Qt::DisplayRole)
    //    return QVariant();

    //needed due to fake index created by TimelineModel::indexAt, tiggered by tooltips etc
    if(!hasIndex(index.row(),index.column(),index.parent())){
        return QVariant();
    }

    if(!index.parent().isValid()){
        TrackModel* track;
        if(role==Qt::ToolTipRole){
            track = (TrackModel*)FromID(index.internalId());
            return QVariant::fromValue("track " + QString::number(index.row()));
        }

        if(role ==TrackNumberRole){
            track = (TrackModel*)FromID(index.internalId());
            return QVariant::fromValue(index.row());
        }
        return QVariant();
    }
    ClipModel* clip;
    switch (role){
    case ClipInRole:
        clip = (ClipModel*)FromID(index.internalId());
        return QVariant::fromValue(clip->in());
        break;
    case ClipOutRole:
        clip = (ClipModel*)FromID(index.internalId());
        return QVariant::fromValue(clip->out());
        break;
    case ClipPosRole:
        clip = (ClipModel*)FromID(index.internalId());
        return QVariant::fromValue(clip->pos());
        break;
    case ClipLengthRole:
        clip = (ClipModel*)FromID(index.internalId());
        return QVariant::fromValue(clip->length());
        return true;
        break;
    defualt:
        return "clip";
    }
    return "null";
}

bool TimelineModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if (!index.isValid()){
        return false;
    }



    if(!index.parent().isValid()){
        TrackModel* track;
        if(role==Qt::ToolTipRole){
            track = (TrackModel*)FromID(index.internalId());
        }
        return true;
    }
    ClipModel* clip;
    switch (role){
    case ClipInRole:
        clip = (ClipModel*)FromID(index.internalId());
        clip->setIn(value.toInt());
        emit timelineUpdated();
        return true;
        break;
    case ClipOutRole:
        clip = (ClipModel*)FromID(index.internalId());
        clip->setOut(value.toInt());
        emit timelineUpdated();
        return true;
        break;
    case ClipPosRole:
        clip = (ClipModel*)FromID(index.internalId());
        clip->setPos(value.toInt());
        emit timelineUpdated();
        return true;
        break;
    defualt:
        return false;;
    }

    return QAbstractItemModel::setData(index,value,role);
}


QHash<int, QByteArray> TimelineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ClipInRole] = "clipIn";
    roles[ClipOutRole] = "clipOut";
    roles[ClipPosRole] = "clipPos";
    roles[TrackNumberRole] = "trackNumber";
    roles[SelectedRole] = "selected";
    roles[InRole] = "in";
    roles[OutRole] = "out";
    roles[TimelineLengthRole] = "timelineLength";


    return roles;
}
