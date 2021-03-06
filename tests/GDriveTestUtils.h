#ifndef GDRIVETESTUTILS_H
#define GDRIVETESTUTILS_H

#include <QSharedPointer>
#include "remotedrive/SyncObject.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include <QDebug>
#include <QDir>
#include <keys/PasswordKey.h>
#include <format/KeePass2Writer.h>
#include <format/KeePass2Reader.h>
#include <core/qsavefile.h>
#include <qmath.h>
#include <QSignalSpy>
#include <QTest>
#include <QObject>

#include "remotedrive/CommandsFactory.h"
#include "remotedrive/AuthCredentials.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/RemoteCommand.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"


using namespace DatabaseSyncObject;
using namespace GoogleDrive;
using KeePassxDriveSync::Command;

class GDriveTestUtils : public QObject
{
Q_OBJECT

public:
    GDriveTestUtils(RemoteDriveApi* remoteDrive);
    ~GDriveTestUtils();
    bool compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database *readDatabase(const CompositeKey &key, const QString &dbPath);
    Database *createLocalDatabase(const QString &dbPath);
    bool uploadDb(const QString &dbPath);
    RemoteFile uploadDbWithResult(const QString &dbPath);
    bool deleteDb(const RemoteFile &dbInfo);
    bool deleteAllDb(const QString &dbTitle);
    const CompositeKey getTestCompositeKey();
    bool saveDatabase(Database *db, const QString &dbPath);
    Entry *createEntry(const QString &title = QString("testTitle"),
                       const QString &password = QString("testPassword"));
    Entry *createEntry(Database *db, const QString &title = QString("testTitle"),
                       const QString &password = QString("testPassword"));
    Group *createGroup(const QString &groupName = QString("testGroup"));
    Group *createGroup(Database *db, const QString &groupName = QString("testGroup"));

    bool deleteLocalDb(const QString &dbPath);
    QString generateTempDbPath(const QString& pattern = QString("test"));
    QSharedPointer<SyncObject> syncDatabase(Database* db, const QString& dbPath);
    /**
     * @brief correctDbName appends kdbx to the db name if it's not present
     * @param dbName name of the database
     * returns copy of the dbName with appended kdbx if needed
     */
    QString correctDbName(const QString& dbName);

    static void waitForSignal(const QSignalSpy &spy, const int timeout);


    RemoteDriveApi *remoteDrive = Q_NULLPTR;

private:

    // const QString dbName = "test.kdbx";
    const QString parentDir = "0B8tltL21wts3alc0eS00UVRLNnc";
    const QString newNotes = "New Notes";
    const QString newPassword = "NewPassword";
    const QString password = "Password";
};

#endif // GDRIVETESTUTILS_H
