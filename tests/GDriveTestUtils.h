#ifndef GDRIVETESTUTILS_H
#define GDRIVETESTUTILS_H
#include <QtCore/QSharedPointer>
#include "gdrive/GDriveSyncObject.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"
#include "gdrive/GoogleDriveApi.h"
#include "gdrive/GoogleDriveTools.h"
#include "qtdrive/lib/file_info.h"
#include "qtdrive/lib/command_delete.h"
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <keys/PasswordKey.h>
#include <format/KeePass2Writer.h>
#include <format/KeePass2Reader.h>
#include <core/qsavefile.h>
#include <QtCore/qmath.h>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

using namespace DatabaseSync;
using namespace GoogleDrive;
class GDriveTestUtils
{
public:
    GDriveTestUtils();
    bool compareResult(QSharedPointer<GDriveSyncObject> actual, QMap<SyncMapKey, int> expectedMap);
    Database* readDatabase(const CompositeKey& key,const QString& dbPath);
    Database* createLocalDatabase(const QString& dbPath);
    bool uploadDb(const QString& dbPath);
    bool deleteDb(const FileInfo& db);
    bool deleteAllDb(const QString& dbTitle);
    const CompositeKey getTestCompositeKey();
    bool saveDatabase(Database* db, const QString& dbPath);
    Entry* createEntry(Database* db,const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(Database* db,const QString& groupName=QString("testGroup"));
    Entry* createEntry(const QString& title=QString("testTitle"),const QString& password=QString("testPassword"));
    Group* createGroup(const QString& groupName=QString("testGroup"));
    bool deleteLocalDb(const QString& dbPath);
    static void waitForSignal(const QSignalSpy& spy, const int timeout);
    GoogleDriveApi* gdrive=googleDriveApi();
private:

    //const QString dbName = "test.kdbx";
    const QString parentDir = "0B8tltL21wts3alc0eS00UVRLNnc";
    const QString newNotes = "New Notes";
    const QString newPassword = "NewPassword";
    const QString password = "Password";
};

#endif // GDRIVETESTUTILS_H
