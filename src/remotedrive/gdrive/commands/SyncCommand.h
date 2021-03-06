#ifndef SyncCommand_H
#define SyncCommand_H
#include "core/Database.h"
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include <QDebug>
#include "remotedrive/Errors.h"
#include "remotedrive/DatabaseSyncFactory.h"
#include "remotedrive/DatabaseSync.h"
#include "remotedrive/DatabaseSyncBase.h"
#include "format/KeePass2Reader.h"
#include "keys/CompositeKey.h"
#include "QtCore/QtConcurrentRun"
#include <QSharedPointer>
#include "remotedrive/RemoteCommand.h"
#include "qtdrive/lib/session.h"
#include "remotedrive/SyncObject.h"
#include "remotedrive/gdrive/commands/ListCommand.h"
#include "remotedrive/OptionsBuilder.h"
#include "remotedrive/gdrive/commands/DownloadCommand.h"
#include <remotedrive/AuthCredentials.h>
#include <remotedrive/gdrive/BaseCommand.h>
#include <remotedrive/gdrive/commands/UploadCommand.h>
#include <core/Config.h>
#include <remotedrive/Constants.h>

using namespace KeePassxDriveSync;
using namespace GoogleDrive;

class SyncCommand : public BaseCommand
{
    Q_OBJECT
private:
    void printSyncSymmary(QSharedPointer<SyncObject> syncObject);
    Database *localDb = 0;
    QString localDbPath;
    CompositeKey localDbKey;
    Database *readDatabase(Database *localDb, const QString &remoteDbPath);
    QDateTime remoteDbLastModified;
    void updateRemoteDatabase(QSharedPointer<SyncObject> syncObject, Database &localDb,
                              Database &remoteDb, const QString &localDbPath, RemoteFile remoteFileInfo);


public:
    SyncCommand(AuthCredentials *creds);
    /**
     * @brief SyncCommand::sync - executes entries/groups level sync of currently opened in memory database and db on google drive with a same name
     * @param localDb - pointer to the opened in memory database
     * @param localDbPath - path to local database file on disk.Used to get psycical database name
     */
    void execute(const QVariantMap options);

    ~SyncCommand();
    // returns last modification time of remote db
    // it's a remote db version which is currently synced with local version
    QDateTime getRemoteDbLastModified();
private:
    void emitCustomSuccess(Database* remoteDb,  QSharedPointer<SyncObject> syncObject);
};
#endif // SyncCommand_H
