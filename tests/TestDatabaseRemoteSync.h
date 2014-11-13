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
#include <gdrive/helpers/SyncRecentDbHelper.h>

using namespace DatabaseSync;
class TestDatabaseRemoteSync: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void testRemoveLocalEntryToRecycleBin();
    void testRemoveRemoteEntrySlots();
    void testRemoteDatabaseSyncDoNothing();
    void testRemoteDatabaseSyncAmbigiousDb();
    void testRemoteDatabaseSyncNoCloudDb();
    void testRemoteDatabaseSyncLoginError();
    void testUpdateLocal();
    void testRemoveLocal();
    void testMissingLocal();
    void testRemoveRemote();
    void testMissingRemote();
    void testUpdateRemote();

private:
    const QString dbName = "test";
    const QString parentDir = "0B8tltL21wts3alc0eS00UVRLNnc";
    const QString newNotes = "New Notes";
    const QString   oldDbPath = QDir::tempPath() + QDir::separator() + "oldDb.kdbx";
    const QString   dbPath = QDir::tempPath() + QDir::separator() + dbName + ".kdbx";
    const QString newPassword = "NewPassword";
    const QString password = "Password";
    Database *db;
    Group *group;
    Group *newGroup;
    Entry *entry;
    Entry *newEntry;

    Uuid   newEntryUuid;
    Uuid   newGroupUuid;
    Group *rootGroup;
    QSharedPointer<SyncRecentDbHelper> syncHelper;

    void compareResult(QSharedPointer<GDriveSyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database* readDatabase(const CompositeKey& key,const QString& dbPath);
    Database* createLocalDatabase();
    void uploadDb(const QString& dbPath);
    void deleteDb(const FileInfo& db);
    const CompositeKey getTestCompositeKey();
    void saveDatabase(Database* db, const QString& dbPath);
    Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));
    Entry* createEntry(const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(const QString& groupName=QString("testGroup"));
    GoogleDriveApi* gdrive=googleDriveApi();
    template <typename T> void templateUpdateLocal();
    template <typename T> void templateRemoveLocal();
    template <typename T> void templateMissingLocal();
    template <typename T> void templateRemoveRemote();
    template <typename T> void templateMissingRemote();
    template <typename T> void templateUpdateRemote();
    template <typename T> void prepare();
    template <typename T> void setUpdateLocal();
    template<typename T>  void setRemoveLocal();
    template<typename T>  void setMissingLocal();
    template<typename T>  void setRemoveRemote();
    template<typename T>  void setMissingRemote();
    template<typename T>  void setUpdateRemote();
    template<typename T> void validateUpdateLocal(const QSharedPointer<GDriveSyncObject>& actual);
    template<typename T> void validateRemoveLocal(const QSharedPointer<GDriveSyncObject>& actual);
    template<typename T> void validateMissingLocal(const QSharedPointer<GDriveSyncObject>& actual);
    template<typename T> void validateRemoveRemote(const QSharedPointer<GDriveSyncObject>& actual);
    template<typename T> void validateMissingRemote(const QSharedPointer<GDriveSyncObject>& actual);
    template<typename T> void validateUpdateRemote(const QSharedPointer<GDriveSyncObject>& actual);
};

#endif // TESTDATABASEREMOTESYNC_H