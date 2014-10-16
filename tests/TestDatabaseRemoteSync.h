#ifndef TESTDATABASEREMOTESYNC_H
#define TESTDATABASEREMOTESYNC_H
#include <QtCore/QObject>
#include <core/Database.h>
#include <gdrive/GoogleDriveApi.h>
#include <core/Tools.h>
#include <gdrive/GoogleDriveTools.h>
#include <qtdrive/lib/command_delete.h>
#include <QtTest/QSignalSpy>
#include <gdrive/Errors.h>
#include <core/Entry.h>
#include <core/Group.h>
#include <gdrive/GDriveDatabaseSyncBase.h>

using namespace DatabaseSync;
class TestDatabaseRemoteSync: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();

    void testRemoveRemoteEntry();
    void testUpdateRemoteEntry();
    void testMissingRemoteEntry();
    void testUpdateLocalEntry();
    void testMissingLocalEntry();
    void testRemoveLocalEntry();
    void testRemoveLocalEntryToRecycleBin();
    void testRemoveRemoteEntrySlots();
    void testRemoteDatabaseSyncDoNothing();
    void testRemoteDatabaseSyncAmbigiousDb();
    void testRemoteDatabaseSyncNoCloudDb();
    void testRemoteDatabaseSyncLoginError();

private:
    void compareResult(QSharedPointer<GDriveSyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database* readDatabase(const CompositeKey& key,const QString& dbPath);
    Database* createLocalDatabase();
    void uploadDb(const QString& dbPath);
    void deleteDb(const FileInfo& db);
    const CompositeKey getTestCompositeKey();
    void saveDatabase(Database* db, const QString& dbPath);
    Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));

    QString dbName;
    GoogleDriveApi* gdrive=googleDriveApi();
    QString parentDir;
};

#endif // TESTDATABASEREMOTESYNC_H
