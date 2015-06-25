#include "DatabaseSyncBase.h"

DatabaseSyncBase::DatabaseSyncBase(Database* db1, Database* db2)
    : db1(db1), db2(db2)
{}

/**
 * @brief DatabaseSync::syncMetadata Check if db metadata was changed and sync it
 * Do it while syncing groups because they always synced before entries
 */

// TODO move metadata sync prior entries and groups and do it once

void DatabaseSyncBase::syncMetadata() {
    // sync recycle bin
    // recycle bin absent in local database and exists in remote database
    // recycle bin changes in remote database are newer that local database changes
    // Note recycleBin() means emty recycle, while recycleBinEnabled means disabled recycle bin

    if (!db1->metadata()->recycleBin() && db2->metadata()->recycleBin() &&
            db1->metadata()->recycleBinChanged() <  db2->metadata()->recycleBinChanged()) {
        // perform full copy of the recycle bin metadata including uuid and other data
        Group* remoteRecycleBin = db2->metadata()->recycleBin();
        Group* recycleBin = new Group();
        recycleBin->setUuid(remoteRecycleBin->uuid());
        recycleBin->setParent(db1->rootGroup());
        recycleBin->copyDataFrom(remoteRecycleBin);
        db1->metadata()->setRecycleBin(recycleBin);
        db1->metadata()->setRecycleBinChanged(db2->metadata()->recycleBinChanged());
        // Note that recycle bin entries will be synced separately
        // as a part of normal sync process
    }

    // user cancelled recycle bin in remote database
    // need to cancel it for local db as well
    // TODO implement this case
}
