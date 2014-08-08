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
    void testRemoteDatabaseSyncSeq();
    void testRemoteDatabaseSyncRemoveEntry();
    void testRemoteDatabaseSyncDoNothing();
    void testRemoteDatabaseSyncAmbigiousDb();
    void testRemoteDatabaseSyncNoCloudDb();
    void testRemoteDatabaseSyncLoginError();

private:
    Database* createLocalDatabase();
    void uploadDb(const QString& dbPath);
    void deleteDb(const FileInfo& db);

    QString dbName;
    GoogleDriveApi* gdrive=googleDriveApi();
    QString parentDir;
};

#endif // TESTDATABASEREMOTESYNC_H
