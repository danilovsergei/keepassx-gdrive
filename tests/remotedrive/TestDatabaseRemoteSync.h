#ifndef TESTDATABASEREMOTESYNC_H
#define TESTDATABASEREMOTESYNC_H

#include <QObject>
#include <core/Database.h>
#include <core/Tools.h>
#include <remotedrive/gdrive/GoogleDriveTools.h>
#include <QSignalSpy>
#include <remotedrive/Errors.h>
#include <core/Entry.h>
#include <core/Group.h>
#include <QScopedPointer>
#include <keys/PasswordKey.h>
#include <format/KeePass2Writer.h>
#include <core/qsavefile.h>
#include <QDebug>
#include <qmath.h>
#include <core/Entry.h>
#include <QTest>
#include "tests.h"
#include "crypto/Crypto.h"

#include "remotedrive/SyncObject.h"
#include "remotedrive/CommandsFactory.h"
#include "remotedrive/AuthCredentials.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/RemoteCommand.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"
#include "GDriveTestUtils.h"

using namespace DatabaseSyncObject;

/**
 * @brief The TestDatabaseRemoteSync class
 */
class TestDatabaseRemoteSync : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();
  void cleanupTestCase();
  void init();
  void cleanup();
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
  void cleanupLocalDb();
  void cleanupFiles(bool includeOldDbPath = false);
  QString dbName;
  const QString parentDir = "0B8tltL21wts3alc0eS00UVRLNnc";
  const QString newNotes = "New Notes";
  // const QString   oldDbPath = QDir::tempPath() + QDir::separator() + "oldDb.kdbx";
  QString dbPath;
  QString oldDbPath;
  const QString newPassword = "NewPassword";
  const QString password = "Password";

  QScopedPointer<Database> db;
  QScopedPointer<Group> group;
  QScopedPointer<Group> newGroup;
  QScopedPointer<Entry> entry;
  QScopedPointer<Entry> newEntry;
  QScopedPointer<Group> rootGroup;

  Uuid newEntryUuid;
  Uuid newGroupUuid;

  void compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey, int> expectedMap);
  Database *readDatabase(const CompositeKey &key, const QString &dbPath);
  Database *createLocalDatabase();
  const CompositeKey getTestCompositeKey();
  // Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
  // Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));
  // Entry* createEntry(const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
  // Group* createGroup(const QString& groupName=QString("testGroup"));

  RemoteDriveApi *remoteDrive = Q_NULLPTR;
  GDriveTestUtils *testUtils = Q_NULLPTR;
  AuthCredentials *creds = Q_NULLPTR;
  CommandsFactory *commandsFactory = Q_NULLPTR;

  template<typename T> void templateUpdateLocal();
  template<typename T> void templateRemoveLocal();
  template<typename T> void templateMissingLocal();
  template<typename T> void templateRemoveRemote();
  template<typename T> void templateMissingRemote();
  template<typename T> void templateUpdateRemote();
  template<typename T> void prepare();
  template<typename T> void setUpdateLocal();
  template<typename T> void setRemoveLocal();
  template<typename T> void setMissingLocal();
  template<typename T> void setRemoveRemote();
  template<typename T> void setMissingRemote();
  template<typename T> void setUpdateRemote();
  template<typename T> void validateUpdateLocal(const QSharedPointer<SyncObject> &actual);
  template<typename T> void validateRemoveLocal(const QSharedPointer<SyncObject> &actual);
  template<typename T> void validateMissingLocal(const QSharedPointer<SyncObject> &actual);
  template<typename T> void validateRemoveRemote(const QSharedPointer<SyncObject> &actual);
  template<typename T> void validateMissingRemote(const QSharedPointer<SyncObject> &actual);
  template<typename T> void validateUpdateRemote(const QSharedPointer<SyncObject> &actual);
};

#endif // TESTDATABASEREMOTESYNC_H
