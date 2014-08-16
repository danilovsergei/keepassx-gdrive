#include "GDriveGroupsSync.h"



GDriveGroupsSync::GDriveGroupsSync(Database *db1, Database *db2)
  : GDriveDatabaseSync(db1, db2) {}

GDriveGroupsSync::~GDriveGroupsSync() {}

bool GDriveGroupsSync::compareByCreationTime(Group *entry1, Group *entry2) {
  return entry1->timeInfo().creationTime() < entry2->timeInfo().creationTime();
}



void GDriveGroupsSync::removeEntry(Group *entry) {
  Group *toRecycle = entries1[entry->uuid()];
  Q_ASSERT(toRecycle);
  db1->recycleGroup(toRecycle);
  GDriveDatabaseSync::syncObject->resultLocalRemovedGroups++;
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
    Group* newEntry = new Group();
    newEntry->copyDataFrom(entry);
    newEntry->setParent(group);
    newEntry->setTimeInfo(entry->timeInfo());
    qDebug() << "Add missing group " + newEntry->name() + " to group::" +
    group->getGroupName();
    GDriveDatabaseSync::syncObject->resultLocalMissingGroups++;
  }
}

void GDriveGroupsSync::increaseRemoteOlderEntries() {
 GDriveDatabaseSync::syncObject->resultRemoteOlderGroups++;
}

void GDriveGroupsSync::increaseRemoteMissingEntries() {
 GDriveDatabaseSync::syncObject->resultRemoteMissingGroups++;
}

void GDriveGroupsSync::increaseRemoteRemovedEntries() {
 GDriveDatabaseSync::syncObject->resultRemoteRemovedGroups++;
}

void GDriveGroupsSync::increaseLocalOlderEntries() {
 GDriveDatabaseSync::syncObject->resultLocalOlderGroups++;
}

void GDriveGroupsSync::increaseLocalMissingEntries() {
 GDriveDatabaseSync::syncObject->resultLocalMissingGroups++;
}

void GDriveGroupsSync::increaseLocalRemovedEntries() {
 GDriveDatabaseSync::syncObject->resultLocalRemovedGroups++;
}



