#ifndef DownloadCommandTest_H
#define DownloadCommandTest_H
#include <QtCore/QObject>
#include <QtTest/QTest>
#include "../../tests.h"
#include "GDriveTestUtils.h"
#include "crypto/Crypto.h"
#include "remotedrive/Errors.h"
#include <QtTest/QSignalSpy>
#include "remotedrive/Errors.h"
#include "remotedrive/gdrive/commands/DownloadCommand.h"

using namespace KeePassxDriveSync;
class DownloadCommandTest : public QObject {
        Q_OBJECT
private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    /**  @brief testDbDownload check db download to the specified folder */
    void testDbDownload();
    /**  @brief testDbDownload check db download to the config default folder */
    void testDbDownloadDefaultFolder();
    void testDbDownloadNoDbNameError();
    void testEmitDownloadDatabaseDone();
private:
    const QString   dbName = "test.kdbx";
    const QString   oldDbName = "oldDb.kdbx";
    const QString   oldDbPath = QDir::tempPath() + QDir::separator() + oldDbName;
    const QString   dbPath = QDir::tempPath() + QDir::separator() + dbName;
    const QList<QueryEntry> filter = GoogleDriveTools::getDbNameFilter(dbName);
    Database* db;
    QScopedPointer<KeePassxDriveSync::Command> helper;

};

#endif // DownloadCommandTest_H
