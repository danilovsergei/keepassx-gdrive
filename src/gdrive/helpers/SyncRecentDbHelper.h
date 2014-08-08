#ifndef SYNCRECENTDBHELPER_H
#define SYNCRECENTDBHELPER_H
#include "core/Database.h"
#include "gdrive/GoogleDriveApi.h"
#include "gdrive/GoogleDriveTools.h"
#include <QtCore/QDebug>
#include "gdrive/Errors.h"
#include "gdrive/GDriveDatabaseSyncFactory.h"
#include "gdrive/GDriveDatabaseSync.h"
#include "format/KeePass2Reader.h"
#include "keys/CompositeKey.h"
#include "QtCore/QtConcurrentRun"
#include <QtCore/QSharedPointer>
class SyncRecentDbHelper : public QObject
{
Q_OBJECT
Q_DISABLE_COPY(SyncRecentDbHelper)
private:

    static QSharedPointer<SyncRecentDbHelper> g_instance;
    void emitSyncError(int errorType,const QString& description);
    void emitSyncDone();
    Database* localDb=0;
    GoogleDriveApi*   gdrive=0;
    QString localDbPath;
    CompositeKey localDbKey;
    Database* readDatabase(Database* localDb,const QString& remoteDbPath);


public:
    SyncRecentDbHelper();
    static QSharedPointer<SyncRecentDbHelper> instance();
    void sync(Database* localDb, const QString& localDbPath);
    void syncParallel(Database* localDb, const QString& localDbPath);
    ~SyncRecentDbHelper();
Q_SIGNALS:
    void syncDone();
    void syncError(int ErrorType,QString description);



};

inline QSharedPointer<SyncRecentDbHelper> syncRecentDbHelper() {
return SyncRecentDbHelper::instance();
}

#endif // SYNCRECENTDBHELPER_H
