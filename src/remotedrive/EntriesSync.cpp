#include "EntriesSync.h"
#include <QtCore/QDebug>
#include "DatabaseSyncFactory.h"

EntriesSync::EntriesSync(Database *db1, Database *db2,
                                     bool syncGroups)
  : DatabaseSync<Entry>(db1, db2), syncGroups(syncGroups) {}

QSharedPointer<SyncObject>EntriesSync::syncDatabases() {
  // sync groups also if it specified by constructor
  if (syncGroups) {
    qDebug() << "Perform whole sync: groups & entries";
    QSharedPointer<DatabaseSyncBase> groupSync =
      DatabaseSyncFactory::createDatabaseSync(
        DatabaseSyncFactory::SyncId::GROUP,
        db1,
        db2);
    syncObject = QSharedPointer<SyncObject>(new SyncObject());
    groupSync->setSyncObject(syncObject);
    groupSync->syncDatabases();
  }
  return DatabaseSync<Entry>::syncDatabases();
}

EntriesSync::~EntriesSync() {}

QString EntriesSync::getType() {
  return ENTRY_TYPE;
}

bool EntriesSync::processEntry(Database *db, Entry *entry)  {
  // put to ignore unused parameter message since its overriden function
  (void)entry;
  (void)db;
  bool result = true;
  return result;
}

void EntriesSync::setParentGroup(Entry *entry, Group *group) {
  // move entry to recycle bin will perform some extra actions by creating
  // recycle bin if needed
  Database *db = group->database();
  db->metadata()->recycleBin() && group->uuid() ==
  db->metadata()->recycleBin()->uuid() ? db->recycleEntry(entry) :  entry->
  setGroup(group);
}

const Group * EntriesSync::getParentGroup(Entry *entry) {
  return entry->group();
}

const QString EntriesSync::getEntryName(Entry *entry) {
  return entry->attributes()->value("Title");
}

QMap<Uuid, Entry *>EntriesSync::getEntriesMap(Database *db) {
  return db->rootGroup()->entriesMapRecursive();
}

/**
 * @brief EntriesSync::removeEntry - moves entry to Recycle Bin
 * @param db - database which contains entry to remove
 * @param entry  - entry to remove
 */
void EntriesSync::removeEntry(Entry *entry) {
  qDebug() << "Recycle entry " + entry->uuid().toBase64();
  db1->recycleEntry(entry);
  getSyncObject()->increase(SEntry(), SRemoved(), SLocal());
}


ObjectType EntriesSync::getObjectType() {
    return SEntry();
}
