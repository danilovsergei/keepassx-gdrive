#include "DatabaseSyncBase.h"

DatabaseSyncBase::DatabaseSyncBase(Database* db1, Database* db2)
    : db1(db1), db2(db2)
{}

/**
 * @brief DatabaseSync::syncMetadata Check if db metadata was changed and sync it
 * Do it while syncing groups because they always synced before entries
 */
void DatabaseSyncBase::syncMetadata() {
    // sync last modification date
    if (db1->metadata()->lastModifiedDate() < db2->metadata()->lastModifiedDate()) {
        db1->metadata()->setLastModifiedDate(db2->metadata()->lastModifiedDate());
    } else {
        // remote database sync will be performed in the DatabaseWidget in sync callback
    }

    // sync recycle bin. There are some cases when it's needed:
    //   recycle bin absent in local database and exists in remote database and
    //   recycle bin changes in remote database are newer that local database changes
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
        // treat recycle bin as regular group from the point of sync
        syncObject->increase(SGroup(), SMissing(), SLocal());
        // do nothing for SRemoteType because it will be counted during groups sync as missing group

        // Note that recycle bin entries will be synced separately
        // as a part of normal sync process
    }

    // user cancelled recycle bin in remote database
    // need to cancel it for local db as well
    // TODO implement this case
}
