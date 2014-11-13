#include "GDriveTestUtils.h"

GDriveTestUtils::GDriveTestUtils()
{}

Entry * GDriveTestUtils::createEntry(const QString& title,
                                     const QString& password) {
  Entry *entry = new Entry;

  entry->setTitle(title);
  entry->setPassword(password);
  entry->setUuid(Uuid::random());
  return entry;
}

Group * GDriveTestUtils::createGroup(const QString& groupName) {
  Group *newGroup = new Group;

  newGroup->setName(groupName);
  newGroup->setUuid(Uuid::random());
  return newGroup;
}

bool GDriveTestUtils::uploadDb(const QString& dbPath) {
  // List all databases with name test.kdbx at google drive
  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QString dbName = QFileInfo(dbPath).fileName();
  const QList<QueryEntry> filter = GoogleDriveTools::getDbNameFilter(dbName);
  int expected = gdrive->getDatabasesSeq(filter).size() + 1;

  // QVERIFY2(gdrive->getDatabasesSeq(filter).size()==0,"Test db exists in
  // google drive before test, but it should not");

  // upload new database or update existing one with revision if it exists
  QFileInfo file(dbPath);
  gdrive->uploadDatabase(dbPath, file.lastModified(), true, parentDir);

  // verify db was loaded successfully. Using >0 because in some cases i will
  // expect 2 databases
  int actual = gdrive->getDatabasesSeq(filter).size();
  Q_ASSERT(actual == expected);
  return actual == expected ? true : false;
}

bool GDriveTestUtils::deleteAllDb(const QString& dbTitle) {
  const FileInfoList dbList = gdrive->getDatabasesSeq(GoogleDriveTools::getDbNameFilter(
                                                        dbTitle));
  bool result=true;
  Q_FOREACH(FileInfo dbRec, dbList) {
    deleteDb(dbRec)==false? result=false: true;
  }
   Q_ASSERT(result);
  return result;
}

/**
 * @brief TestGoogleDriveApi::deleteDb - Internal test function used to delete
 * database from cloud. Not needed yet by Keepassx application
 * @param db
 */
bool GDriveTestUtils::deleteDb(const FileInfo& db) {
  CommandDelete *deleteCmd = new CommandDelete(getSession());

  deleteCmd->exec(db.id());

  if (!deleteCmd->waitForFinish(false)) {
    return false;
  }

  const FileInfoList dbList = gdrive->getDatabasesSeq(GoogleDriveTools::getDbNameFilter(
                                                        db.title()));
  Q_FOREACH(FileInfo dbRec, dbList) {
    Q_ASSERT(dbRec.id() != db.id());

    if (dbRec.id() != db.id()) {
      qDebug() << "Db still exists in Google Drive";
      return false;
    }
  }
  return true;
}

bool GDriveTestUtils::deleteLocalDb(const QString& dbPath) {
return QFile(dbPath).exists()&& QFile::remove(dbPath);
}

bool GDriveTestUtils::saveDatabase(Database *db, const QString& dbPath) {
  // save fake database locally
  KeePass2Writer m_writer;
  QString fileName(dbPath);

      qDebug() << "saving to " + fileName;
  QFile qFile(fileName);

  if (qFile.exists()) {
    qFile.remove();
  }
  Q_ASSERT(qFile.exists() == false);
  QSaveFile saveFile(fileName);
  bool result = false;

  if (saveFile.open(QIODevice::WriteOnly)) {
    m_writer.writeDatabase(&saveFile, db);
    result = saveFile.commit();
  }

  // check database saved successfully
  return result && qFile.exists();
}

const CompositeKey GDriveTestUtils::getTestCompositeKey() {
  CompositeKey key;

  key.addKey(PasswordKey("test"));
  return key;
}

Database * GDriveTestUtils::createLocalDatabase(const QString& dbPath) {
  Database *db1 = new Database();

  db1->setKey(getTestCompositeKey());
  QString dbName = QFileInfo(dbPath).fileName();
  db1->metadata()->setName(dbName);

  // save fake database locally
  KeePass2Writer m_writer;
  QString fileName(dbPath);
  qDebug() << "saving to " + fileName;
  QFile qFile(fileName);

  if (qFile.exists()) {
    qFile.remove();
  }
  Q_ASSERT(qFile.exists() == false);
  QSaveFile saveFile(fileName);
  bool result = false;

  if (saveFile.open(QIODevice::WriteOnly)) {
    m_writer.writeDatabase(&saveFile, db1);
    result = saveFile.commit();
  }

  // check database saved successfully
  Q_ASSERT(result == true);
  Q_ASSERT(qFile.exists() == true);
  return db1;
}

Entry * GDriveTestUtils::createEntry(Database      *db,
                                     const QString& title,
                                     const QString& password) {
  Entry *entry = new Entry();

  entry->setGroup(db->resolveGroup(db->rootGroup()->uuid()));
  entry->setTitle(title);
  entry->setPassword(password);
  entry->setUuid(Uuid::random());
  return entry;
}

Group * GDriveTestUtils::createGroup(Database      *db,
                                     const QString& groupName) {
  Group *newGroup = new Group;

  newGroup->setName(groupName);
  newGroup->setUuid(Uuid::random());
  newGroup->setParent(db->resolveGroup(db->rootGroup()->uuid()));
  return newGroup;
}

/**
 * @brief GDriveTestUtils::compareResult compares actual and expected
 * results for  database sync
 * @param actual what actual sync provided
 * @param expectedMap what is expected
 */
bool GDriveTestUtils::compareResult(QSharedPointer<GDriveSyncObject>actual,
                                    QMap<SyncMapKey, int>expectedMap) {
  QSharedPointer<GDriveSyncObject>expected = QSharedPointer<GDriveSyncObject>(
    new GDriveSyncObject());
  Q_FOREACH(SyncMapKey key, expectedMap.keys()) {
    expected->set(static_cast<ObjectType>(key.get(0)),
                  static_cast<ObjectName>(key.get(
                                            1)),
                  static_cast<ObjectLocation>(key.get(
                                                2)),
                  expectedMap.value(key));
  }
  QMap<SyncMapKey, QPair<int, int> >result = actual->compare(expected);
  Q_FOREACH(SyncMapKey key, result.keys()) {
    qDebug() <<
      QString("%1 : %2 <> %3").arg(key.toString(),
                                   QString::number(result.value(key).first),
                                   QString::number(result.value(key).second));
  }
  Q_ASSERT(0 == result.size());
  return 0 == result.size();
}

/**
 * @brief SyncRecentDbHelper::readDatabase reads stored locally database into
 * the memory
 */
Database * GDriveTestUtils::readDatabase(const CompositeKey& key,
                                         const QString     & dbPath) {
  KeePass2Reader reader;
  Database *db = Q_NULLPTR;
  QFile     file(dbPath);

  Q_ASSERT(file.open(QIODevice::ReadOnly));
  db = reader.readDatabase(dbPath, key);
  Q_ASSERT(db != Q_NULLPTR);
  return db;
}

void GDriveTestUtils::waitForSignal(const QSignalSpy& spy, const int timeout) {
    int waitTime = 0;
    while (spy.count() == 0 && waitTime < timeout) {
      QTest::qWait(200);
      waitTime += 200;
    }
}
