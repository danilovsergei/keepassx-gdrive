#ifndef TESTDATABASEREMOTESYNC_H
#define TESTDATABASEREMOTESYNC_H

#include <QtCore/QObject>
#include <core/Database.h>
#include <core/Tools.h>
#include <remotedrive/gdrive/GoogleDriveTools.h>
#include <QtTest/QSignalSpy>
#include <remotedrive/Errors.h>
#include <core/Entry.h>
#include <core/Group.h>
#include <QtCore/QScopedPointer>
#include <keys/PasswordKey.h>
#include <format/KeePass2Writer.h>
#include <core/qsavefile.h>
#include <QtCore/QDebug>
#include <QtCore/qmath.h>
#include <core/Entry.h>
#include <QtTest/QTest>
#include "tests.h"
#include "crypto/Crypto.h"

#include "remotedrive/SyncObject.h"
#include "remotedrive/CommandsFactory.h"
#include "remotedrive/AuthCredentials.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/Command.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"
#include "GDriveTestUtils.h"








using namespace DatabaseSyncObject;

/**
 * @brief The TestDatabaseRemoteSync class
 */
class TestDatabaseRemoteSync: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void init();
    void cleanup();
    void testSingleRun();
    void testRemoveLocalEntryToRecycleBin();
    void testRemoveRemoteEntrySlots();
    void testRemoteDatabaseSyncDoNothing();    
    void testRemoteDatabaseSyncNoCloudDb();
    void testRemoteDatabaseSyncLoginError();
    void testUpdateLocal();
    void testRemoveLocal();
    void testMissingLocal();
    void testRemoveRemote();
    void testMissingRemote();
    void testUpdateRemote();

    // Commented because debug code was removed from production code. Need to find better way for it
    // void testRemoteDatabaseSyncAmbigiousDb();

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

    void compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database* readDatabase(const CompositeKey& key,const QString& dbPath);
    Database* createLocalDatabase();
    const CompositeKey getTestCompositeKey();
    Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));
    Entry* createEntry(const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(const QString& groupName=QString("testGroup"));

    RemoteDriveApi *remoteDrive = Q_NULLPTR;
    GDriveTestUtils* testUtils;

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
    template<typename T> void validateUpdateLocal(const QSharedPointer<SyncObject>& actual);
    template<typename T> void validateRemoveLocal(const QSharedPointer<SyncObject>& actual);
    template<typename T> void validateMissingLocal(const QSharedPointer<SyncObject>& actual);
    template<typename T> void validateRemoveRemote(const QSharedPointer<SyncObject>& actual);
    template<typename T> void validateMissingRemote(const QSharedPointer<SyncObject>& actual);
    template<typename T> void validateUpdateRemote(const QSharedPointer<SyncObject>& actual);
};

#endif // TESTDATABASEREMOTESYNC_H
