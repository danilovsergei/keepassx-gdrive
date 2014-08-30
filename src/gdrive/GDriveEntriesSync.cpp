#include "GDriveEntriesSync.h"
#include <QtCore/QDebug>
#include "GDriveDatabaseSyncFactory.h"
GDriveEntriesSync::GDriveEntriesSync(Database *db1, Database *db2,
                                     bool syncGroups)
  : GDriveDatabaseSync<Entry>(db1, db2), syncGroups(syncGroups) {}

QSharedPointer<GDriveSyncObject>GDriveEntriesSync::syncDatabases() {
  // sync groups also if it specified by constructor
  if (syncGroups) {
    qDebug() << "Perform whole sync: groups & entries";
    QSharedPointer<GDriveDatabaseSyncBase> groupSync =
      GDriveDatabaseSyncFactory::createDatabaseSync(
        GDriveDatabaseSyncFactory::SyncId::GROUP,
        db1,
        db2);
    syncObject = QSharedPointer<GDriveSyncObject>(new GDriveSyncObject());
    groupSync->setSyncObject(syncObject);
    groupSync->syncDatabases();
  }
  return GDriveDatabaseSync<Entry>::syncDatabases();
}

GDriveEntriesSync::~GDriveEntriesSync() {}

QString GDriveEntriesSync::getType() {
  return ENTRY_TYPE;
}

bool GDriveEntriesSync::processEntry(Database *db, Entry *entry)  {
  // put to ignore unused parameter message since its overriden function
  (void)entry;
  (void)db;
  bool result = true;
  return result;
}

void GDriveEntriesSync::setParentGroup(Entry *entry, Group *group) {
  entry->setGroup(group);
}

const Group * GDriveEntriesSync::getParentGroup(Entry *entry) {
  return entry->group();
}

const QString GDriveEntriesSync::getEntryName(Entry *entry) {
  return entry->attributes()->value("Title");
}

QMap<Uuid, Entry *>GDriveEntriesSync::getEntriesMap(Database *db) {
  return db->rootGroup()->entriesMapRecursive();
}

/**
 * @brief GDriveEntriesSync::removeEntry - moves entry to Recycle Bin
 * @param db - database which contains entry to remove
 * @param entry  - entry to remove
 */
void GDriveEntriesSync::removeEntry(Entry *entry) {
    qDebug() << "Recycle entry " + entry->uuid().toBase64();
  db1->recycleEntry(entry);
  getResultStat()->increaseLocalRemovedEntries();
}

QSharedPointer<SyncEntry> GDriveEntriesSync::getResultStat() {
  Q_ASSERT(!syncObject.isNull());
  return syncObject->getResultStat(SyncEntry::ObjectType::Entry);
}
