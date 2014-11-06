#include "SyncRecentDbHelper.h"

SyncRecentDbHelper::SyncRecentDbHelper()
    :gdrive(new GoogleDriveApi())
{

}
void SyncRecentDbHelper::syncParallel(Database* localDb,const QString& localDbPath) {
    QtConcurrent::run(this,&SyncRecentDbHelper::sync,localDb,localDbPath);
}

QSharedPointer<GDriveSyncObject> SyncRecentDbHelper::sync(Database* localDb, const QString& localDbPath) {
Q_ASSERT(localDb);
Q_ASSERT(localDbPath.length()>0);
QString dbName=QFileInfo(localDbPath).fileName();
const FileInfoList dbList=googleDriveApi()->getDatabasesSeq(GoogleDriveTools::getDbNameFilter(dbName));
//no need to sync database because given copy exists only locally.
//insted local copy will be uploaded to the cloud
if (dbList.size()==0) {
    qDebug() << QString("Nothing to sync. There is no remote database with name: %1").arg(QFileInfo(localDbPath).fileName());
    QSharedPointer<GDriveSyncObject> syncObject = QSharedPointer<GDriveSyncObject>(new GDriveSyncObject());
    Q_EMIT syncDone(syncObject);
    return QSharedPointer<GDriveSyncObject>(syncObject);
}

if (dbList.size()>1) {
    emitSyncError(Errors::SyncError::AMBIGIOUS_DB,QString("There are more than one database with name %1. Keepassx requires to have only unique databases in the folder").arg(dbName));
    return QSharedPointer<GDriveSyncObject>();
    }
FileInfo remoteDbInfo = dbList.value(0);
qDebug() << "remote db date = "+QString::number(remoteDbInfo.modifiedDate().toLocalTime().toTime_t());
qDebug() << "local db date  = "+QString::number(localDb->metadata()->lastModifiedDate().toLocalTime().toTime_t());
// compare seconds because milliseconds ommited while saving db file on filesystem
if (remoteDbInfo.modifiedDate().toLocalTime().toTime_t()==localDb->metadata()->lastModifiedDate().toLocalTime().toTime_t()) {
    qDebug() << QString("Databases have the same modification time %1. Will skip sync").arg(localDb->metadata()->lastModifiedDate().toLocalTime().toString());
    QSharedPointer<GDriveSyncObject> syncObject = QSharedPointer<GDriveSyncObject>(new GDriveSyncObject());
    Q_EMIT syncDone(syncObject);
    return QSharedPointer<GDriveSyncObject>(syncObject);
}

const QString remoteDbPath=gdrive->downloadDatabaseSeq(dbList.first(),QDir::tempPath(),QString::number(QDateTime::currentMSecsSinceEpoch())+"-"+dbName);
if (remoteDbPath.length()==0) {
    emitSyncError(Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM,QString("There is a problem to download remote database %1").arg(dbName));
    return QSharedPointer<GDriveSyncObject>();
}
Q_ASSERT(localDbPath!=remoteDbPath);
qDebug() <<"downloaded db to  "+remoteDbPath;
qDebug() <<"start syncronization of ::  ";
qDebug() <<"    remote database:"+remoteDbPath;
qDebug() <<"    local database:"+localDbPath;

Database* remoteDb=readDatabase(localDb,remoteDbPath);
if (remoteDb==Q_NULLPTR)
    return QSharedPointer<GDriveSyncObject>();
QSharedPointer<GDriveDatabaseSyncBase> sync=GDriveDatabaseSyncFactory::createDatabaseSync(GDriveDatabaseSyncFactory::SyncId::ALL,localDb,remoteDb);
QSharedPointer<GDriveSyncObject> syncObject;
syncObject = sync->syncDatabases();
qDebug() << "Sync results::";
qDebug() << "    Local Added missing entries::"+QString::number(syncObject->get(SEntry(),SMissing(),SLocal()));
qDebug() << "    Local Added missing groups::"+QString::number(syncObject->get(SGroup(),SMissing(),SLocal()));
qDebug() << "    Local Updated existing entries::"+QString::number(syncObject->get(SEntry(),SOlder(),SLocal()));
qDebug() << "    Local Updated existing groups::"+QString::number(syncObject->get(SGroup(),SOlder(),SLocal()));
qDebug() << "    Local Removed entries::"+QString::number(syncObject->get(SEntry(),SRemoved(),SLocal()));
qDebug() << "    Local Removed groups::"+QString::number(syncObject->get(SGroup(),SRemoved(),SLocal()));

qDebug() << "    Remote Added missing entries::"+QString::number(syncObject->get(SEntry(),SMissing(),SRemote()));
qDebug() << "    Remote Added missing groups::"+QString::number(syncObject->get(SGroup(),SMissing(),SRemote()));
qDebug() << "    Remote Updated existing entries::"+QString::number(syncObject->get(SEntry(),SOlder(),SRemote()));
qDebug() << "    Remote Updated existing groups::"+QString::number(syncObject->get(SGroup(),SOlder(),SRemote()));
qDebug() << "    Remote Removed entries::"+QString::number(syncObject->get(SEntry(),SRemoved(),SRemote()));
qDebug() << "    Remote Removed groups::"+QString::number(syncObject->get(SGroup(),SRemoved(),SRemote()));
// store time to check it again with remote db during further syncs
// it will help to run sync when both local and remote database were modified simultaneously and remote database has older timestamp
remoteDbLastModified = remoteDbInfo.modifiedDate().toLocalTime();
Q_EMIT syncDone(syncObject);
delete remoteDb;


return syncObject;
}
/**
 * @brief SyncRecentDbHelper::readDatabase reads stored locally remote database into the memory
 * @param localDb pointer to local database to get key from it
 * @param remoteDbPath path to stored locally remote database
 * @return pointer to remote database in memory
 */
Database* SyncRecentDbHelper::readDatabase(Database* localDb,const QString& remoteDbPath) {
    KeePass2Reader reader;
    Database* remoteDb=0;
    QFile file(remoteDbPath);
    if (!file.open(QIODevice::ReadOnly)) {
        emitSyncError(Errors::FileError::OPEN_FILE_PROBLEM,QString("There is a problem to open file of downloaded remote database %1").arg(remoteDbPath));
        return Q_NULLPTR;
    }
    remoteDb = reader.readDatabase(remoteDbPath,localDb->getKey());
    if (remoteDb==Q_NULLPTR) {
     emitSyncError(Errors::SyncError::KEY_PROBLEM,QString("There is a problem to login into downloaded remote database %1").arg(remoteDbPath));
     return Q_NULLPTR;
    }
    return remoteDb;

}

void SyncRecentDbHelper::emitSyncError(int errorType,const QString& description) {
    Q_EMIT syncError(errorType,description);
}

void SyncRecentDbHelper::emitSyncDone(const QSharedPointer<GDriveSyncObject>& syncObject) {
    Q_EMIT syncDone(syncObject);
}

SyncRecentDbHelper::~SyncRecentDbHelper()
{
}

QSharedPointer<SyncRecentDbHelper> SyncRecentDbHelper::newInstance() {
    return QSharedPointer<SyncRecentDbHelper>(new SyncRecentDbHelper);
}
