#ifndef SYNCRECENTDBHELPER_H
#define SYNCRECENTDBHELPER_H
#include "core/Database.h"
#include "gdrive/GoogleDriveApi.h"
#include "gdrive/GoogleDriveTools.h"
#include <QtCore/QDebug>
#include "gdrive/Errors.h"
#include "gdrive/GDriveDatabaseSyncFactory.h"
#include "gdrive/GDriveDatabaseSync.h"
#include "gdrive/GDriveDatabaseSyncBase.h"
#include "format/KeePass2Reader.h"
#include "keys/CompositeKey.h"
#include "QtCore/QtConcurrentRun"
#include <QtCore/QSharedPointer>
class SyncRecentDbHelper : public QObject
{
Q_OBJECT
Q_DISABLE_COPY(SyncRecentDbHelper)
private:
    void emitSyncError(int errorType,const QString& description);
    void emitSyncDone(const QSharedPointer<GDriveSyncObject>& syncObject);
    Database* localDb=0;
    QString localDbPath;
    CompositeKey localDbKey;
    Database* readDatabase(Database* localDb,const QString& remoteDbPath);
    QScopedPointer<GoogleDriveApi> gdrive;
    QDateTime remoteDbLastModified;


public:
    SyncRecentDbHelper();
    static QSharedPointer<SyncRecentDbHelper> newInstance();
    /**
     * @brief SyncRecentDbHelper::sync - executes entries/groups level sync of currently opened in memory database and db on google drive with a same name
     * @param localDb - pointer to the opened in memory database
     * @param localDbPath - path to local database file on disk.Used to get psycical database name
     */
    QSharedPointer<GDriveSyncObject> sync(Database* localDb, const QString& localDbPath);
    /** Same as sync but performed asynchronously in different thead.
     *  Control flow managed by signals syncDone and syncError
     */
    void syncParallel(Database* localDb, const QString& localDbPath);
    ~SyncRecentDbHelper();
    // returns last modification time of remote db
    // it's a remote db version which is currently synced with local version
    QDateTime getRemoteDbLastModified();

Q_SIGNALS:
    void syncDone(QSharedPointer<GDriveSyncObject>);
    void syncError(int ErrorType,QString description);

};
#endif // SYNCRECENTDBHELPER_H
