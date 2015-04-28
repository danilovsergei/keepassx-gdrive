#ifndef GROUPSSYNC_H
#define GROUPSSYNC_H

#include "core/Database.h"
#include "core/Group.h"
#include "core/Entry.h"
#include "core/Tools.h"
#include "core/Metadata.h"
#include "DatabaseSync.h"
#include <QtCore/QDebug>
#include <QtCore/QObject>
using namespace DatabaseSyncObject;
class GroupsSync : public DatabaseSync<Group>
{
public:
    GroupsSync(Database *db1, Database *db2);
    ~GroupsSync();

    /**
     * @brief syncDatabases - syncs groups between two provided databases
     * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
     * @param db2 - second source database.
     */
    QSharedPointer<SyncObject> syncDatabases();
protected:
    bool processEntry(Database *db,Group* entry);
    void removeEntry(Group* entry);
    void setParentGroup(Group* entry, Group* group);
    ObjectType getObjectType();
    const Group* getParentGroup(Group* entry);
    QMap<Uuid,Group*> getEntriesMap(Database* db);
    const QString getEntryName(Group* entry);
    QString getType();
private:
    const QString ENTRY_TYPE="Group";

};

#endif //GROUPSSYNC_H
