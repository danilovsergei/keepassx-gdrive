#include "GDriveDatabaseSync.h"

#include <QtCore/QDebug>

GDriveDatabaseSync::GDriveDatabaseSync()
{

}

bool GDriveDatabaseSync::compareByUuid(Entry* entry1,Entry* entry2) {
return entry1->uuid()<entry2->uuid();
}

bool GDriveDatabaseSync::compareByCreationTime(Entry* entry1,Entry* entry2) {
return entry1->timeInfo().creationTime()<entry2->timeInfo().creationTime();
}

void GDriveDatabaseSync::addMissingEntries(Database* db, const QList<Entry *> &entries) {
//sort by creation time.Parent group always created before child.Sort guaranties parents will be created before childs
qSort(entries.begin(),entries.end(),GDriveDatabaseSync::compareByCreationTime);
QMap<Uuid,const Group*> groups;
Q_FOREACH(const Group* group,db->rootGroup()->groupsRecursive(true)) {
groups[group->uuid()]=group;
}


qDebug() << "Add missing entries";
Q_FOREACH(Entry* entry,entries) {
if (!isItemNew(db,entry))
  continue;
qDebug() << "Add missing entry";
const Group* group=groups[entry->group()->uuid()];
Entry* newEntry=new Entry();
newEntry->beginUpdate();
newEntry->copyDataFrom(entry);
newEntry->setGroup(group);
newEntry->endUpdate();
}
}


/**
 * @brief isItemNew - checks if item entry was not ever present in db.
 * Avoids such cases when item was removed from db1, but still presents in db2. And db2 reports item is missing in db1
 * @param db  - database to check if item was removed
 * @param entry - entry to check in db
 * @return - true if item was never removed from db, false  - it item was removed
 */
bool GDriveDatabaseSync::isItemNew(Database* db,Entry* entry) {
//TODO implement check
}

void GDriveDatabaseSync::updateEntry(Entry* entry,Entry* new_data) {
qDebug() << "Updating the entry";
entry->beginUpdate();
entry->copyDataFrom(new_data);
entry->endUpdate();
}

void GDriveDatabaseSync::syncDatabases(Database* db1,Database* db2) {
Group* root1=db1->rootGroup();
Group* root2=db2->rootGroup();
QList<Entry*> entries1=root1->entriesRecursive();
QList<Entry*> entries2=root2->entriesRecursive();
QList<Entry*> missingEntries;

qSort(entries1.begin(),entries1.end(),GDriveDatabaseSync::compareByUuid);
qSort(entries2.begin(),entries2.end(),GDriveDatabaseSync::compareByUuid);

bool isFinished=false;
QListIterator<Entry*> it1(entries1);
QListIterator<Entry*> it2(entries2);


Entry* entry1=it1.next();
Entry* entry2=it2.next();

while(it1.hasNext() or it2.hasNext())  {

    if (entry1->uuid()==entry2->uuid()) {

      if (entry1->timeInfo().lastModificationTime()==entry2->timeInfo().lastModificationTime()) {
        qDebug() << QString("match::"+entry1->uuid().toHex()+"::"+entry1->attributes()->value("Title")+":: Parent::"+entry1->group()->uuid().toHex());
      }
      else {
            qDebug() << QString("ModificationTimeMismatch::"+entry1->uuid().toHex()+"::"+entry1->attributes()->value("Title")+":: Parent::"+entry1->group()->uuid().toHex());
            if (entry1->timeInfo().lastModificationTime()<entry2->timeInfo().lastModificationTime()) {
                updateEntry(entry1,entry2);
            }
      }
      //Move both items next because we had a match
      entry1=it1.next();
      entry2=it2.next();
    } else
    {
     //Iterate to the next item to compare it with a previos item from the source because we didn't have a match
      if (entry1->uuid()>entry2->uuid()) {

      qDebug() << QString("diff::"+entry2->uuid().toHex()+"::"+entry2->attributes()->value("Title")+":: Parent::"+entry2->group()->uuid().toHex());
      missingEntries.append(entry2);
      entry2=it2.next();
      }
      else {
      qDebug() << QString("diff::"+entry1->uuid().toHex()+"::"+entry1->attributes()->value("Title")+":: Parent::"+entry1->group()->uuid().toHex());
      entry1=it1.next();
      }
    }
}

addMissingEntries(db1,missingEntries);


}
