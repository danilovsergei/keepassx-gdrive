#ifndef GDRIVEDATABASESYNCBASE_H
#define GDRIVEDATABASESYNCBASE_H
#include <QtCore/QSharedPointer>
#include <QtCore/QMap>
namespace DatabaseSync {
struct SyncEntry {
public:
    enum class ObjectType {Group=0,Entry=1};
    SyncEntry(ObjectType type);
    void increaseRemoteOlderEntries();
    void increaseRemoteMissingEntries();
    void increaseRemoteRemovedEntries();

    void increaseLocalOlderEntries();
    void increaseLocalMissingEntries();
    void increaseLocalRemovedEntries();
    int  getRemoteOlderEntries();
    int  getRemoteMissingEntries();
    int  getRemoteRemovedEntries();

    int  getLocalOlderEntries();
    int  getLocalMissingEntries();
    int  getLocalRemovedEntries();
private:
   ObjectType type;
    //groups/entries missed in local database, but present in remote database
   int resultLocalMissingEntries=0;
   //groups/entries present in local database but missing in remote database
   int resultRemoteMissingEntries=0;
   //groups/entries which have older timestamp in local database
   int resultLocalOlderEntries=0;
   //groups/entries which have older timestamp in remote database
   int resultRemoteOlderEntries=0;
   //groups/entries which were deleted in local db
   int resultLocalRemovedEntries=0;
   //groups/entries which were deleted in remote db
   int resultRemoteRemovedEntries=0;



};
struct GDriveSyncObject {
private:
    QMap<SyncEntry::ObjectType,QSharedPointer<SyncEntry> > data;
public:
    GDriveSyncObject();
    QSharedPointer<SyncEntry> getResultStat(SyncEntry::ObjectType entryType);

};
class GDriveDatabaseSyncBase
{
public:
    GDriveDatabaseSyncBase();
    virtual ~GDriveDatabaseSyncBase() {}
    /**
     * @brief syncDatabases - syncs items between two provided by derived class constructor databases
     */
    virtual QSharedPointer<GDriveSyncObject> syncDatabases()=0;
    virtual void setSyncObject(QSharedPointer<GDriveSyncObject> syncObject)=0;
};
}
#endif // GDRIVEDATABASESYNCBASE_H
