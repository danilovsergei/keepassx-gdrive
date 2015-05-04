#include "TestDatabaseSync.h"

#include <QtTest/QTest>
#include "tests.h"
#include <QtCore/QDebug>
#include "crypto/Crypto.h"
#include "core/Tools.h"

void TestDatabaseSync::initTestCase() {
    Crypto::init();
}
void TestDatabaseSync::init()
{

    db1= new Database();
    db2= new Database();
    groupRoot = db1->resolveGroup(db1->rootGroup()->uuid());
    groupRoot2 = db2->resolveGroup(db2->rootGroup()->uuid());
    groupRoot2->setUuid(groupRoot->uuid());
    groupRoot2->setTimeInfo(groupRoot->timeInfo());
    entrySync=DatabaseSyncFactory::createDatabaseSync(DatabaseSyncFactory::SyncId::ENTRY,db1,db2);
    groupSync=DatabaseSyncFactory::createDatabaseSync(DatabaseSyncFactory::SyncId::GROUP,db1,db2);
    allSync=DatabaseSyncFactory::createDatabaseSync(DatabaseSyncFactory::SyncId::ALL,db1,db2);

}

void TestDatabaseSync::cleanup() {
    delete groupRoot;
    delete groupRoot2;
    delete db1;
    delete db2;
}


/**
 * @brief TestDatabaseSync::testEntryAndGroupRemove - check group and entry will be removed from local db after removal from remote db
 * 1. Add group and child entry both to local and remote db
 * 2. Perform sync to make sure databases are identical
 * 3. Remove group and entry from remote db
 * 4. Perform sync
 * 5. Make sure group and entry moved to recycle bin in local db
 */
void TestDatabaseSync::testEntryAndGroupRemove(){
    Group* localGroup = new Group();
    localGroup->setParent(groupRoot);
    localGroup->setUuid(Uuid::random());
    localGroup->setName("TestGroupName");

    Group* remoteGroup = new Group();
    remoteGroup->setParent(groupRoot2);
    remoteGroup->copyDataFrom(localGroup);
    remoteGroup->setUuid(localGroup->uuid());
    remoteGroup->setTimeInfo(localGroup->timeInfo());

    Entry* localEntry=new Entry();
    localEntry->setTitle("New Entry");
    localEntry->setUuid(Uuid::random());
    localEntry->setGroup(localGroup);

    Entry* remoteEntry=new Entry();
    remoteEntry->copyDataFrom(localEntry);
    remoteEntry->setUuid(localEntry->uuid());
    remoteEntry->setGroup(remoteGroup);
    remoteEntry->setTimeInfo(localEntry->timeInfo());

    //doing sync to make sure databases are fully identical
    allSync->syncDatabases();

    Tools::sleep(1000);

    db2->recycleEntry(remoteEntry);
    db2->recycleGroup(remoteGroup);

    //sync to remove both items from local database
    allSync->syncDatabases();

    //check only Recycle bin group created and exists in local db
    QCOMPARE(groupRoot->children().size(),1);
    QCOMPARE(groupRoot->children().first()==db1->metadata()->recycleBin(),true);
    Group* recycleBin=db1->metadata()->recycleBin();
    //check group was removed to recycle bin
    QCOMPARE(recycleBin->children().size(),1);
    QCOMPARE(recycleBin->children().first()->uuid(),remoteGroup->uuid());
    //check entry was removed to recycle bin
    QCOMPARE(recycleBin->entries().size(),1);
    QCOMPARE(recycleBin->entries().first()->uuid(),remoteEntry->uuid());

    delete localEntry;
    delete remoteEntry;
    delete localGroup;
    delete remoteGroup;
    delete recycleBin;
}



/**
 * @brief TestDatabaseSync::testEntryAndGroupAttributesUpdate - check newly created group and entry inside are synced after update
 * 1 Create new group in db2
 * 2 Create new entry in db2 in a new group
 * 3 Check both group and entry will be synced
 */
void TestDatabaseSync::testEntryAndGroupAddUpdate(){
    Group* group = new Group();
    group->setParent(groupRoot2);
    group->setName("title");
    group->setNotes("GroupNotes");

    Entry* entry = new Entry();
    entry->setGroup(group);
    entry->setTitle("NewEntry");

    allSync->syncDatabases();

    //check rootGroup of source db contains one group
    QCOMPARE(groupRoot->children().size(),1);
    QCOMPARE(groupRoot->children().first()->uuid(),group->uuid());

    QCOMPARE(groupRoot->children().first()->entries().size(),1);
    QCOMPARE(groupRoot->children().first()->entries().first()->uuid(),entry->uuid());

    delete entry;
    delete group;




}


/**
 * @brief testGroupAttributesUpdate - check group synced after attributes update
 */
void TestDatabaseSync::testGroupAttributesUpdate() {

    Group* entry = new Group();
    entry->setParent(groupRoot);
    entry->setName("title");
    entry->setNotes("GroupNotes");


    Group* newEntry =new Group();
    newEntry->copyDataFrom(entry);
    newEntry->setUuid(entry->uuid());
    newEntry->setParent(groupRoot2);

    //sleep to change last modification time
    Tools::sleep(1000);
    newEntry->setName("New title");
    newEntry->setNotes("NewGroupNotes");
    QDateTime oldLastUpdated=entry->timeInfo().lastModificationTime();
    groupSync->syncDatabases();

    //verify new password was  migrated to local entry
    QCOMPARE(entry->name(),newEntry->name());
    //verify last modification time was updated
    QCOMPARE(entry->timeInfo().lastModificationTime(),newEntry->timeInfo().lastModificationTime());

    delete entry;
    delete newEntry;
}


/**
 * @brief TestDatabaseSync::testGroupGroupUpdate - check parent group of the group is updated after sync
 */
void TestDatabaseSync::testGroupGroupUpdate() {
    Group* entry = new Group();
    entry->setParent(groupRoot);
    entry->setName("testtitle");

    Group* newEntry =new Group();
    newEntry->copyDataFrom(entry);
    newEntry->setUuid(entry->uuid());
    newEntry->setParent(groupRoot2);

    //create new group in the remote database
    Group* newGroup=new Group();
    newGroup->setName("NewGroup");
    newGroup->setUuid(Uuid::random());
    newGroup->setParent(groupRoot2);

    //create the same group in local database
    Group* newLocalGroup=new Group();
    newLocalGroup->setName("NewGroup");
    newLocalGroup->setUuid(newGroup->uuid());
    newLocalGroup->setParent(groupRoot);

    //sleep to change last modification time
    Tools::sleep(1000);
    newEntry->setParent(newGroup);

    groupSync->syncDatabases();

    //verify parent group of the local group was changed
    QCOMPARE(entry->parentGroup()->uuid(),newEntry->parentGroup()->uuid());
    //verify last modification time was updated
    QCOMPARE(entry->timeInfo().locationChanged(),newEntry->timeInfo().locationChanged());

    delete entry;
    delete newEntry;
    delete newGroup;
    delete newLocalGroup;
}


/**
 * @brief TestDatabaseSync::testGroupAddUpdate - check missing in local db group added to it after sync
 */
void TestDatabaseSync::testGroupAddUpdate() {
    //create new group in the remote database
    Group* newGroup=new Group();
    newGroup->setName("NewGroup");
    newGroup->setUuid(Uuid::random());
    newGroup->setParent(groupRoot2);

    groupSync->syncDatabases();

    //check one child group appeared in rootgroup of local db
    QCOMPARE(groupRoot->children().length(),1);

    delete newGroup;
}



/**
 * @brief TestDatabaseSync::testEntryAddUpdate - check missing in local db entry added to it after sync
 */
void TestDatabaseSync::testEntryAddUpdate() {
    Entry* newEntry = new Entry();
    newEntry->setGroup(groupRoot2);
    newEntry->setTitle("testtitle");

    entrySync->syncDatabases();

    //verify entry appeared in local db
    QCOMPARE(groupRoot->entries().length(),1);
    QCOMPARE(groupRoot->entries().first()->uuid(),newEntry->uuid());
    QCOMPARE(groupRoot->entries().first()->attributes()->value("Title"),newEntry->attributes()->value("Title"));

    //verify last modification time was updated
    QCOMPARE(groupRoot->entries().first()->timeInfo().lastModificationTime(),newEntry->timeInfo().lastModificationTime());

    delete newEntry;
}

/**
 * @brief TestDatabaseSync::testGroupDeleteUpdate - check group was synced locally to recycle bin after it was removed in remote db
 */
void TestDatabaseSync::testGroupDeleteUpdate() {
    Group* entry = new Group();
    entry->setParent(groupRoot);
    entry->setName("NewGroup");

    Group* newEntry =new Group();
    newEntry->copyDataFrom(entry);
    newEntry->setParent(groupRoot2);
    newEntry->setUuid(entry->uuid());

    //sleep to change last modification time
    Tools::sleep(1000);
    db2->recycleGroup(newEntry);
    groupSync->syncDatabases();

    //check recycle bin enabled. must happen after item deletion
    QCOMPARE(db1->metadata()->recycleBin()!=0,true);

    Group* recycleBin=db1->resolveGroup(db1->metadata()->recycleBin()->uuid());

    //verify recycle bin contain removed entry
    QCOMPARE(recycleBin->children().length(),1);
    //verify removed entry is the same was removed
    QCOMPARE(recycleBin->children().first()->uuid(),newEntry->uuid());

    delete entry;
    delete newEntry;
}


/**
 * @brief TestDatabaseSync::testEntryDeleteUpdate - check entry was synced locally to recycle bin after it was removed in remote db
 */
void TestDatabaseSync::testEntryDeleteUpdate() {
    Entry* entry = new Entry();
    entry->setGroup(groupRoot);
    entry->setTitle("testtitle");

    Entry* newEntry =new Entry();
    newEntry->copyDataFrom(entry);
    newEntry->setGroup(groupRoot2);

    //sleep to change last modification time
    Tools::sleep(1000);
    db2->recycleEntry(newEntry);

    entrySync->syncDatabases();

    Group* recycleBin=db1->resolveGroup(db1->metadata()->recycleBin()->uuid());

    //verify entry was removed from local db root group
    QCOMPARE(groupRoot->entries().length(),0);
    //verify recycle bin contain removed entry
    QCOMPARE(recycleBin->entries().length(),1);
    //verify removed entry is the same was removed
    QCOMPARE(recycleBin->entries().first()->uuid(),newEntry->uuid());

    delete entry;
    delete newEntry;
}



/**
 * @brief TestDatabaseSync::testEntryGroupUpdate - check entry was synced after group update
 */
void TestDatabaseSync::testEntryGroupUpdate() {

Entry* entry = new Entry();
entry->setGroup(groupRoot);
entry->setTitle("testtitle");
Entry* newEntry =new Entry();
newEntry->copyDataFrom(entry);
newEntry->setGroup(groupRoot2);

//create new group in the remote database
Group* newGroup=new Group();
newGroup->setName("NewGroup");
newGroup->setUuid(Uuid::random());
newGroup->setParent(groupRoot2);

//create the same group in local database
Group* newLocalGroup=new Group();
newLocalGroup->setName("NewGroup");
newLocalGroup->setUuid(newGroup->uuid());
newLocalGroup->setParent(groupRoot);

//sleep to change last modification time
Tools::sleep(1000);
newEntry->setGroup(newGroup);


entrySync->syncDatabases();

//verify group of the local entry was changed
QCOMPARE(entry->group()->uuid(),newEntry->group()->uuid());
//verify last modification time was updated
QCOMPARE(entry->timeInfo().lastModificationTime(),newEntry->timeInfo().lastModificationTime());
QCOMPARE(entry->timeInfo().locationChanged(),newEntry->timeInfo().locationChanged());

delete entry;
delete newEntry;
delete newGroup;
delete newLocalGroup;
}


/**
 * @brief TestDatabaseSync::testEntryAttributesUpdate - check entry was synced after password update
 */
void TestDatabaseSync::testEntryAttributesUpdate() {
Entry* entry = new Entry();
entry->setGroup(groupRoot);
entry->setTitle("testtitle");
entry->setPassword("1234");
entry->attributes()->set("attr1", "abc");


Entry* newEntry =new Entry();
newEntry->copyDataFrom(entry);
newEntry->setGroup(groupRoot2);

//sleep to change last modification time
Tools::sleep(1000);
newEntry->setPassword("5678");
QDateTime oldLastUpdated=entry->timeInfo().lastModificationTime();

entrySync->syncDatabases();

//verify new password was  migrated to local entry
QCOMPARE(entry->password(),newEntry->password());
//verify last modification time was updated
QCOMPARE(entry->timeInfo().lastModificationTime(),newEntry->timeInfo().lastModificationTime());
//verify local item was saved in history
QCOMPARE(entry->historyItems().first()->timeInfo().lastModificationTime(),oldLastUpdated);

delete entry;
delete newEntry;
}


QTEST_GUILESS_MAIN(TestDatabaseSync)
