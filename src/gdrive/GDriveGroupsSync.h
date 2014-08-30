#ifndef GDRIVEGROUPSSYNC_H
#define GDRIVEGROUPSSYNC_H

#include "../core/Database.h"
#include "../core/Group.h"
#include "../core/Entry.h"
#include "../core/Tools.h"
#include "../core/Metadata.h"
#include "GDriveDatabaseSync.h"
#include <QtCore/QDebug>
#include <QtCore/QObject>
using namespace DatabaseSync;
class GDriveGroupsSync : public GDriveDatabaseSync<Group>
{
public:
    GDriveGroupsSync(Database* db1, Database* db2);
    ~GDriveGroupsSync();

    /**
     * @brief syncDatabases - syncs groups between two provided databases
     * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
     * @param db2 - second source database.
     */
    QSharedPointer<GDriveSyncObject> syncDatabases();
protected:
    bool processEntry(Database *db,Group* entry);
    void removeEntry(Group* entry);
    void setParentGroup(Group* entry, Group* group);
    const Group* getParentGroup(Group* entry);
    QMap<Uuid,Group*> getEntriesMap(Database* db);
    const QString getEntryName(Group* entry);
    QString getType();
    QSharedPointer<SyncEntry> getResultStat();
private:
    const QString ENTRY_TYPE="Group";

};

#endif // GDRIVEGROUPSSYNC_H
