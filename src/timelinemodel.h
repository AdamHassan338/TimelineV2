#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QAbstractItemModel>
#include <unordered_set>
#include <unordered_map>
#include "trackmodel.h"
#include "clipmodel.h"

class TimelineModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    TimelineModel();

    void addClip(int trackIndex, int pos, int in, int out);
    void deleteClip(QModelIndex clipIndex);

    int moveClipToTrack(QModelIndex clipIndex,QModelIndex newTrackIndex);
    //re calcualtes the length of timeline in frames
    void reCalculateLength();
    std::vector<TrackModel*> m_tracks;
    std::vector<ClipModel*> m_clips;

    //cut action key is Qt::Key_S
    void cutClip(QModelIndex clipIndex, int cutFrame);

    void createTrack(MediaType type = MediaType::VIDEO);
    void* FromID(quint64 id) const{
        return m_idToObjectMap.at(id);
    }
    //function for creating fake index, used when creating a new track. one that isnt QModelIndex() but is still invalid
    QModelIndex createFakeIndex();
    void movePlayhead(int dx);

    void moveTrack(QModelIndex track,QModelIndex dest);



signals:
    void timelineUpdated();
    void newClip(int row,int track);
    void trackMoved(int oldIndex,int newIndex);
    void playheadMoved(int frame);

    void tracksChanged();

private:
    //length of the timeline, grows automaticly with clips
    int m_length = 0;
    int playheadPos = 0;

    enum ItemType {
        Track,
        Clip,
        Invalid
    };

    void insertClip(ClipModel* clip,QModelIndex parentTrackIndex);


    TrackModel* findParentTrackOfClip(ClipModel* clip) const;

    int findTrackRow(TrackModel* track) const;

    int findClipRow(TrackModel* track,ClipModel* clip) const;

    quint64 assignIdToTrack(TrackModel* track){
        quint64 id = nextId++;
        m_idToObjectMap[id] = track;
        m_trackIDs.insert(id);
        return id;
    };
    quint64 assignIdToClip(ClipModel* clip){
        quint64 id = nextId++;
        m_idToObjectMap[id] = clip;
        m_clipIDs.insert(id);
        return id;
    };

    quint64 findID(void* object) const{
        auto it = m_idToObjectMap.cbegin();
        while(it!= m_idToObjectMap.cend()){
            if(it->second == object){
                return it->first;
            }
            it++;
        }
        return quint64(0);
    };



    bool isClip(quint64 id) const{
        return getType(id)==Clip;
    }

    bool isTrack(quint64 id) const{
        return getType(id)==Track;
    }

    std::unordered_map<quint64, void*> m_idToObjectMap;
    std::unordered_set<quint64> m_trackIDs;
    std::unordered_set<quint64> m_clipIDs;


    quint64 nextId = 1;



    ItemType getType(quint64 id) const{
        if(m_trackIDs.find(id) != m_trackIDs.end()){
            return Track;
        }

        if(m_clipIDs.find(id) != m_clipIDs.end()){
            return Clip;
        }

        return Invalid;
    }





    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    //bool insertRows(int row, int count, const QModelIndex &parent) override;

    QHash<int, QByteArray> roleNames() const override;

    enum TimelineRoles {
        NameRole = Qt::UserRole + 1,
        ClipInRole,
        ClipOutRole,
        ClipPosRole,
        ClipLengthRole,
        ClipTypeRole,
        TrackNumberRole,
        TrackTypeRole,
        TimelineLengthRole,
        SelectedRole,
        InRole,
        OutRole

    };


    // QAbstractItemModel interface
public:
    int getPlayheadPos() const;
    void setPlayheadPos(int newPlayheadPos);

    // QAbstractItemModel interface
public:
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::DropActions supportedDropActions() const override;

    // QAbstractItemModel interface
public:

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // TIMELINEMODEL_H
