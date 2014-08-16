#include "GDriveDatabaseSync.h"
#include "../core/Group.h"
namespace DatabaseSync {
GDriveDatabaseSync::GDriveDatabaseSync(Database *db1, Database *db2)
  : db1(db1), db2(db2),syncObject(QSharedPointer<GDriveSyncObject>(new GDriveSyncObject())) {
}

QSharedPointer<GDriveSyncObject>GDriveDatabaseSync::getSyncObject() {
  return syncObject;
}


template <typename T>
void GDriveDatabaseSync::syncEntry(T* localEntry,T* cloudEntry) {
    // local entry last update time is older than remote entry.Updating local
    // database
    if (localEntry->timeInfo().lastModificationTime().toTime_t() <
        cloudEntry->timeInfo().lastModificationTime().toTime_t()) {
      qDebug() << QString(
        "Source Db has older entry.UUID=" + localEntry->uuid().toHex()) ;
      updateEntryData(localEntry, cloudEntry);
    }
    // we need to just count changes to decide further whether whole remote
    // database should be updated
    else if (localEntry->timeInfo().lastModificationTime().toTime_t() >
             cloudEntry->timeInfo().lastModificationTime().toTime_t()) {
      increaseRemoteOlderEntries();
    }
}

template <typename T>
void GDriveDatabaseSync::updateEntryGroup(T *entry, T *new_data) {
  qDebug() << "Updating parent group";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  Q_ASSERT(new_data->parentGroup());
  Group *targetGroup = db1->resolveGroup(new_data->parentGroup()->uuid());
  Q_ASSERT(targetGroup);
  entry->setParent(targetGroup);
  entry->setTimeInfo(new_data->timeInfo());
  increaseLocalOlderEntries();
}





template <typename T>
void GDriveDatabaseSync::syncLocation(T* localEntry,T* cloudEntry) {
    // entry location was changed - to another group or recycle bin. It
    // changes independently from lastModificationTime
    // updating only in a case if remote entry has newer location change
    // timestamp
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

    // we need to just count changes to decide further whether whole remote
    // database should be updated
    else if (localEntry->timeInfo().locationChanged().toTime_t() >
             cloudEntry->timeInfo().locationChanged().toTime_t()) {
      bool isRemoved =
        Tools::hasChild(db1->metadata()->recycleBin(), localEntry);

      // entry was moved to normal group
      if (!isRemoved) {
        GDriveDatabaseSync::syncObject->resultRemoteOlderEntries++;
      }

      // entry was moved to recycle bin which is also group
      else {
       GDriveDatabaseSync::syncObject->resultRemoteRemovedEntries++;
      }
    }
}


template <class T> void GDriveDatabaseSync::removeEntry(T* entry) {
    //each derived class must implement its own remove entry
    Q_ASSERT(false);
}



template <class T>
void GDriveDatabaseSync::updateEntryData(T *entry, T *new_data) {
  qDebug() << "Updating the group";
  Q_ASSERT(entry);
  Q_ASSERT(new_data);
  entry->copyDataFrom(new_data);
  entry->setTimeInfo(new_data->timeInfo());
  increaseLocalOlderEntries();

}




QSharedPointer<GDriveSyncObject> GDriveDatabaseSync::syncDatabases() {
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

      syncEntry(localEntry,cloudEntry);
      syncLocation(localEntry,cloudEntry);

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
      increaseRemoteMissingEntries();
    }
  }
return GDriveDatabaseSync::syncObject;
}



template void GDriveDatabaseSync::syncEntry<Group>(Group*,Group*);
template void GDriveDatabaseSync::syncLocation<Group>(Group*,Group*);
}
