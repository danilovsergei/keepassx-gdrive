#ifndef DATABASESYNC_H
#define DATABASESYNC_H
#include "core/Database.h"
#include <QDebug>
#include "core/Tools.h"
#include "DatabaseSyncBase.h"
#include "core/Metadata.h"
using namespace DatabaseSyncObject;

// generalize Entry/Group to template SO - sync object
template<class SO>
class DatabaseSync : public DatabaseSyncBase
{


    void syncEntry(SO *localEntry, SO *cloudEntry);

    void syncLocation(SO *localEntry, SO *cloudEntry);
    void updateEntryGroup(SO *entry, SO *new_data);
    static bool compareByCreationTime(SO *entry1, SO *entry2);

    void addMissingEntries(QList<SO *> missingEntries);
    virtual bool processEntry(Database *db, SO *entry) = 0;
    virtual void setParentGroup(SO *entry, Group *group) = 0;
    virtual void removeEntry(SO *entry) = 0;
    virtual void updateEntryData(SO *entry, SO *new_data) = 0;
    virtual const Group *getParentGroup(SO *entry) = 0;
    virtual const QString getEntryName(SO *entry) = 0;
    virtual QMap<Uuid, SO *> getEntriesMap(Database *db) = 0;
    virtual QString getType() = 0;
    virtual ObjectType getObjectType() = 0;


public:
    DatabaseSync(Database *db1, Database *db2);
    QSharedPointer<SyncObject> syncDatabases();
    QSharedPointer<SyncObject> syncEntries();
    void setSyncObject(QSharedPointer<SyncObject> syncObject);
protected:
    QMap<Uuid, SO *> entries1;
    QMap<Uuid, SO *> entries2;

};

#endif // DATABASESYNC_H
