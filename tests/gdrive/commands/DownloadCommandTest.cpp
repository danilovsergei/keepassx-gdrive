#include "DownloadCommandTest.h"
#include "gdrive/Errors.h"
#include "gdrive/commands/DownloadCommand.h"
QTEST_GUILESS_MAIN(DownloadCommandTest)
using namespace GoogleDrive;
GDriveTestUtils  testUtils;
static const int SPY_WAIT_TIMEOUT = 10000;
void DownloadCommandTest::initTestCase() {
  Crypto::init();
}

void DownloadCommandTest::init() {
  db = testUtils.createLocalDatabase(dbPath);
  testUtils.deleteAllDb(dbName);
  testUtils.uploadDb(dbPath);
  Q_ASSERT(testUtils.deleteLocalDb(dbPath));
}

void DownloadCommandTest::cleanup() {
  testUtils.deleteAllDb(dbName);
  testUtils.deleteLocalDb(dbPath);
  delete db;
}

void DownloadCommandTest::testDbDownload() {
  QScopedPointer<KeePassxDriveSync::Command> helper (DownloadCommand::newInstance());
  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

  QVariantList options;
  options.append(QVariant::fromValue(fi));
  options.append(QDir::tempPath());
  helper->execute(options);
  QCOMPARE(helper->getResult().size(), 1);
  const QString localDb = helper->getResult().at(0).toString();

  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
  QCOMPARE(helper->getErrorCode(),
           static_cast<int>(Errors::NO_ERROR));
}

//void DownloadCommandTest::testDbDownloadDefaultFolder() {
//  const QString fullDbName(config()->get(
//                             "cloud/dbdir").toString() + QDir::separator() +
//                           dbName);

//  QSharedPointer<GDriveDbDownloadHelper> helper =
//    GDriveDbDownloadHelper::newInstance();
//  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());
//  const QString  localDb =  helper->downloadDatabase(fi);
//  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
//  QCOMPARE(localDb,                                       fullDbName);
//  QCOMPARE(helper->getErrorCode(),
//           static_cast<int>(Errors::NO_ERROR));
//}

//void DownloadCommandTest::testDbDownloadNoDbNameError() {
//  QSharedPointer<GDriveDbDownloadHelper> helper =
//    GDriveDbDownloadHelper::newInstance();
//  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

//  // delete cloud database to force not database found error
//  testUtils.deleteDb(fi);
//  helper->downloadDatabase(fi, QDir::tempPath());
//  QCOMPARE(helper->getErrorCode(),
//           static_cast<int>(Errors::DownloadError::
//                            GOOGLEDRIVEAPI_DOWNLOAD_COMMAND_PROBLEM));
//}

//void DownloadCommandTest::testEmitDownloadDatabaseDone()  {
//  QSharedPointer<GDriveDbDownloadHelper> helper =
//    GDriveDbDownloadHelper::newInstance();
//  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());
//  QSignalSpy     spy(helper.data(),
//                     SIGNAL(downloadDatabaseDone(const QString &)));
//  helper->downloadDatabaseParallel(fi, QDir::tempPath());
//  GDriveTestUtils::waitForSignal(spy, SPY_WAIT_TIMEOUT);

//  // make sure signal was emitted
//  QCOMPARE(spy.count(), 1);

//  QList<QVariant> args = spy.takeFirst();
//  QString localDb(args.at(0).value<QString>());
//  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
//}

//void DownloadCommandTest::testEmitDownloadDatabaseError() {
//  QSharedPointer<GDriveDbDownloadHelper> helper =
//    GDriveDbDownloadHelper::newInstance();
//  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

//  // delete cloud database to force not database found error
//  testUtils.deleteDb(fi);

//  QSignalSpy spy(helper.data(), SIGNAL(downloadDatabaseError(int,const QString&)));
//  helper->downloadDatabaseParallel(fi, QDir::tempPath());
//  GDriveTestUtils::waitForSignal(spy, SPY_WAIT_TIMEOUT);

//  // make sure signal was emitted
//  QCOMPARE(spy.count(), 1);

//  QList<QVariant> args = spy.takeFirst();
//  int errorCode(args.at(0).value<int>());
//  QCOMPARE(errorCode, static_cast<int>(Errors::DownloadError::
//                                       GOOGLEDRIVEAPI_DOWNLOAD_COMMAND_PROBLEM));
//}
