#ifndef GDRIVEDATABASESYNC_H
#define GDRIVEDATABASESYNC_H
#include "core/Database.h"
#include <QtCore/QDebug>
#include "../core/Tools.h"
#include "GDriveDatabaseSyncBase.h"
#include "../core/Metadata.h"
using namespace DatabaseSyncObject;

//generalize Entry/Group to template SO - sync object
template<class SO>
class GDriveDatabaseSync : public GDriveDatabaseSyncBase
{
protected:
    Database* db1;
    Database* db2;
    QMap<Uuid,SO*> entries1;
    QMap<Uuid,SO*> entries2;

    void syncEntry(SO* localEntry,SO* cloudEntry);
    void updateEntryData(SO *entry, SO *new_data);
    void syncLocation(SO* localEntry,SO* cloudEntry);
    void updateEntryGroup(SO *entry, SO *new_data);
    static bool compareByCreationTime(SO *entry1, SO *entry2);

    void addMissingEntries(QList<SO *>missingEntries);
    virtual bool processEntry(Database *db, SO* entry)=0;
    virtual void setParentGroup(SO* entry, Group *group)=0;
    virtual void removeEntry(SO* entry)=0;
    virtual const Group* getParentGroup(SO* entry)=0;
    virtual const QString getEntryName(SO* entry) = 0;
    virtual  QMap<Uuid,SO*> getEntriesMap(Database* db)=0;
    virtual QString getType()=0;
    virtual ObjectType getObjectType()=0;

public:
    GDriveDatabaseSync(Database* db1, Database* db2);
    QSharedPointer<SyncObject> syncDatabases();
    void setSyncObject(QSharedPointer<SyncObject> syncObject);
};

#endif // GDRIVEDATABASESYNC_H
