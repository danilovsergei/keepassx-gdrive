#ifndef GDRIVEDBDOWNLOADHELPERTEST_H
#define GDRIVEDBDOWNLOADHELPERTEST_H
#include <QtCore/QObject>
#include <QtTest/QTest>
#include "../tests.h"
#include "gdrive/helpers/GDriveDbDownloadHelper.h"
#include "gdrive/GoogleDriveApi.h"
#include "GDriveTestUtils.h"
#include "crypto/Crypto.h"
#include "gdrive/Errors.h"
#include <QtTest/QSignalSpy>
class GDriveDbDownloadHelperTest : public QObject {
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
    void testEmitDownloadDatabaseError();
private:
    const QString   dbName = "test.kdbx";
    const QString   oldDbName = "oldDb.kdbx";
    const QString   oldDbPath = QDir::tempPath() + QDir::separator() + oldDbName;
    const QString   dbPath = QDir::tempPath() + QDir::separator() + dbName;
    const QList<QueryEntry> filter = GoogleDriveTools::getDbNameFilter(dbName);
    Database* db;

};

#endif // GDRIVEDBDOWNLOADHELPERTEST_H
