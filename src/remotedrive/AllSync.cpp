#include "AllSync.h"

AllSync::AllSync(Database *db1, Database *db2) :
  GroupsSync(db1, db2)
{
}

QSharedPointer<SyncObject> AllSync::syncDatabases()
{
  syncObject = QSharedPointer<SyncObject>(new SyncObject());
  syncObject->setLocalModificationDate(db1->metadata()->lastModifiedDate());
  syncObject->setRemoteModificationDate(db2->metadata()->lastModifiedDate());
  syncMetadata();
  // avoid updating lastModified date after synced entries creation
  // which will lead to lastModified set to now
  // instead correct lastModified will be set by syncMetadata from remote database if applicable
  db1->metadata()->setUpdateLastModifiedDate(false);

  qDebug() << "Perform whole sync: groups & entries";
  QSharedPointer<DatabaseSyncBase> groupSync = DatabaseSyncFactory::createDatabaseSync(
    DatabaseSyncFactory::SyncId::GROUP, db1, db2);
  QSharedPointer<DatabaseSyncBase> entriesSync = DatabaseSyncFactory::createDatabaseSync(
    DatabaseSyncFactory::SyncId::ENTRY, db1, db2);
  groupSync->setSyncObject(syncObject);
  groupSync->syncEntries();
  entriesSync->setSyncObject(syncObject);
  entriesSync->syncEntries();
  db1->metadata()->setUpdateLastModifiedDate(true);
  return syncObject;
}
