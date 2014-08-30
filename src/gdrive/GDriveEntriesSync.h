#ifndef GDRIVEENTRIESSYNC_H
#define GDRIVEENTRIESSYNC_H
#include "../core/Database.h"
#include "../core/Group.h"
#include "../core/Entry.h"
#include "../core/Tools.h"
#include "../core/Metadata.h"
#include  "GDriveGroupsSync.h"
#include "QtCore/QSharedPointer"
using namespace DatabaseSync;
class GDriveEntriesSync:public GDriveDatabaseSync<Entry>
{
public:
    GDriveEntriesSync(Database* db1, Database* db2, bool syncGroups=true);
    ~GDriveEntriesSync();
    QSharedPointer<GDriveSyncObject> syncDatabases();
protected:
    void removeEntry(Entry* entry);
    bool processEntry(Database *db,Entry* entry);
    void setParentGroup(Entry* entry, Group* group);
    const Group* getParentGroup(Entry* entry);
    const QString getEntryName(Entry* entry);
    QMap<Uuid,Entry*> getEntriesMap(Database* db);
    QString getType();
    QSharedPointer<SyncEntry> getResultStat();

private:
    bool syncGroups;
    const QString ENTRY_TYPE="Entry";


};

#endif // GDRIVEENTRIESSYNC_H
