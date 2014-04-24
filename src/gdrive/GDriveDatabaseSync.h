#ifndef GDRIVEDATABASESYNC_H
#define GDRIVEDATABASESYNC_H
#include "../core/Database.h"
#include "../core/Group.h"
#include "../core/Entry.h"

class GDriveDatabaseSync:public QObject
{
public:
    GDriveDatabaseSync();
    /**
     * @brief syncDatabases - syncs items between two provided databases
     * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
     * @param db2 - second source database.
     */
    void syncDatabases(Database* db1,Database* db2);
    static bool compareByUuid(Entry* entry1,Entry* entry2);
    static bool compareByCreationTime(Entry* entry1,Entry* entry2);
private:
    void addMissingEntries(Database* db, const QList<Entry*>& entries);
    void updateEntry(Entry* entry, Entry* new_data);
    bool isItemNew(Database* db,Entry* entry);

};

#endif // GDRIVEDATABASESYNC_H
