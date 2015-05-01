#ifndef GDRIVETESTUTILS_H
#define GDRIVETESTUTILS_H
#include <QtCore/QSharedPointer>
#include "remotedrive/SyncObject.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"
#include "remotedrive/gdrive/GoogleDriveTools.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <keys/PasswordKey.h>
#include <format/KeePass2Writer.h>
#include <format/KeePass2Reader.h>
#include <core/qsavefile.h>
#include <QtCore/qmath.h>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "remotedrive/CommandsFactory.h"
#include "remotedrive/AuthCredentials.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/Command.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"


using namespace DatabaseSyncObject;
using namespace GoogleDrive;
using KeePassxDriveSync::Command;

class GDriveTestUtils : public QObject
{
    Q_OBJECT

public:
    GDriveTestUtils();
    bool compareResult(QSharedPointer<SyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database *readDatabase(const CompositeKey &key, const QString &dbPath);
    Database *createLocalDatabase(const QString &dbPath);
    bool uploadDb(const QString &dbPath);
    bool deleteDb(const FileInfo &db);
    bool deleteAllDb(const QString &dbTitle);
    const CompositeKey getTestCompositeKey();
    bool saveDatabase(Database *db, const QString &dbPath);
    Entry *createEntry(Database *db, const QString &title = QString("testTitle"),
                       const QString &password = QString("testPassword"));
    Group *createGroup(Database *db, const QString &groupName = QString("testGroup"));
    Entry *createEntry(const QString &title = QString("testTitle"),
                       const QString &password = QString("testPassword"));
    Group *createGroup(const QString &groupName = QString("testGroup"));
    bool deleteLocalDb(const QString &dbPath);
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
