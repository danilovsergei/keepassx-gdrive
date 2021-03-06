#include "SyncCommand.h"
#include <core/Tools.h>

Q_DECLARE_METATYPE(RemoteFileList)
Q_DECLARE_METATYPE(QSharedPointer<SyncObject>)
Q_DECLARE_METATYPE(Database *)
Q_DECLARE_METATYPE(QList<QueryEntry>)
Q_DECLARE_METATYPE(RemoteFile)
SyncCommand::SyncCommand(AuthCredentials *creds) :
  BaseCommand(creds)
{
}

void SyncCommand::execute(const QVariantMap options)
{
  Q_ASSERT(options.size() >= 1);
  Database *localDb = parseOption<Database *>(options, OPTION_DB_POINTER);
  const QString localDbPath = parseOption<QString>(options, OPTION_ABSOLUTE_DB_NAME);
  const QString dbName = QFileInfo(localDbPath).fileName();

  KeePassxDriveSync::Command listCommand = KeePassxDriveSync::Command(new ListCommand(creds));
  listCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_FILTER,
                                                       GoogleDriveTools::getDbNameFilter(
                                                         dbName)).build());
  listCommand->waitForFinish();
  if (checkForError(listCommand)) return;
  RemoteFileList dbList = listCommand->getResult().at(0).value<RemoteFileList>();

  // no need to sync database because given copy exists only locally.
  // insted local copy will be uploaded to the cloud
  if (dbList.size() == 0) {
    qDebug() << QString("Nothing to sync. There is no remote database with name: %1").arg(QFileInfo(
                                                                                            localDbPath).fileName());
   QSharedPointer<SyncObject> emptySyncObject = QSharedPointer<SyncObject>(new SyncObject());
   emptySyncObject->setLocalModificationDate(localDb->metadata()->lastModifiedDate());
   // will use empty remote database last modification time to indicate that remote database absent
   emptySyncObject->setRemoteModificationDate(QDateTime());
   emitCustomSuccess(Q_NULLPTR,  emptySyncObject);
   return;
  }

  if (dbList.size() > 1) {
    emitError(Errors::SyncError::AMBIGIOUS_DB,
              QString(
                "There are more than one database with name %1. "
                "Keepassx requires to have only unique databases in the folder").arg(
                dbName));
    return;
  }
  RemoteFile remoteDbInfo = dbList.value(0);
  qDebug() << "remote db date = "+
    remoteDbInfo.getModifiedDate().toLocalTime().toString();
  qDebug() << "local db date  = "+
    localDb->metadata()->lastModifiedDate().toLocalTime().toString();
  // compare seconds because milliseconds ommited while saving db file on filesystem
  if (remoteDbInfo.getModifiedDate().toLocalTime().toTime_t()
      == localDb->metadata()->lastModifiedDate().toLocalTime().toTime_t()) {
    qDebug() << QString("Databases have the same modification time %1. Will skip sync").arg(
      localDb->metadata()->lastModifiedDate().toLocalTime().toString());

    QSharedPointer<SyncObject> emptySyncObject = QSharedPointer<SyncObject>(new SyncObject());
    emptySyncObject->setLocalModificationDate(localDb->metadata()->lastModifiedDate());
    emptySyncObject->setRemoteModificationDate(remoteDbInfo.getModifiedDate());
    emitCustomSuccess(Q_NULLPTR,  emptySyncObject);
    return;
  }
  KeePassxDriveSync::Command downloadCommand
    = KeePassxDriveSync::Command(new DownloadCommand(creds));
  // download remote database locally for further sync
  downloadCommand->executeAsync(OptionsBuilder().addOption(OPTION_FI,
                                                           dbList.first()).addOption(OPTION_DB_DIR,
                                                                                     QDir::
                                                                                     tempPath()).
                                addOption(OPTION_DB_NAME,
                                          QString::number(
                                            QDateTime::currentMSecsSinceEpoch())+"-"
                                          +dbName).build());
  downloadCommand->waitForFinish();
  if (checkForError(downloadCommand)) return;

  const QString remoteDbPath = downloadCommand->getResult().at(0).toString();

  if (remoteDbPath.length() == 0) {
    emitError(Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM,
              QString("There is a problem to download remote database %1").arg(dbName));
    return;
  }

  Q_ASSERT(localDbPath != remoteDbPath);
  qDebug() <<"downloaded db to  "+remoteDbPath;
  qDebug() <<"start syncronization of ::  ";
  qDebug() <<"    remote database:"+remoteDbPath;
  qDebug() <<"    local database:"+localDbPath;

  Database *remoteDb = readDatabase(localDb, remoteDbPath);
  if (remoteDb == Q_NULLPTR) {
    emitError(Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM, QString(
                "Unable to read database %1").arg(remoteDbPath));
    return;
  }
  QSharedPointer<DatabaseSyncBase> sync = DatabaseSyncFactory::createDatabaseSync(
    DatabaseSyncFactory::SyncId::ALL, localDb, remoteDb);
  QSharedPointer<SyncObject> syncObject = sync->syncDatabases();

  printSyncSymmary(syncObject);

  emitCustomSuccess(remoteDb, syncObject);
}

/**
 * @brief SyncCommand::emitCustomSuccess emits sucess with provided sync object
 *  and cleans downloaded remote database.
 * @param remoteDb downloaded remote database
 * @param syncObject syncobject with sync data to pass
 * @param remoteDbLastModified time when remote db was last modfied
 */
void SyncCommand::emitCustomSuccess(Database * remoteDb, QSharedPointer<SyncObject> syncObject)
{
  setResult(KeePassxDriveSync::ResultBuilder().addValue(syncObject).build());
  if (remoteDb != Q_NULLPTR) {
    delete remoteDb;
  }
  emitSuccess();
}

void SyncCommand::updateRemoteDatabase(QSharedPointer<SyncObject> syncObject, Database &localDb,
                                       Database &remoteDb, const QString &localDbPath,
                                       RemoteFile remoteFileInfo)
{
  int remoteChanges = syncObject->get(SEntry(), SMissing(), SRemote()) +syncObject->get(
    SGroup(), SMissing(), SRemote())
                      +syncObject->get(SEntry(), SOlder(), SRemote()) + syncObject->get(
    SGroup(), SOlder(), SRemote())
                      +syncObject->get(SEntry(), SRemoved(), SRemote()) + syncObject->get(
    SGroup(), SRemoved(), SRemote());
  bool remoteOlder = remoteDb.metadata()->lastModifiedDate()
                     < localDb.metadata()->lastModifiedDate();
  QDateTime lastModifiedDate
    = remoteOlder ? localDb.metadata()->lastModifiedDate() : remoteDb.metadata()->lastModifiedDate();
  QString parentDir = config()->get(RemoteDrive::CONFIG_KEEPASS_REMOTE_FOLDER).toString();
  // Upload db because sync has new changes for remote database or we need update db modification date
  if (remoteChanges > 0 || remoteOlder) {
    KeePassxDriveSync::Command uploadCommand = KeePassxDriveSync::Command(new UploadCommand(creds));

    uploadCommand->executeAsync(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                           localDbPath).addOption(
                                  OPTION_LAST_MODIFIED_TIME,
                                  lastModifiedDate).addOption(
                                  OPTION_PARENT_NAME,
                                  parentDir).build());
  }
}

void SyncCommand::printSyncSymmary(QSharedPointer<SyncObject> syncObject)
{
  qDebug() << "Sync results::";
  qDebug() << "    Local Added missing entries::"
    +QString::number(syncObject->get(SEntry(), SMissing(), SLocal()));
  qDebug() << "    Local Added missing groups::"
    +QString::number(syncObject->get(SGroup(), SMissing(), SLocal()));
  qDebug() << "    Local Updated existing entries::"
    +QString::number(syncObject->get(SEntry(), SOlder(), SLocal()));
  qDebug() << "    Local Updated existing groups::"
    +QString::number(syncObject->get(SGroup(), SOlder(), SLocal()));
  qDebug() << "    Local Removed entries::"
    +QString::number(syncObject->get(SEntry(), SRemoved(), SLocal()));
  qDebug() << "    Local Removed groups::"
    +QString::number(syncObject->get(SGroup(), SRemoved(), SLocal()));

  qDebug() << "    Remote Added missing entries::"
    +QString::number(syncObject->get(SEntry(), SMissing(), SRemote()));
  qDebug() << "    Remote Added missing groups::"
    +QString::number(syncObject->get(SGroup(), SMissing(), SRemote()));
  qDebug() << "    Remote Updated existing entries::"
    +QString::number(syncObject->get(SEntry(), SOlder(), SRemote()));
  qDebug() << "    Remote Updated existing groups::"
    +QString::number(syncObject->get(SGroup(), SOlder(), SRemote()));
  qDebug() << "    Remote Removed entries::"
    +QString::number(syncObject->get(SEntry(), SRemoved(), SRemote()));
  qDebug() << "    Remote Removed groups::"
    +QString::number(syncObject->get(SGroup(), SRemoved(), SRemote()));
}

/**
 * @brief SyncCommand::readDatabase reads stored locally remote database into the memory
 * @param localDb pointer to local database to get key from it
 * @param remoteDbPath path to stored locally remote database
 * @return pointer to remote database in memory
 */
Database *SyncCommand::readDatabase(Database *localDb, const QString &remoteDbPath)
{
  KeePass2Reader reader;
  Database *remoteDb = 0;
  QFile file(remoteDbPath);
  if (!file.open(QIODevice::ReadOnly)) {
    emitError(Errors::FileError::OPEN_FILE_PROBLEM,
              QString(
                "There is a problem to open file of downloaded remote database %1").arg(
                remoteDbPath));
    return Q_NULLPTR;
  }
  remoteDb = reader.readDatabase(remoteDbPath, localDb->getKey());
  if (remoteDb == Q_NULLPTR) {
    emitError(Errors::SyncError::KEY_PROBLEM,
              QString("There is a problem to login into downloaded remote database %1").arg(
                remoteDbPath));
    return Q_NULLPTR;
  }
  return remoteDb;
}

SyncCommand::~SyncCommand()
{
}
