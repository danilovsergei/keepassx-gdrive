#include "GDriveEntriesSync.h"
#include <QtCore/QDebug>

GDriveEntriesSync::GDriveEntriesSync(Database *db1, Database *db2)
{
    this->db1=db1;
    this->db2=db2;
}

bool GDriveEntriesSync::compareByUuid(Entry* entry1,Entry* entry2) {
return entry1->uuid()<entry2->uuid();
}

bool GDriveEntriesSync::compareByCreationTime(Entry* entry1,Entry* entry2) {
return entry1->timeInfo().creationTime()<entry2->timeInfo().creationTime();
}

/**
 * @brief GDriveEntriesSync::addMissingEntries - adds missing entries to local db
 * @param db - db to add entries
 * @param sourceEntries -map of local entries
 * @param missingEntries - map of missing entries
 */
void GDriveEntriesSync::addMissingEntries(QList<Entry *> missingEntries) {
  //we can not check if item is really a new item because there is no way to check locally whether it was removed w/o recycle bin
  if (!db2->metadata()->recycleBinEnabled()) {
    //TODO show all missingEntries dialog to the user with ability to choose which items should be really removed
    //Strongly recommend to the user to enable recycle bin to prevent manual sync
    return;
  }

//sort by creation time.Parent group always created before child.Sort guaranties parents will be created before childs
qSort(missingEntries.begin(),missingEntries.end(),GDriveEntriesSync::compareByCreationTime);

qDebug() << "Add missing entries";
Q_FOREACH(Entry* entry,missingEntries) {

//get missing entry group in the source database
Group* group=db1->rootGroup()->groupsMapRecursive(true)[entry->group()->uuid()];
Q_ASSERT(group);

//creating new entry
Entry* newEntry=new Entry();
newEntry->copyDataFrom(entry);
newEntry->setGroup(group);
newEntry->setTimeInfo(entry->timeInfo());
qDebug() << "Add missing entry "+newEntry->attributes()->value("Title")+" to group::"+group->getGroupName();
}
}

/**
 * @brief GDriveEntriesSync::removeEntry - moves entry to Recycle Bin
 * @param db - database which contains entry to remove
 * @param entry  - entry to remove
 */
void GDriveEntriesSync::removeEntry(Entry* entry) {
    qDebug() << "Recycle entry "+entry->uuid().toBase64();
    db1->recycleEntry(entry);
}

/**
 * @brief GDriveEntriesSync::updateEntryData - updates item in local database by copying all the data including data, attachments, attributes from
 * remote item
 * @param entry - local entry to be updated
 * @param new_data - remote entry to copy data from
 */
void GDriveEntriesSync::updateEntryData(Entry* entry,Entry* new_data) {
qDebug() << "Updating the entry";
entry->beginUpdate();
entry->copyDataFrom(new_data);
entry->endUpdate();
entry->setTimeInfo(new_data->timeInfo());
}

/**
 * @brief GDriveEntriesSync::updateEntryGroup - Moves entry to the new group.Called when locationChanged of the new group is higher
 * @param db - Target database entry will be modified
 * @param entry - target entry which will be modified
 * @param new_data - entry with latest data and latest group
 */
void GDriveEntriesSync::updateEntryGroup(Entry* entry, Entry* new_data) {
    qDebug() << "Updating entry group";
    Group* targetGroup=db1->resolveGroup(new_data->group()->uuid());
    Q_ASSERT(targetGroup);
    entry->setGroup(targetGroup);
    entry->setTimeInfo(new_data->timeInfo());
}

void GDriveEntriesSync::syncDatabases() {
entries1=db1->rootGroup()->entriesMapRecursive();
entries2=db2->rootGroup()->entriesMapRecursive();

QList<Entry*> missingEntries;
Q_FOREACH(Entry* cloudEntry, entries2) {
    //entry exists in both databases
    if (entries1.contains(cloudEntry->uuid())) {
    Entry* localEntry= entries1[cloudEntry->uuid()];
    //local entry last update time is older than remote entry
    if (localEntry->timeInfo().lastModificationTime()<cloudEntry->timeInfo().lastModificationTime()) {
        qDebug() << QString("Source Db has older entry::"+localEntry->uuid().toHex()+"::"+localEntry->attributes()->value("Title")+":: Parent::"+localEntry->group()->uuid().toHex());
        updateEntryData(localEntry,cloudEntry);
    }
    //remote entry group was changed
    if (localEntry->timeInfo().locationChanged()!=cloudEntry->timeInfo().locationChanged()) {
        bool isRemoved=Tools::hasChild(db2->metadata()->recycleBin(), cloudEntry);
        //entry was moved to normal group
        if (!isRemoved) {
             updateEntryGroup(localEntry,cloudEntry);
        }
        //entry was moved to recycle bin which is also group
        else {
            removeEntry(localEntry);
        }
}


}
//entry exists only in remote database
else {
missingEntries.append(cloudEntry);
}
}
if (missingEntries.length()>0) {
addMissingEntries(missingEntries);
}


}
