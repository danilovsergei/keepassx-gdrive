#include "SyncRecentDbHelper.h"

//its not enough to define static variable only in the header. To know its object to allocate memory it should be defined in the class as well
QSharedPointer<SyncRecentDbHelper> SyncRecentDbHelper::g_instance;

SyncRecentDbHelper::SyncRecentDbHelper() {

}


void SyncRecentDbHelper::syncParallel(Database* localDb,const QString& localDbPath) {
    QtConcurrent::run(this,&SyncRecentDbHelper::sync,localDb,localDbPath);
}

/**
 * @brief SyncRecentDbHelper::sync - executes entries level sync of local abd google drive database
 * @param localDb - pointer to local database
 * @param localDbPath - path to local database
 */
void SyncRecentDbHelper::sync(Database* localDb, const QString& localDbPath) {
Q_ASSERT(localDb);
Q_ASSERT(localDbPath.length()>0);
QString dbName=QFileInfo(localDbPath).fileName();
const FileInfoList dbList=googleDriveApi()->getDatabasesSeq(GoogleDriveTools::getDbNameFilter(dbName));
//no need to sync database because given copy exists only locally.
//insted local copy will be uploaded to the cloud
if (dbList.size()==0) {
    qDebug() << QString("Nothing to sync. There is no remote database with name: %1").arg(QFileInfo(localDbPath).fileName());
    Q_EMIT syncDone();
    return;
}

if (dbList.size()>1) {
    emitSyncError(Errors::SyncError::AMBIGIOUS_DB,QString("There are more than one database with name %1. Keepassx requires to have only unique databases in the folder").arg(dbName));
    return;
    }
const QString remoteDbPath=gdrive->downloadDatabaseSeq(dbList.first(),QDir::tempPath(),QString::number(QDateTime::currentMSecsSinceEpoch())+"-"+dbName);
if (remoteDbPath.length()==0) {
    emitSyncError(Errors::DownloadError::COMMON_DOWNLOAD_PROBLEM,QString("There is a problem to download remote database %1").arg(dbName));
    return;
}
Q_ASSERT(localDbPath!=remoteDbPath);
qDebug() << "we are here!!!";
qDebug() <<"downloaded db to  "+remoteDbPath;
qDebug() <<"start syncronization of ::  ";
qDebug() <<"    remote database:"+remoteDbPath;
qDebug() <<"    local database:"+localDbPath;

Database* remoteDb=readDatabase(localDb,remoteDbPath);
if (remoteDb==Q_NULLPTR)
    return;
QSharedPointer<GDriveDatabaseSync> sync=GDriveDatabaseSyncFactory::createDatabaseSync(GDriveDatabaseSyncFactory::SyncId::ALL,localDb,remoteDb);
sync->syncDatabases();
qDebug() << "Sync results::";
qDebug() << "    Local Added missing entries::"+QString::number(sync->getLocalMissingEntries());
qDebug() << "    Local Added missing groups::"+QString::number(sync->getLocalMissingGroups());
qDebug() << "    Local Updated existing entries::"+QString::number(sync->getLocalOlderEntries());
qDebug() << "    Local Updated existing groups::"+QString::number(sync->getLocalOlderGroups());
qDebug() << "    Local Removed entries::"+QString::number(sync->getLocalRemovedEntries());
qDebug() << "    Local Removed groups::"+QString::number(sync->getLocalRemovedGroups());

qDebug() << "    Remote Added missing entries::"+QString::number(sync->getRemoteMissingEntries());
qDebug() << "    Remote Added missing groups::"+QString::number(sync->getRemoteMissingGroups());
qDebug() << "    Remote Updated existing entries::"+QString::number(sync->getRemoteOlderEntries());
qDebug() << "    Remote Updated existing groups::"+QString::number(sync->getRemoteOlderGroups());
qDebug() << "    Remote Removed entries::"+QString::number(sync->getRemoteRemovedEntries());
qDebug() << "    Remote Removed groups::"+QString::number(sync->getRemoteRemovedGroups());
Q_EMIT syncDone();

delete remoteDb;
}

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
    qDebug() <<description;
}

void SyncRecentDbHelper::emitSyncDone() {
    Q_EMIT syncDone();
}

SyncRecentDbHelper::~SyncRecentDbHelper()
{

}

QSharedPointer<SyncRecentDbHelper> SyncRecentDbHelper::instance() {
    //qRegisterMetaType<SyncRecentDbHelper>("SyncRecentDbHelper");
    if (g_instance.isNull())
        g_instance=QSharedPointer<SyncRecentDbHelper>(new SyncRecentDbHelper());
    return g_instance;
}

