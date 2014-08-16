#ifndef GDRIVEDATABASESYNC_H
#define GDRIVEDATABASESYNC_H
#include "core/Database.h"
#include <QtCore/QDebug>
#include "../core/Tools.h"
#include "../core/Metadata.h"
namespace DatabaseSync {
struct GDriveSyncObject {
    //groups/entries missed in local database, but present in remote database
    int resultLocalMissingEntries=0;
    int resultLocalMissingGroups=0;

    //groups/entries present in local database but missing in remote database
    int resultRemoteMissingGroups=0;
    int resultRemoteMissingEntries=0;



    //groups/entries which have older timestamp in local database
    int resultLocalOlderEntries=0;
    int resultLocalOlderGroups=0;

    void setOlderEntries(int value);
    //groups/entries which have older timestamp in remote database
    int resultRemoteOlderEntries=0;
    int resultRemoteOlderGroups=0;

    //groups/entries which were deleted in local db
    int resultLocalRemovedEntries=0;
    int resultLocalRemovedGroups=0;


    //groups/entries which were deleted in remote db
    int resultRemoteRemovedEntries=0;
    int resultRemoteRemovedGroups=0;
};

class GDriveDatabaseSync:public QObject
{
    Q_OBJECT
protected:
    Database* db1;
    Database* db2;
    QSharedPointer<GDriveSyncObject> syncObject;
    QMap<Uuid,Group*> entries1;
    QMap<Uuid,Group*> entries2;
    virtual void increaseRemoteOlderEntries()=0;
    virtual void increaseRemoteMissingEntries()=0;
    virtual void increaseRemoteRemovedEntries()=0;

    virtual void increaseLocalOlderEntries()=0;
    virtual void increaseLocalMissingEntries()=0;
    virtual void increaseLocalRemovedEntries()=0;


    template <typename T>
    void syncEntry(T* localEntry,T* cloudEntry);
    template <class T>
    void updateEntryData(T *entry, T *new_data);
    template <typename T>
    void syncLocation(T* localEntry,T* cloudEntry);

    template <typename T>
    void updateEntryGroup(T *entry, T *new_data);
    template <class T> void removeEntry(T* entry);


public:
    GDriveDatabaseSync(Database* db1, Database* db2);
    virtual QSharedPointer<GDriveSyncObject> syncDatabases()=0;
    virtual ~GDriveDatabaseSync() {}


    QSharedPointer<GDriveSyncObject> getSyncObject();



};

}

#endif // GDRIVEDATABASESYNC_H
