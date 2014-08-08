#ifndef TESTDATABASESYNC_H
#define TESTDATABASESYNC_H
#include <QtCore/QObject>
#include "gdrive/GDriveEntriesSync.h"
#include "gdrive/GDriveGroupsSync.h"
#include "gdrive/GDriveDatabaseSyncFactory.h"

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
QSharedPointer<GDriveDatabaseSync> groupSync;
QSharedPointer<GDriveDatabaseSync> entrySync;
QSharedPointer<GDriveDatabaseSync> allSync;
Group* groupRoot;
Group* groupRoot2;
};

#endif // TESTDATABASESYNC_H
