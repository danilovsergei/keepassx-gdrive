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


class TestDatabaseRemoteSync: public QObject
{
Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void testRemoveRemoteEntry();
    void testUpdateRemoteEntry();
    void testRemoteDatabaseSyncRemoveEntry();
    void testRemoteDatabaseSyncDoNothing();
    void testRemoteDatabaseSyncAmbigiousDb();
    void testRemoteDatabaseSyncNoCloudDb();
    void testRemoteDatabaseSyncLoginError();

private:
    Database* createLocalDatabase();
    void uploadDb(const QString& dbPath);
    void deleteDb(const FileInfo& db);
    void saveDatabase(Database* db, const QString& dbPath);
    Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));

    QString dbName;
    GoogleDriveApi* gdrive=googleDriveApi();
    QString parentDir;
};

#endif // TESTDATABASEREMOTESYNC_H
