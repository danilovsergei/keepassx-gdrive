#include "DownloadCommandTest.h"


QTEST_GUILESS_MAIN(DownloadCommandTest)
using namespace GoogleDrive;
using namespace KeePassxDriveSync;
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
  helper.reset(DownloadCommand::newInstance());
}

void DownloadCommandTest::cleanup() {
  testUtils.deleteAllDb(dbName);
  testUtils.deleteLocalDb(dbPath);
  delete db;
}

void DownloadCommandTest::testDbDownload() {
  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

  helper->execute(OptionsBuilder::newInstance()->addOption(OPTION_FI,
                                                           fi)->addOption(
                    OPTION_DB_DIR, QDir::tempPath())->build());
  QCOMPARE(helper->getResult().size(),                    1);
  const QString localDb = helper->getResult().at(0).toString();

  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
  QCOMPARE(helper->getErrorCode(),
           static_cast<int>(Errors::NO_ERROR));
}

void DownloadCommandTest::testDbDownloadDefaultFolder() {
  const QString fullDbName(config()->get(
                             "cloud/dbdir").toString() + QDir::separator() +
                           dbName);
  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

  helper->execute(OptionsBuilder::newInstance()->addOption(OPTION_FI,
                                                           fi)->build());
  QCOMPARE(helper->getErrorCode(),
           static_cast<int>(Errors::NO_ERROR));
  QCOMPARE(helper->getResult().size(),                    1);
  const QString localDb = helper->getResult().at(0).value<QString>();
  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
  QCOMPARE(localDb,                                       fullDbName);
}

void DownloadCommandTest::testDbDownloadNoDbNameError() {
  QScopedPointer<KeePassxDriveSync::Command> helper(DownloadCommand::newInstance());
  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());

  // delete cloud database to force not database found error
  testUtils.deleteDb(fi);
  helper->execute(OptionsBuilder::newInstance()->addOption(OPTION_FI,
                                                           fi)->addOption(
                    OPTION_DB_DIR, QDir::tempPath())->build());
  QCOMPARE(helper->getErrorCode(),
           static_cast<int>(Errors::DownloadError::
                            GOOGLEDRIVEAPI_DOWNLOAD_COMMAND_PROBLEM));
}

void DownloadCommandTest::testEmitDownloadDatabaseDone()  {
  const FileInfo fi(googleDriveApi()->getDatabasesSeq(filter).first());
  QSignalSpy     spy(helper.data(), SIGNAL(finished()));

  GoogleDriveSession * session = GoogleDriveSession::getEmptySession();
  qDebug() << "main thread =" << this->thread();
  helper->executeAsync(OptionsBuilder::newInstance()->addOption(OPTION_FI,
                                                                fi)->addOption(
                         OPTION_DB_DIR, QDir::tempPath())->build());
  GDriveTestUtils::waitForSignal(spy, SPY_WAIT_TIMEOUT);

  // make sure signal was emitted
  QCOMPARE(spy.count(), 1);

//  QList<QVariant> args    = spy.takeFirst();
//  const QString   localDb = helper->getResult().at(0).value<QString>();
//  QCOMPARE(localDb.size() > 0 && QFile(localDb).exists(), true);
}
