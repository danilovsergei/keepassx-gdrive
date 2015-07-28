#include "GroupsSync.h"

GroupsSync::GroupsSync(Database *db1, Database *db2) :
  DatabaseSync<Group>(db1, db2)
{
}

GroupsSync::~GroupsSync()
{
}

bool GroupsSync::processEntry(Database *db, Group *entry)
{
  bool result = true;

  // avoid to sync root groups. They always supposed to be the same
  if (entry == db->rootGroup()) result = false;
  return result;
}

void GroupsSync::removeEntry(Group *entry)
{
  Group *toRecycle = entries1[entry->uuid()];

  Q_ASSERT(toRecycle);
  db1->recycleGroup(toRecycle);
  getSyncObject()->increase(SGroup(), SRemoved(), SLocal());
}

void GroupsSync::setParentGroup(Group *entry, Group *group)
{
  // move entry to recycle bin will perform some extra actions by creating
  // recycle bin if needed
  Database *db = group->database();

  db->metadata()->recycleBin() && group->uuid()
  == db->metadata()->recycleBin()->uuid() ? db->recycleGroup(entry) : entry->
  setParent(group);
}

const Group *GroupsSync::getParentGroup(Group *entry)
{
  return entry->parentGroup();
}

const QString GroupsSync::getEntryName(Group *entry)
{
  return entry->name();
}

QMap<Uuid, Group *> GroupsSync::getEntriesMap(Database *db)
{
  QMap<Uuid, Group *> entries = db->rootGroup()->groupsMapRecursive(true);

  // add recycle bin group since it's also has to be synced if was created at
  // one of the sources and currently absent in another source
  // TODO find out if still need it after adding separate metadata sync
  if (db->metadata()->recycleBin())
    entries.insert(db->metadata()->recycleBin()->uuid(), db->metadata()->recycleBin());
  return entries;
}

QString GroupsSync::getType()
{
  return ENTRY_TYPE;
}

ObjectType GroupsSync::getObjectType()
{
  return SGroup();
}

void GroupsSync::updateEntryData(Group *entry, Group *newData)
{
  qDebug() << "Updating the " + getType() + " data";
  Q_ASSERT(entry);
  Q_ASSERT(newData);
  entry->copyDataFrom(newData);
  entry->setTimeInfo(newData->timeInfo());
  getSyncObject()->increase(getObjectType(), SOlder(), SLocal());
}
