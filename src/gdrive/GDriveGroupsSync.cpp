#include "GDriveGroupsSync.h"


GDriveGroupsSync::GDriveGroupsSync(Database* db1,Database* db2)

{
   this->db1=db1;
   this->db2=db2;
}

GDriveGroupsSync::~GDriveGroupsSync(){

}

bool GDriveGroupsSync::compareByCreationTime(Group* entry1,Group* entry2) {
return entry1->timeInfo().creationTime()<entry2->timeInfo().creationTime();
}

void GDriveGroupsSync::updateEntryData(Group* entry, Group* new_data) {
    qDebug() << "Updating the group";
    Q_ASSERT(entry);
    Q_ASSERT(new_data);
    entry->copyDataFrom(new_data);
    entry->setTimeInfo(new_data->timeInfo());
}

void GDriveGroupsSync::updateEntryGroup(Group* entry, Group *new_data) {
    qDebug() << "Updating parent group";
    Q_ASSERT(entry);
    Q_ASSERT(new_data);
    Q_ASSERT(new_data->parentGroup());
    Group* targetGroup=entries1[new_data->parentGroup()->uuid()];
    Q_ASSERT(targetGroup);
    entry->setParent(targetGroup);
    entry->setTimeInfo(new_data->timeInfo());
}

void GDriveGroupsSync::removeEntry(Group* entry){
    Group* toRecycle=entries1[entry->uuid()];
    Q_ASSERT(toRecycle);
    db1->recycleGroup(toRecycle);

}

void GDriveGroupsSync::addMissingEntries(QList<Group*> missingEntries) {
    //we can not check if item is really a new item because there is no way to check locally whether it was removed w/o recycle bin
    if (!db2->metadata()->recycleBinEnabled()) {
      //TODO show all missingEntries dialog to the user with ability to choose which items should be really removed
      //Strongly recommend to the user to enable recycle bin to prevent manual sync
      return;
    }
  //sort by creation time.Parent group always created before child.Sort guaranties parents will be created before childs
  qSort(missingEntries.begin(),missingEntries.end(),GDriveGroupsSync::compareByCreationTime);

  qDebug() << "Add missing entries";
  Q_FOREACH(Group* entry,missingEntries) {
  Group* parentGr=entry->parentGroup();
  //get missing entry group in the source database
  Group* group=entries1[entry->parentGroup()->uuid()];
  Q_ASSERT(group);

  //creating new entry
  Group* newEntry=new Group();
  newEntry->copyDataFrom(entry);
  newEntry->setParent(group);
  newEntry->setTimeInfo(entry->timeInfo());
  qDebug() << "Add missing entry "+newEntry->name()+" to group::"+group->getGroupName();
  }
}


void GDriveGroupsSync::syncDatabases() {
//make sure root groups are the same.It has to be if we are syncing the same database
Q_ASSERT(db1->rootGroup()->uuid()==db2->rootGroup()->uuid());
Q_ASSERT(db1->rootGroup()->timeInfo().lastModificationTime()==db2->rootGroup()->timeInfo().lastModificationTime());

entries1=db1->rootGroup()->groupsMapRecursive(true);
entries2=db2->rootGroup()->groupsMapRecursive(true);
QList<Group*> missingEntries;
Q_FOREACH(Group* cloudEntry, entries2) {
    //avoid to sync root groups. They always supposed to be the same
    if (cloudEntry==db2->rootGroup())
        continue;

    //entry exists in both databases
    if (entries1.contains(cloudEntry->uuid())) {
    Group* localEntry= entries1[cloudEntry->uuid()];
    //local entry last update time is older than remote entry
    if (localEntry->timeInfo().lastModificationTime()<cloudEntry->timeInfo().lastModificationTime()) {
        qDebug() << QString("Source Db has older entry::");
        updateEntryData(localEntry,cloudEntry);
    }
    //remote entry group was changed
    if (localEntry->timeInfo().locationChanged()!=cloudEntry->timeInfo().locationChanged()) {
        //bool isRemoved=false;
        //if (cloudEntry->parentGroup()==db2->metadata()->recycleBin()) {
        //isRemoved=true;
        //}
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
