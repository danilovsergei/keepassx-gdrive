#ifndef GDRIVEGROUPSSYNC_H
#define GDRIVEGROUPSSYNC_H

#include "../core/Database.h"
#include "../core/Group.h"
#include "../core/Entry.h"
#include "../core/Tools.h"
#include "../core/Metadata.h"
#include <QtCore/QDebug>

#include <QtCore/QObject>

class GDriveGroupsSync : public QObject
{
    Q_OBJECT
public:
    GDriveGroupsSync(Database* db1, Database* db2);
    ~GDriveGroupsSync();

    /**
     * @brief syncDatabases - syncs groups between two provided databases
     * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
     * @param db2 - second source database.
     */
    void syncDatabases();
    static bool compareByUuid(Group* entry1,Group* entry2);
    static bool compareByCreationTime(Group* entry1,Group* entry2);
private:
    void addMissingEntries(QList<Group*> missingEntries);
    void updateEntryData(Group* entry, Group* new_data);
    void updateEntryGroup(Group *entry, Group* new_data);
    void removeEntry(Group* entry);
    bool isItemMoved(Group* entry);

    Database* db1;
    Database* db2;
    QMap<Uuid,Group*> entries1;
    QMap<Uuid,Group*> entries2;


};

#endif // GDRIVEGROUPSSYNC_H
