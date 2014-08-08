#include "GDriveGroupsSync.h"


GDriveGroupsSync::GDriveGroupsSync(Database *db1, Database *db2)
  : GDriveDatabaseSync(db1, db2) {}

GDriveGroupsSync::~GDriveGroupsSync() {}

bool GDriveGroupsSync::compareByCreationTime(Group *entry1, Group *entry2) {
  return entry1->timeInfo().creationTime() < entry2->timeInfo().creationTime();
}

void GDriveGroupsSync::updateEntryData(Group *entry, Group *new_data) {
  qDebug() << "Updating the group";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  entry->copyDataFrom(new_data);
  entry->setTimeInfo(new_data->timeInfo());
  GDriveDatabaseSync::resultLocalOlderGroups++;
}

void GDriveGroupsSync::updateEntryGroup(Group *entry, Group *new_data) {
  qDebug() << "Updating parent group";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  Q_ASSERT(new_data->parentGroup());
  Group *targetGroup = entries1[new_data->parentGroup()->uuid()];
  Q_ASSERT(targetGroup);
  entry->setParent(targetGroup);
  entry->setTimeInfo(new_data->timeInfo());
  GDriveDatabaseSync::resultLocalOlderGroups++;
}

void GDriveGroupsSync::removeEntry(Group *entry) {
  Group *toRecycle = entries1[entry->uuid()];

  Q_ASSERT(toRecycle);
  db1->recycleGroup(toRecycle);
  GDriveDatabaseSync::resultLocalRemovedGroups++;
}

void GDriveGroupsSync::addMissingEntries(QList<Group *>missingEntries) {
  // we can not check if item is really a new item because there is no way to
  // check locally whether it was removed w/o recycle bin
  if (!db2->metadata()->recycleBinEnabled()) {
    // TODO show all missingEntries dialog to the user with ability to choose
    // which items should be really removed
    // Strongly recommend to the user to enable recycle bin to prevent manual
    // sync
    return;
  }

  // sort by creation time.Parent group always created before child.Sort
  // guaranties parents will be created before childs
  qSort(missingEntries.begin(),
        missingEntries.end(), GDriveGroupsSync::compareByCreationTime);

  qDebug() << "Add missing entries";
  Q_FOREACH(Group * entry, missingEntries) {
    Group *parentGr = entry->parentGroup();

    // get missing entry group in the source database
    Group *group = entries1[entry->parentGroup()->uuid()];

    Q_ASSERT(group);

    // creating new entry
    Group *newEntry = new Group();
    newEntry->copyDataFrom(entry);
    newEntry->setParent(group);
    newEntry->setTimeInfo(entry->timeInfo());
    qDebug() << "Add missing group " + newEntry->name() + " to group::" +
    group->getGroupName();
    GDriveDatabaseSync::resultLocalMissingGroups++;
  }
}

void GDriveGroupsSync::syncDatabases() {
  // make sure root groups are the same.It has to be if we are syncing the same
  // database
  Q_ASSERT(db1->rootGroup()->uuid() == db2->rootGroup()->uuid());
  entries1 = db1->rootGroup()->groupsMapRecursive(true);
  entries2 = db2->rootGroup()->groupsMapRecursive(true);
  QList<Group *> missingEntries;
  Q_FOREACH(Group * cloudEntry, entries2) {
    // avoid to sync root groups. They always supposed to be the same
    if (cloudEntry == db2->rootGroup()) continue;

    // avoid to sync recycle bin since it's not a normal group and created
    // automatically when entries deleted
    if (cloudEntry == db2->metadata()->recycleBin()) continue;

    // remote group also exists in local database
    if (entries1.contains(cloudEntry->uuid())) {
      Group *localEntry = entries1[cloudEntry->uuid()];

      // local entry last update time is older than remote entry
      if (localEntry->timeInfo().lastModificationTime() <
          cloudEntry->timeInfo().lastModificationTime()) {
        qDebug() << QString("Source Db has older group::");
        updateEntryData(localEntry, cloudEntry);
      }

      // we need to just count changes to decide further whether whole remote
      // database should be updated
      else {
        GDriveDatabaseSync::resultRemoteOlderGroups++;
      }

      // group location was changed - to another group or recycle bin. It
      // changes independently from lastModificationTime
      // updating only in a case if remote group has newer location change
      // timestamp
      if (localEntry->timeInfo().locationChanged() <
          cloudEntry->timeInfo().locationChanged()) {
        bool isRemoved =
          Tools::hasChild(db2->metadata()->recycleBin(), cloudEntry);

        // entry was moved to normal group
        if (!isRemoved) {
          updateEntryGroup(localEntry, cloudEntry);
        }

        // entry was moved to recycle bin which is also group
        else {
          removeEntry(localEntry);
        }
      }

      // we need to just count changes to decide further whether whole remote
      // database should be updated
      else {
        bool isRemoved =
          Tools::hasChild(db1->metadata()->recycleBin(), localEntry);

        // entry was moved to normal group
        if (!isRemoved) {
          GDriveDatabaseSync::resultRemoteOlderGroups++;
        }

        // entry was moved to recycle bin which is also group
        else {
          GDriveDatabaseSync::resultRemoteRemovedGroups++;
        }
      }
    }

    // group exists only in remote database
    else {
      missingEntries.append(cloudEntry);
    }
  }

  if (missingEntries.length() > 0) {
    addMissingEntries(missingEntries);
  }

  // count whether any unique groups exist in local database
  // to further decide whether we need to update remote db
  Q_FOREACH(Group * localEntry, entries1) {
    // avoid to sync root groups. They always supposed to be the same
    if (localEntry == db1->rootGroup()) continue;

    // avoid to sync recycle bin since it's not a normal group and created
    // automatically when entries deleted
    if (localEntry == db1->metadata()->recycleBin()) continue;

    if (entries2.contains(localEntry->uuid())) {
      GDriveDatabaseSync::resultRemoteMissingGroups++;
    }
  }
}
