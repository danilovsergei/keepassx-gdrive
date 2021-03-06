#include "GDriveTestUtils.h"

Q_DECLARE_METATYPE(QList<QueryEntry>)
Q_DECLARE_METATYPE(RemoteFile)
Q_DECLARE_METATYPE(RemoteFileList)
Q_DECLARE_METATYPE(QSharedPointer<DatabaseSyncObject::SyncObject>)
Q_DECLARE_METATYPE(Database *)

GDriveTestUtils::GDriveTestUtils(RemoteDriveApi *remoteDrive) :
  remoteDrive(remoteDrive)
{
}

GDriveTestUtils::~GDriveTestUtils()
{
  qDebug() << "delete testUtils";
}

Entry *GDriveTestUtils::createEntry(const QString &title, const QString &password)
{
  Entry *entry = new Entry;

  entry->setTitle(title);
  entry->setPassword(password);
  entry->setUuid(Uuid::random());
  return entry;
}

Entry *GDriveTestUtils::createEntry(Database *db, const QString &title, const QString &password)
{
  Entry *entry = createEntry(title, password);
  entry->setGroup(db->resolveGroup(db->rootGroup()->uuid()));
  return entry;
}

Group *GDriveTestUtils::createGroup(const QString &groupName)
{
  Group *newGroup = new Group;

  newGroup->setName(groupName);
  newGroup->setUuid(Uuid::random());
  return newGroup;
}

Group *GDriveTestUtils::createGroup(Database *db, const QString &groupName)
{
  Group *newGroup = createGroup(groupName);
  newGroup->setParent(db->resolveGroup(db->rootGroup()->uuid()));
  return newGroup;
}

bool GDriveTestUtils::uploadDb(const QString &dbPath)
{
  // List all databases with name test.kdbx at google drive
  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QString dbName = QFileInfo(dbPath).fileName();
  const QList<QueryEntry> filter = GoogleDriveTools::getDbNameFilter(dbName);
  KeePassxDriveSync::Command listCommand = remoteDrive->list();
  QVariantMap options = OptionsBuilder().addOption(OPTION_DB_FILTER, filter).build();
  listCommand->executeAsync(options);
  listCommand->waitForFinish();
  RemoteFileList fileList = listCommand->getResult().at(0).value<RemoteFileList>();
  int expected = fileList.size() + 1;

  // upload new database or update existing one with revision if it exists
  QFileInfo file(dbPath);
  KeePassxDriveSync::Command uploadCommand = remoteDrive->upload();
  uploadCommand->executeAsync(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                         dbPath).addOption(
                                OPTION_LAST_MODIFIED_TIME,
                                file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                               parentDir).build());
  uploadCommand->waitForFinish();

  // verify db was loaded successfully. Using >0 because in some cases i will
  // expect 2 databases
  listCommand->executeAsync(options);
  listCommand->waitForFinish();
  fileList = listCommand->getResult().at(0).value<RemoteFileList>();
  int actual = fileList.size();
  qDebug() << QString("actual = %1 , expected = %2").arg(actual).arg(expected);
  Q_ASSERT(actual == expected);
  return actual == expected ? true : false;
}

RemoteFile GDriveTestUtils::uploadDbWithResult(const QString &dbPath)
{
  // get all databases with the same name and expect +1 to this number
  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  QString dbName = QFileInfo(dbPath).fileName();
  const QList<QueryEntry> filter = GoogleDriveTools::getDbNameFilter(dbName);
  KeePassxDriveSync::Command listCommand = remoteDrive->list();
  QVariantMap options = OptionsBuilder().addOption(OPTION_DB_FILTER, filter).build();
  listCommand->executeAsync(options);
  listCommand->waitForFinish();
  RemoteFileList fileList = listCommand->getResult().at(0).value<RemoteFileList>();
  int expected = fileList.size() + 1;

  // upload new database or update existing one with revision if it exists
  QFileInfo file(dbPath);
  KeePassxDriveSync::Command uploadCommand = remoteDrive->upload();
  uploadCommand->executeAsync(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                         dbPath).addOption(
                                OPTION_LAST_MODIFIED_TIME,
                                file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                               parentDir).build());
  uploadCommand->waitForFinish();
  RemoteFile remoteDb = uploadCommand->getResult().at(0).value<RemoteFile>();
  // expect remote db will have some id. Otherwise something went wrong with upload api
  return remoteDb;
}

QSharedPointer<SyncObject> GDriveTestUtils::syncDatabase(Database *db, const QString &dbPath)
{
  KeePassxDriveSync::Command syncCommand = remoteDrive->sync();
  syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,
                                                       db).addOption(OPTION_ABSOLUTE_DB_NAME,
                                                                     dbPath).build());
  syncCommand->waitForFinish();
  Q_ASSERT(syncCommand->getErrorCode() == Errors::NO_ERROR);
  Q_ASSERT(syncCommand->getResult().length() > 0);
  typedef QSharedPointer < DatabaseSyncObject::SyncObject > SyncType;
  qRegisterMetaType < QSharedPointer < DatabaseSyncObject::SyncObject > >(
    "QSharedPointer<SyncObject>");
  qRegisterMetaType<SyncType>("SyncType");
  QVariant vSyncObject = syncCommand->getResult().at(0);
  QSharedPointer<SyncObject> syncObject = vSyncObject.value<SyncType>();

  return syncObject;
}

QString GDriveTestUtils::correctDbName(const QString &dbName)
{
  QString title = dbName;
  if (!title.endsWith(".kdbx"))
    title.append(".kdbx");
  return title;
}

bool GDriveTestUtils::deleteAllDb(const QString &dbTitle)
{
  KeePassxDriveSync::Command listCommand = remoteDrive->list();
  QVariantMap options = OptionsBuilder().addOption(OPTION_DB_FILTER, GoogleDriveTools::getDbNameFilter(
                                                     correctDbName(
                                                       dbTitle))).build();
  listCommand->executeAsync(options);
  listCommand->waitForFinish();
  const RemoteFileList dbList = listCommand->getResult().at(0).value<RemoteFileList>();
  bool result = true;
  Q_FOREACH(RemoteFile dbRec, dbList) {
    deleteDb(dbRec) == false ? result = false : true;
    qDebug() << "remove remote database";
  }
  Q_ASSERT(result);
  return result;
}

/**
 * @brief TestGoogleDriveApi::deleteDb - Internal test function used to delete
 * database from cloud. Not needed yet by Keepassx application
 * @param db
 */
bool GDriveTestUtils::deleteDb(const RemoteFile &dbInfo)
{
  KeePassxDriveSync::Command deleteCommand = remoteDrive->remove();
  QVariantMap options = OptionsBuilder().addOption(OPTION_REMOTE_FILE, dbInfo).build();
  deleteCommand->executeAsync(options);
  deleteCommand->waitForFinish();
  bool result = deleteCommand->getErrorCode() == Errors::NO_ERROR ? true : false;
  return result;
}

bool GDriveTestUtils::deleteLocalDb(const QString &dbPath)
{
  return QFile(dbPath).exists() && QFile::remove(dbPath);
}

bool GDriveTestUtils::saveDatabase(Database *db, const QString &dbPath)
{
  // save fake database locally
  KeePass2Writer m_writer;
  QString fileName(dbPath);

  qDebug() << "saving to " + fileName;
  QFile qFile(fileName);

  if (qFile.exists())
    qFile.remove();
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

const CompositeKey GDriveTestUtils::getTestCompositeKey()
{
  CompositeKey key;

  key.addKey(PasswordKey("test"));
  return key;
}

Database *GDriveTestUtils::createLocalDatabase(const QString &dbPath)
{
  Database *db1 = new Database();

  db1->setKey(getTestCompositeKey());
  QString dbName = QFileInfo(dbPath).fileName();
  db1->metadata()->setName(dbName);

  // save fake database locally
  KeePass2Writer m_writer;
  QString fileName(dbPath);
  qDebug() << "saving to " + fileName;
  QFile qFile(fileName);

  if (qFile.exists())
    qFile.remove();
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

/**
 * @brief GDriveTestUtils::compareResult compares actual and expected
 * results for  database sync
 * @param actual what actual sync provided
 * @param expectedMap what is expected
 */
bool GDriveTestUtils::compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey,
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
  Q_ASSERT(0 == result.size());
  return 0 == result.size();
}

/**
 * @brief SyncRecentDbHelper::readDatabase reads stored locally database into
 * the memory
 */
Database *GDriveTestUtils::readDatabase(const CompositeKey &key, const QString &dbPath)
{
  KeePass2Reader reader;
  Database *db = Q_NULLPTR;
  QFile file(dbPath);

  Q_ASSERT(file.open(QIODevice::ReadOnly));
  db = reader.readDatabase(dbPath, key);
  Q_ASSERT(db != Q_NULLPTR);
  return db;
}

void GDriveTestUtils::waitForSignal(const QSignalSpy &spy, const int timeout)
{
  int waitTime = 0;
  while (spy.count() == 0 && waitTime < timeout) {
    QTest::qWait(200);
    waitTime += 200;
  }
}

QString GDriveTestUtils::generateTempDbPath(const QString &pattern)
{
  return QDir::tempPath() + QDir::separator()
         +QString::number(QDateTime::currentMSecsSinceEpoch()) +"_"+pattern+ ".kdbx";
}
