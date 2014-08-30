#include "GDriveGroupsSync.h"


GDriveGroupsSync::GDriveGroupsSync(Database *db1, Database *db2)
  : GDriveDatabaseSync<Group>(db1, db2) {}

QSharedPointer<GDriveSyncObject>GDriveGroupsSync::syncDatabases() {
  return GDriveDatabaseSync<Group>::syncDatabases();
}

GDriveGroupsSync::~GDriveGroupsSync() {}

bool GDriveGroupsSync::processEntry(Database *db, Group *entry)  {
  bool result = true;

  // avoid to sync root groups. They always supposed to be the same
  if (entry == db->rootGroup()) result = false;

  // avoid to sync recycle bin since it's not a normal group and created
  // automatically when entries deleted
  if (entry == db->metadata()->recycleBin()) result = false;
  return result;
}

void GDriveGroupsSync::removeEntry(Group *entry) {
  Group *toRecycle = entries1[entry->uuid()];

  Q_ASSERT(toRecycle);
  db1->recycleGroup(toRecycle);
  getResultStat()->increaseLocalRemovedEntries();
}

void GDriveGroupsSync::setParentGroup(Group *entry, Group *group) {
  entry->setParent(group);
}

const Group * GDriveGroupsSync::getParentGroup(Group *entry) {
  return entry->parentGroup();
}

const QString GDriveGroupsSync::getEntryName(Group *entry) {
  return entry->name();
}

QMap<Uuid, Group *>GDriveGroupsSync::getEntriesMap(Database *db) {
  return db->rootGroup()->groupsMapRecursive(true);
}

QString GDriveGroupsSync::getType() {
  return ENTRY_TYPE;
}

QSharedPointer<SyncEntry> GDriveGroupsSync::getResultStat() {
  Q_ASSERT(!syncObject.isNull());
  return syncObject->getResultStat(SyncEntry::ObjectType::Group);
}
