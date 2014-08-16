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
class GDriveGroupsSync : public GDriveDatabaseSync
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
    static bool compareByUuid(Group* entry1,Group* entry2);
    static bool compareByCreationTime(Group* entry1,Group* entry2);
private:
    void addMissingEntries(QList<Group*> missingEntries);
    void removeEntry(Group* entry);
    bool isItemMoved(Group* entry);
    template <typename T>
    void syncEntry(T* localEntry,T* cloudEntry);
    void increaseRemoteOlderEntries();
    void increaseRemoteMissingEntries();
    void increaseRemoteRemovedEntries();
    void increaseLocalOlderEntries();
    void increaseLocalMissingEntries();
    void increaseLocalRemovedEntries();





};

#endif // GDRIVEGROUPSSYNC_H
