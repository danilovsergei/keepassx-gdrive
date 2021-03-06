#include "TestDatabaseRemoteSync.h"

Q_DECLARE_METATYPE(QSharedPointer<SyncObject>)
Q_DECLARE_METATYPE(Database *)

/**
 * @brief TestDatabaseRemoteSync::initTestCase executed before class execution
 */
void TestDatabaseRemoteSync::initTestCase()
{
  Crypto::init();
  qDebug() << "Running init testcase";

  creds = new GoogleDriveCredentials(this);
  creds->init();
  commandsFactory = new CommandsFactoryImpl(this, creds);
  remoteDrive = new RemoteDriveApi(this, commandsFactory);
  testUtils = new GDriveTestUtils(remoteDrive);
}

void TestDatabaseRemoteSync::cleanupTestCase()
{
  // delete testUtils;
}

/**
 * @brief TestDatabaseRemoteSync::init executed before each testmethod
 */
void TestDatabaseRemoteSync::init()
{
  dbPath = testUtils->generateTempDbPath();
  oldDbPath = testUtils->generateTempDbPath("old_test");
  dbName = QFileInfo(dbPath).fileName();

  db.reset(testUtils->createLocalDatabase(dbPath));
  rootGroup.reset(db->resolveGroup(db->rootGroup()->uuid()));
  group.reset(testUtils->createGroup("Group"));
  newGroup.reset(testUtils->createGroup("NewGroup"));
  entry.reset(testUtils->createEntry("Entry", password));
  newEntry.reset(testUtils->createEntry("NewEntry", password));
  newEntryUuid = newEntry->uuid();
  newGroupUuid = newGroup->uuid();

  // remove all remote databases if exist
  testUtils->deleteAllDb(dbName);
}

void TestDatabaseRemoteSync::cleanup()
{
  qDebug() << "starting cleanup";
  testUtils->deleteAllDb(dbName);
  cleanupLocalDb();
  cleanupFiles(true);
}

void TestDatabaseRemoteSync::cleanupLocalDb()
{
  entry.reset();
  newEntry.reset();
  group.reset();
  newGroup.reset();
  rootGroup.reset();
  db.reset();
}

void TestDatabaseRemoteSync::cleanupFiles(bool includeOldDbPath)
{
  if (QFile(dbPath).exists())
    QFile::remove(dbPath);
  if (includeOldDbPath && QFile(oldDbPath).exists())
    QFile::remove(oldDbPath);
}

const CompositeKey TestDatabaseRemoteSync::getTestCompositeKey()
{
  CompositeKey key;

  key.addKey(PasswordKey("test"));
  return key;
}

/**
 * @brief TestDatabaseRemoteSync::compareResult compares actual and expected
 * results for  database sync
 * @param actual what actual sync provided
 * @param expectedMap what is expected
 */
void TestDatabaseRemoteSync::compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey,
                                                                                   int> expectedMap)
{
  QSharedPointer<SyncObject> expected = QSharedPointer<SyncObject>(
    new SyncObject());
  Q_FOREACH(SyncMapKey key, expectedMap.keys()) {
    expected->set(static_cast<ObjectType>(key.get(0)),
                  static_cast<ObjectName>(key.get(
                                            1)),
                  static_cast<ObjectLocation>(key.get(
                                                2)),
                  expectedMap.value(key));
  }
  QMap<SyncMapKey, QPair<int, int> > result = actual->compare(expected);
  Q_FOREACH(SyncMapKey key, result.keys()) {
    qDebug()
      <<QString("%1 : %2 <> %3").arg(key.toString(),
                                     QString::number(result.value(key).first),
                                     QString::number(result.value(key).second));
  }
  QCOMPARE(0, result.size());
}

/**
 * @brief readDatabase reads stored locally database into
 * the memory
 */
Database *TestDatabaseRemoteSync::readDatabase(const CompositeKey &key, const QString &dbPath)
{
  KeePass2Reader reader;
  Database *db = Q_NULLPTR;
  QFile file(dbPath);

  Q_ASSERT(file.open(QIODevice::ReadOnly));
  db = reader.readDatabase(dbPath, key);
  Q_ASSERT(db != Q_NULLPTR);
  return db;
}

/**
 * @brief TestDatabaseRemoteSync::testRemoveRemote upload database
 * with one entry/group to the cloud. Then remove entry locally and check sync reports
 * that remote database entry should be removed
 */
void TestDatabaseRemoteSync::testRemoveRemote()
{
  templateRemoveRemote<Entry *>();
  cleanup();
  init();
  templateRemoveRemote<Group *>();
}

template<typename T>
void TestDatabaseRemoteSync::templateRemoveRemote()
{
  prepare<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  Tools::sleep(1000);
  // remove entry/group from local db instance
  setRemoveRemote<T>();

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateRemoveRemote<T>(actual);
}

template<> void TestDatabaseRemoteSync::setRemoveRemote<Entry *>()
{
  db->recycleEntry(newEntry.data());
}

template<> void TestDatabaseRemoteSync::setRemoveRemote<Group *>()
{
  db->recycleGroup(newGroup.data());
}

template<> void TestDatabaseRemoteSync::validateRemoveRemote<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SGroup(), SRemoved(), SRemote()), 1);
  // mising recycle bin group
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateRemoveRemote<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SRemoved(), SRemote()), 1);
  // mising recycle bin group
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testMissingRemote checks entry/group reported as missing in remote db when removed in local db and sync with remote
 */
void TestDatabaseRemoteSync::testMissingRemote()
{
  templateMissingRemote<Entry *>();
  cleanup();
  init();
  templateMissingRemote<Group *>();
}

template<typename T>
void TestDatabaseRemoteSync::templateMissingRemote()
{
  prepare<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  Tools::sleep(1000);
  setMissingRemote<T>();

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateMissingRemote<T>(actual);
}

template<> void TestDatabaseRemoteSync::setMissingRemote<Entry *>()
{
  testUtils->createEntry(db.data());
}

template<> void TestDatabaseRemoteSync::setMissingRemote<Group *>()
{
  testUtils->createGroup(db.data());
}

template<> void TestDatabaseRemoteSync::validateMissingRemote<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateMissingRemote<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SMissing(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testMissingRemote checks entry/group reported as older in remote db when updated in local db and sync with remote
 */
void TestDatabaseRemoteSync::testUpdateRemote()
{
  qDebug() << "Start tests for Entry";
  templateUpdateRemote<Entry *>();
  cleanup();
  init();
  qDebug() << "Start tests for Group";
  templateUpdateRemote<Group *>();
}

template<typename T>
void TestDatabaseRemoteSync::templateUpdateRemote()
{
  prepare<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));
  Tools::sleep(1000);
  setUpdateRemote<T>();
  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateUpdateRemote<T>(actual);
}

template<> void TestDatabaseRemoteSync::setUpdateRemote<Entry *>()
{
  newEntry->setPassword("Updated_Password");
}

template<> void TestDatabaseRemoteSync::setUpdateRemote<Group *>()
{
  newGroup->setNotes("Updated_Notes");
}

template<> void TestDatabaseRemoteSync::validateUpdateRemote<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SGroup(), SOlder(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateUpdateRemote<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check change was recorded in remote database
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SOlder(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testUpdateLocal checks updated remote
 * entry/group was also updated locally after sync
 */
void TestDatabaseRemoteSync::testUpdateLocal()
{
  templateUpdateLocal<Entry *>();
  cleanup();
  init();
  templateUpdateLocal<Group *>();
}

/**
 * @brief TestDatabaseRemoteSync::testUpdateLocalEntry checks updated remote
 * entry was also updated locally after sync
 */
template<typename T>
void TestDatabaseRemoteSync::templateUpdateLocal()
{
  prepare<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), oldDbPath));
  Tools::sleep(1000);
  setUpdateLocal<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  // completely delete latest local db version
  cleanupLocalDb();
  cleanupFiles();

  // rename old version to have the same name as remote db to let sync work
  QFile::rename(oldDbPath, dbPath);

  // re read old version which has 1 entry removed
  db.reset(readDatabase(getTestCompositeKey(), dbPath));
  rootGroup.reset(db->resolveGroup(db->rootGroup()->uuid()));

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateUpdateLocal<T>(actual);
}

template<> void TestDatabaseRemoteSync::setUpdateLocal<Group *>()
{
  newGroup->setNotes(newNotes);
}

template<> void TestDatabaseRemoteSync::setUpdateLocal<Entry *>()
{
  newEntry->setPassword(newPassword);
}

template<> void TestDatabaseRemoteSync::prepare<Group *>()
{
  group->setParent(rootGroup.data());
  newGroup->setParent(rootGroup.data());
}

template<> void TestDatabaseRemoteSync::prepare<Entry *>()
{
  qDebug() << "Doing prepare stage";
  entry->setGroup(rootGroup.data());
  newEntry->setGroup(rootGroup.data());
  qDebug() << "Finish prepare stage";
}

template<> void TestDatabaseRemoteSync::validateUpdateLocal<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Group *> entries = db->rootGroup()->groupsMapRecursive(false);
  QCOMPARE(entries.contains(newGroupUuid) && entries.value(
             newGroupUuid)->notes() == newNotes, true);
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SGroup(), SOlder(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateUpdateLocal<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Entry *> entries = db->rootGroup()->entriesMapRecursive();
  QCOMPARE(entries.contains(newEntryUuid) && entries.value(
             newEntryUuid)->attributes()->value("Password") == newPassword, true);
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SOlder(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testUpdateLocal checks removed remote
 * entry/group was also removed locally to recycle bin after sync
 */
void TestDatabaseRemoteSync::testRemoveLocal()
{
  templateRemoveLocal<Entry *>();
  cleanup();
  init();
  templateRemoveLocal<Group *>();
}

template<typename T>
void TestDatabaseRemoteSync::templateRemoveLocal()
{
  prepare<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), oldDbPath));
  Tools::sleep(1000);
  setRemoveLocal<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  // completely delete latest local db version
  cleanupLocalDb();
  cleanupFiles();

  // rename old version to have the same name as remote db to let sync work
  QFile::rename(oldDbPath, dbPath);

  // re read old version which has 1 entry removed
  db.reset(readDatabase(getTestCompositeKey(), dbPath));
  rootGroup.reset(db->resolveGroup(db->rootGroup()->uuid()));

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateRemoveLocal<T>(actual);
}

template<> void TestDatabaseRemoteSync::setRemoveLocal<Group *>()
{
  db->recycleGroup(newGroup.data());
}

template<> void TestDatabaseRemoteSync::setRemoveLocal<Entry *>()
{
  db->recycleEntry(newEntry.data());
}

template<> void TestDatabaseRemoteSync::validateRemoveLocal<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Entry *> entries = db->rootGroup()->entriesMapRecursive();
  QCOMPARE(entries.contains(
             newEntryUuid)
           && db->metadata()->recycleBin()->entries().contains(db->resolveEntry(
                                                                 newEntryUuid)),
           true);
  QMap<SyncMapKey, int> expectedMap;

  // since entry was removed new recycle bin group should be created for it
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SLocal()), 1);

  expectedMap.insert(SyncMapKey(SEntry(), SRemoved(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateRemoveLocal<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Group *> entries = db->rootGroup()->groupsMapRecursive(false);
  QCOMPARE(entries.contains(
             newGroupUuid)
           && db->metadata()->recycleBin()->children().contains(db->resolveGroup(
                                                                  newGroupUuid)),
           true);
  QMap<SyncMapKey, int> expectedMap;

  // since entry was removed new recycle bin group should be created for it
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SLocal()), 1);

  expectedMap.insert(SyncMapKey(SGroup(), SRemoved(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testMissingLocal checks missing locally
 * entry/group was added after sync
 */
void TestDatabaseRemoteSync::testMissingLocal()
{
  templateUpdateLocal<Entry *>();
  cleanup();
  init();
  templateUpdateLocal<Group *>();
}

template<typename T>
void TestDatabaseRemoteSync::templateMissingLocal()
{
  Q_ASSERT(testUtils->saveDatabase(db.data(), oldDbPath));
  Tools::sleep(1000);

  // prepare will create entry which will present only in remote database
  prepare<T>();

  // setMissingLocal<T>();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  // completely delete latest local db version
  cleanup();

  // rename old version to have the same name as remote db to let sync work
  QFile::rename(oldDbPath, dbPath);

  // re read old version which has 1 entry removed
  db.reset(readDatabase(getTestCompositeKey(), dbPath));
  rootGroup.reset(db->resolveGroup(db->rootGroup()->uuid()));

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was updated in local database
  validateRemoveLocal<T>(actual);
}

template<> void TestDatabaseRemoteSync::validateMissingLocal<Entry *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Entry *> entries = db->rootGroup()->entriesMapRecursive();
  QCOMPARE(entries.contains(newEntryUuid)
           && rootGroup->entries().contains(db->resolveEntry(newEntryUuid)), true);
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SMissing(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

template<> void TestDatabaseRemoteSync::validateMissingLocal<Group *>(
  const QSharedPointer<SyncObject> &actual)
{
  // check entry was updated in local database
  QMap<Uuid, Group *> entries = db->rootGroup()->groupsMapRecursive(false);
  QCOMPARE(entries.contains(newGroupUuid)
           && rootGroup->children().contains(db->resolveGroup(newGroupUuid)), true);
  QMap<SyncMapKey, int> expectedMap;

  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testRemoveLocalEntryToRecycleBin checks
 * remotely added and immediately removed entry was created locally and moved to
 * recycle bin
 */
void TestDatabaseRemoteSync::testRemoveLocalEntryToRecycleBin()
{
  Q_ASSERT(testUtils->saveDatabase(db.data(), oldDbPath));
  Tools::sleep(1000);
  // create new entry which will present only in remote db
  newEntry.reset(testUtils->createEntry(db.data(), "NewEntry", "New Password"));
  db->recycleEntry(newEntry.data());
  Uuid uuid = newEntry->uuid();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));

  // completely delete latest local db version
  cleanupLocalDb();
  cleanupFiles();

  // rename old version to have the same name as remote db to let sync work
  QFile::rename(oldDbPath, dbPath);

  // read old version which has 1 entry removed
  db.reset(readDatabase(getTestCompositeKey(), dbPath));

  // run sync database syncronously
  QSharedPointer<SyncObject> actual = testUtils->syncDatabase(db.data(), dbPath);

  // check entry was synced to local database
  QList<Entry *> entries = db->rootGroup()->entries();
  QCOMPARE(db->resolveEntry(uuid) != Q_NULLPTR, true);

  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SMissing(), SLocal()), 1);

  // since entry was removed new recycle bin group should be created for it
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SLocal()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief the same as testRemoveRemoteEntry but uses signal/slots to detect sync
 * is done
 */
void TestDatabaseRemoteSync::testRemoveRemoteEntrySlots()
{
  qRegisterMetaType < QSharedPointer < SyncObject > >
    ("QSharedPointer<SyncObject>");

  // save database to file since upload db takes local file
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));
  Q_ASSERT(testUtils->uploadDb(dbPath));
  QDateTime beforeRemoval = db->metadata()->lastModifiedDate();
  Tools::sleep(1000);

  db->recycleEntry(entry.data());
  QDateTime afterRemoval = db->metadata()->lastModifiedDate();
  Q_ASSERT(testUtils->saveDatabase(db.data(), dbPath));

  KeePassxDriveSync::Command syncCommand = remoteDrive->sync();
  syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,
                                                       db.data()).addOption(
                              OPTION_ABSOLUTE_DB_NAME, dbPath).build());
  syncCommand->waitForFinish();
  // make sure no errors happen
  QCOMPARE(syncCommand->getErrorCode(), static_cast<int>(Errors::NO_ERROR));

  // make sure Entry still removed after sync
  QCOMPARE(entry->group()->uuid(), db->metadata()->recycleBin()->uuid());
  QList<QVariant> args = syncCommand->getResult();
  // expect:
  // 0: QSharedPointer<SyncObject> syncObject
  QCOMPARE(args.count(), 1);
  QSharedPointer<SyncObject> actual = args.at(0).value < QSharedPointer <
                                                           SyncObject > >();
  QMap<SyncMapKey, int> expectedMap;
  expectedMap.insert(SyncMapKey(SEntry(), SRemoved(), SRemote()), 1);

  // since entry was removed new recycle bin group was automatically created for
  // it
  expectedMap.insert(SyncMapKey(SGroup(), SMissing(), SRemote()), 1);
  compareResult(actual, expectedMap);
}

/**
 * @brief TestDatabaseRemoteSync::testRemoteDatabaseSyncDoNothing - test sync
 * returns syncDone when databases are identical
 */
void TestDatabaseRemoteSync::testRemoteDatabaseSyncDoNothing()
{
  qRegisterMetaType < QSharedPointer < SyncObject > >
    ("QSharedPointer<SyncObject>");
  Q_ASSERT(testUtils->uploadDb(dbPath));
  qDebug() << "passed this step";
  Tools::sleep(1000);

  KeePassxDriveSync::Command syncCommand = remoteDrive->sync();
  syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,
                                                       db.data()).addOption(
                              OPTION_ABSOLUTE_DB_NAME, dbPath).build());
  syncCommand->waitForFinish();
  // make sure no errors happen
  QCOMPARE(syncCommand->getErrorCode(), static_cast<int>(Errors::NO_ERROR));
}

/**
 * @brief TestDatabaseRemoteSync::testRemoteDatabaseSyncAmbigiousDb - test sync
 * returns an error when 2 databases with the same names exist in the the drive
 */
// Commented because debug code was removed from production code. Need to find better way for it
// void TestDatabaseRemoteSync::testRemoteDatabaseSyncAmbigiousDb()
// {
// Database *db = createLocalDatabase();
// QString dbPath(QDir::tempPath() + QDir::separator() + dbName + ".kdbx");

// uploadDb(dbPath);
// DebugGoogleDriveApi::setDebugOption(
// DebugGoogleDriveApi::DEBUG_CHECK_FOR_EXISTING_CLOUD_DB,
// "0");
// uploadDb(dbPath);
// DebugGoogleDriveApi::setDebugOption(
// DebugGoogleDriveApi::DEBUG_CHECK_FOR_EXISTING_CLOUD_DB,
// "1");
// Tools::sleep(1000);

// QSignalSpy spy(syncHelper.data(), SIGNAL(syncError(int, QString)));
// syncHelper->sync(db, dbPath);
// int waitTime = 0;

// while (spy.count() == 0 && waitTime < 10000) {
// QTest::qWait(200);
// waitTime += 200;
// }
// const FileInfoList dbList = gdrive->getDatabasesSeq(GoogleDriveTools::getDbNameFilter(
// dbName + ".kdbx"));
// QCOMPARE(dbList.length(), 2);

//// make sure syncError was emitted
// QCOMPARE(spy.count(), 1);
// QList<QVariant> args = spy.takeFirst();
// QCOMPARE(args.count(), 2);
// QCOMPARE(args.at(0).toInt(), static_cast<int>(Errors::SyncError::AMBIGIOUS_DB));

// deleteDb(dbList[0]);
// deleteDb(dbList[1]);
// delete db;
// }

/**
 * @brief TestDatabaseRemoteSync::testRemoteDatabaseSyncNoCloudDb - test sync
 * returns sync done when there is no cloud database exists
 */
void TestDatabaseRemoteSync::testRemoteDatabaseSyncNoCloudDb()
{
  qRegisterMetaType < QSharedPointer < SyncObject > >
    ("QSharedPointer<SyncObject>");
  KeePassxDriveSync::Command syncCommand = remoteDrive->sync();
  syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,
                                                       db.data()).addOption(
                              OPTION_ABSOLUTE_DB_NAME, dbPath).build());
  syncCommand->waitForFinish();
  // make sure no errors happen
  QCOMPARE(syncCommand->getErrorCode(), static_cast<int>(Errors::NO_ERROR));
}

void TestDatabaseRemoteSync::testRemoteDatabaseSyncLoginError()
{
  Q_ASSERT(testUtils->uploadDb(dbPath));
  qDebug() << "Date while uploading: " + db->metadata()->lastModifiedDate().toString();
  Tools::sleep(1000);
  entry->setGroup(db->rootGroup());
  qDebug() << "Date after db modification: " + db->metadata()->lastModifiedDate().toString();
  testUtils->saveDatabase(db.data(), dbPath);
  // changing database password to make sync fail. Since sync is using local db
  // key to open remote db as well
  CompositeKey key;
  key.addKey(PasswordKey("NewPassword"));
  db->setKey(key);

  KeePassxDriveSync::Command syncCommand = remoteDrive->sync();
  syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,
                                                       db.data()).addOption(
                              OPTION_ABSOLUTE_DB_NAME, dbPath).build());
  syncCommand->waitForFinish();
  // make sure syncError was emitted with non zero error code
  QCOMPARE(syncCommand->getErrorCode(),
           static_cast<int>(Errors::SyncError::KEY_PROBLEM));
}

// TODO Do some mocking to test download and sync errors

QTEST_GUILESS_MAIN(TestDatabaseRemoteSync)
