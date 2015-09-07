#ifndef TESTDATABASESYNC_H
#define TESTDATABASESYNC_H
#include <QObject>
#include "remotedrive/EntriesSync.h"
#include "remotedrive/GroupsSync.h"
#include "remotedrive/DatabaseSyncBase.h"
#include "remotedrive/DatabaseSyncFactory.h"

class TestDatabaseSync: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEntryAttributesUpdate();
    void testEntryGroupUpdate();
    void testEntryDeleteUpdate();
    void testEntryAddUpdate();

    void testGroupAttributesUpdate();
    void testGroupGroupUpdate();
    void testGroupDeleteUpdate();
    void testGroupAddUpdate();


    void testEntryAndGroupAddUpdate();
    void testEntryAndGroupRemove();

    void initTestCase();
    void init();
    void cleanup();
private:
Database* db1;
Database* db2;
QSharedPointer<DatabaseSyncBase> groupSync;
QSharedPointer<DatabaseSyncBase> entrySync;
QSharedPointer<DatabaseSyncBase> allSync;
Group* groupRoot;
Group* groupRoot2;
};

#endif // TESTDATABASESYNC_H
