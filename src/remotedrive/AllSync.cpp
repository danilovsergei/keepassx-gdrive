#include "AllSync.h"

AllSync::AllSync(Database *db1, Database *db2) :
  GroupsSync(db1, db2)
{
}

QSharedPointer<SyncObject> AllSync::syncDatabases()
{
  syncObject = QSharedPointer<SyncObject>(new SyncObject());
  syncMetadata();
  qDebug() << "Perform whole sync: groups & entries";
  QSharedPointer<DatabaseSyncBase> groupSync = DatabaseSyncFactory::createDatabaseSync(
    DatabaseSyncFactory::SyncId::GROUP, db1, db2);
  QSharedPointer<DatabaseSyncBase> entriesSync = DatabaseSyncFactory::createDatabaseSync(
    DatabaseSyncFactory::SyncId::ENTRY, db1, db2);
  groupSync->setSyncObject(syncObject);
  groupSync->syncEntries();
  entriesSync->setSyncObject(syncObject);
  entriesSync->syncEntries();
  return syncObject;
}
