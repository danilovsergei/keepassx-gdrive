#ifndef GDRIVEENTRIESSYNC_H
#define GDRIVEENTRIESSYNC_H
#include "../core/Database.h"
#include "../core/Group.h"
#include "../core/Entry.h"
#include "../core/Tools.h"
#include "../core/Metadata.h"
#include  "GDriveGroupsSync.h"
class GDriveEntriesSync:public GDriveGroupsSync
{
public:
    GDriveEntriesSync(Database* db1, Database* db2, bool syncGroups=true);
    /**
     * @brief syncDatabases - syncs items between two provided databases
     * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
     * @param db2 - second source database.
     */
    void syncDatabases();
    static bool compareByUuid(Entry* entry1,Entry* entry2);
    static bool compareByCreationTime(Entry* entry1,Entry* entry2);
private:
    void addMissingEntries(QList<Entry *> missingEntries);
    void updateEntryData(Entry* entry, Entry* new_data);
    void updateEntryGroup(Entry* entry, Entry* new_data);
    void removeEntry(Entry* entry);
    bool isItemMoved(Entry* entry);
    bool syncGroups;
    QMap<Uuid,Entry*> entries1;
    QMap<Uuid,Entry*> entries2;

};

#endif // GDRIVEENTRIESSYNC_H
