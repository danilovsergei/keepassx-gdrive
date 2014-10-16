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
  return result;
}

void GDriveGroupsSync::removeEntry(Group *entry) {
  Group *toRecycle = entries1[entry->uuid()];

  Q_ASSERT(toRecycle);
  db1->recycleGroup(toRecycle);
  getSyncObject()->increase(SEntry(), SRemoved(), SLocal());
}

void GDriveGroupsSync::setParentGroup(Group *entry, Group *group) {
  // move entry to recycle bin will perform some extra actions by creating
  // recycle bin if needed
  Database *db = group->database();

  db->metadata()->recycleBin() && group->uuid() ==
  db->metadata()->recycleBin()->uuid() ? db->recycleGroup(entry) :  entry->
  setParent(group);
}

const Group * GDriveGroupsSync::getParentGroup(Group *entry) {
  return entry->parentGroup();
}

const QString GDriveGroupsSync::getEntryName(Group *entry) {
  return entry->name();
}

QMap<Uuid, Group *>GDriveGroupsSync::getEntriesMap(Database *db) {
  QMap<Uuid, Group *> entries = db->rootGroup()->groupsMapRecursive(true);

  // add recycle bin group since it's also has to be synced if was created at
  // one of the sources
  if (db->metadata()->recycleBin()) entries.insert(
      db->metadata()->recycleBin()->uuid(), db->metadata()->recycleBin());
  return entries;
}

QString GDriveGroupsSync::getType() {
  return ENTRY_TYPE;
}

ObjectType GDriveGroupsSync::getObjectType() {
    return SGroup();
}

