#include "GDriveDatabaseSync.h"
#include "../core/Group.h"
#include "../core/Entry.h"


template<typename SO>
GDriveDatabaseSync<SO>::GDriveDatabaseSync(Database *db1, Database *db2)
  : db1(db1), db2(db2) {}

template<typename SO>
QSharedPointer<GDriveSyncObject>GDriveDatabaseSync<SO>::getSyncObject() {
  return syncObject;
}

template<typename SO>
void GDriveDatabaseSync<SO>::setSyncObject(
  QSharedPointer<GDriveSyncObject>syncObject) {
  Q_ASSERT(!syncObject.isNull());
  GDriveDatabaseSync<SO>::syncObject = syncObject;
}

template<typename SO>
void GDriveDatabaseSync<SO>::syncEntry(SO *localEntry, SO *cloudEntry) {
  // local entry last update time is older than remote entry.Updating local
  // database
  if (localEntry->timeInfo().lastModificationTime().toTime_t() <
      cloudEntry->timeInfo().lastModificationTime().toTime_t()) {
    qDebug() << QString(
      "Local Db has older " + getType() + " data.UUID=" +
      localEntry->uuid().toHex());
    updateEntryData(localEntry, cloudEntry);
  }

  // we need to just count changes to decide further whether whole remote
  // database should be updated
  else if (localEntry->timeInfo().lastModificationTime().toTime_t() >
           cloudEntry->timeInfo().lastModificationTime().toTime_t()) {
    getResultStat()->increaseRemoteOlderEntries();
  }
}

template<typename SO>
void GDriveDatabaseSync<SO>::updateEntryGroup(SO *entry, SO *new_data) {
  qDebug() << "Updating parent group";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  Q_ASSERT(getParentGroup(new_data));
  Group *targetGroup = db1->resolveGroup(getParentGroup(new_data)->uuid());
  Q_ASSERT(targetGroup);
  setParentGroup(entry, targetGroup);
  entry->setTimeInfo(new_data->timeInfo());
  getResultStat()->increaseLocalOlderEntries();
}

template<typename SO>
void GDriveDatabaseSync<SO>::syncLocation(SO *localEntry, SO *cloudEntry) {
  // entry location was changed - to another group or recycle bin. It
  // changes independently from lastModificationTime
  // updating only in a case
  // if remote entry has newer location change timestamp
  if (localEntry->timeInfo().locationChanged().toTime_t() <
      cloudEntry->timeInfo().locationChanged().toTime_t()) {
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

  // Case when local entry location is newer than remote entry location
  // we need to just count such changes to decide further whether whole remote
  // database should be updated
  else if (localEntry->timeInfo().locationChanged().toTime_t() >
           cloudEntry->timeInfo().locationChanged().toTime_t()) {
    bool isRemoved =
      Tools::hasChild(db1->metadata()->recycleBin(), localEntry);

    // entry was moved to normal group
    if (!isRemoved) {
      getResultStat()->increaseRemoteOlderEntries();
    }

    // entry was moved to recycle bin which is also group
    else {
      getResultStat()->increaseRemoteRemovedEntries();
    }
  }
}

template<typename SO>
void GDriveDatabaseSync<SO>::updateEntryData(SO *entry, SO *new_data) {
  qDebug() << "Updating the " + getType() + " data";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  entry->copyDataFrom(new_data);
  entry->setTimeInfo(new_data->timeInfo());
  getResultStat()->increaseLocalOlderEntries();
}

template<class SO>
bool GDriveDatabaseSync<SO>::compareByCreationTime(SO *entry1, SO *entry2) {
  return entry1->timeInfo().creationTime() < entry2->timeInfo().creationTime();
}

template<class SO>
void GDriveDatabaseSync<SO>::addMissingEntries(QList<SO *>missingEntries) {
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
        missingEntries.end(), compareByCreationTime);

  qDebug() << "Add missing entries of type " + getType();
  Q_FOREACH(SO * entry, missingEntries) {
    const Group *parentGr = getParentGroup(entry);

    // get missing entry group in the source database
    Group *group = db1->rootGroup()->groupsMapRecursive(true)[parentGr->uuid()];

    Q_ASSERT(group);

    // creating new entry
    SO *newEntry = new SO();
    newEntry->copyDataFrom(entry);
    setParentGroup(newEntry, group);
    newEntry->setTimeInfo(entry->timeInfo());
    qDebug() << "Add missing " + getType() + ": " + getEntryName(newEntry) +
    " to group::" +
      group->getGroupName();
    getResultStat()->increaseLocalMissingEntries();
  }
}

template<typename SO>
QSharedPointer<GDriveSyncObject>GDriveDatabaseSync<SO>::syncDatabases() {
  qDebug() << "Running sync for type: " + getType();

  // make sure root groups are the same.It has to be if we are syncing the same
  // database
  Q_ASSERT(db1->rootGroup()->uuid() == db2->rootGroup()->uuid());
  entries1 = getEntriesMap(db1);
  entries2 = getEntriesMap(db2);

  QList<SO *> missingEntries;
  Q_FOREACH(SO * cloudEntry, entries2) {
    if (!processEntry(db2, cloudEntry)) continue;

    // remote entry also exists in local database
    if (entries1.contains(cloudEntry->uuid())) {
      SO *localEntry = entries1[cloudEntry->uuid()];

      syncEntry(localEntry, cloudEntry);
      syncLocation(localEntry, cloudEntry);
    }

    // entry exists only in remote database
    else {
      missingEntries.append(cloudEntry);
    }
  }

  if (missingEntries.length() > 0) {
    addMissingEntries(missingEntries);
  }

  // count whether any unique entry exist in local database
  // to further decide whether we need to update remote db
  Q_FOREACH(SO * localEntry, entries1) {
    if (!processEntry(db1, localEntry)) continue;

    if (!entries2.contains(localEntry->uuid())) {
      getResultStat()->increaseRemoteMissingEntries();
    }
  }
  return GDriveDatabaseSync::syncObject;
}

template class GDriveDatabaseSync<Group>;
template class GDriveDatabaseSync<Entry>;
