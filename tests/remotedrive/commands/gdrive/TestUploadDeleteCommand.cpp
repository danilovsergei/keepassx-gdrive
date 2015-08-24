#include "TestUploadDeleteCommand.h"
Q_DECLARE_METATYPE(RemoteFile)

void TestUploadDeleteCommand::initTestCase()
{
  AuthCredentials *creds = new GoogleDriveCredentials(this);
  creds->init();
  CommandsFactory *commandsFactory = new CommandsFactoryImpl(this, creds);
  // remote drive will be used to call all remote drive functions like sync , upload, download
  remoteDriveApi = new RemoteDriveApi(this, commandsFactory);

  testUtils.reset(new GDriveTestUtils(remoteDriveApi));
  Crypto::init();
  qDebug() << "Running init testcase";
}

TestUploadDeleteCommand::TestUploadDeleteCommand()
{
}

void TestUploadDeleteCommand::testDeleteOneFile()
{
  QString dbPath = testUtils->generateTempDbPath();
  Database* db = testUtils->createLocalDatabase(dbPath);
  RemoteFile remoteDb = testUtils->uploadDbWithResult(dbPath);
  qDebug() << "Upload command finished";
  // verify db was uploaded just fine
  QVERIFY(remoteDb.getId().size() > 0);
  QVERIFY(!remoteDb.getCreatedDate().isNull() && remoteDb.getCreatedDate().isValid());

  KeePassxDriveSync::Command deleteCommand = remoteDriveApi->remove();
  qDebug() << "delete command started";
  deleteCommand->executeAsync(OptionsBuilder().addOption(OPTION_REMOTE_FILE, remoteDb).build());
  deleteCommand->waitForFinish();
  qDebug() << "delete command finished";
  QVERIFY(deleteCommand->getErrorCode() == Errors::NO_ERROR);

  // clean up
  testUtils->deleteLocalDb(dbPath);
  delete db;
}

void TestUploadDeleteCommand::deleteTwoFiles()
{
  testDeleteOneFile();
  testDeleteOneFile();
}

void TestUploadDeleteCommand::deleleNonExistingFile()
{
  RemoteFile remoteDb;
  remoteDb.setId("non_existing_" + QString::number(QDateTime::currentMSecsSinceEpoch()));
  KeePassxDriveSync::Command deleteCommand = remoteDriveApi->remove();
  deleteCommand->executeAsync(OptionsBuilder().addOption(OPTION_REMOTE_FILE, remoteDb).build());
  deleteCommand->waitForFinish();
  // verify error was emmited. API emits error in all cases if non empty response returned
  QVERIFY2(deleteCommand->getErrorCode() == Errors::FileError::DELETE_DB_PROBLEM,
           QString("Got error code %1.Expected %2").arg(deleteCommand->getErrorCode()).arg(
             Errors::FileError::DELETE_DB_PROBLEM).toAscii());
}

void TestUploadDeleteCommand::uploadOneDb()
{
  QString dbPath = testUtils->generateTempDbPath();
  Database* db = testUtils->createLocalDatabase(dbPath);
  KeePassxDriveSync::Command uploadCommand = remoteDriveApi->upload();
  QFileInfo file(dbPath);
  uploadCommand->executeAsync(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                         dbPath).addOption(
                                OPTION_LAST_MODIFIED_TIME,
                                file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                               parentDir).build());
  uploadCommand->waitForFinish();
  QVERIFY(uploadCommand->getErrorCode() == Errors::NO_ERROR);

  // clean up
  testUtils->deleteLocalDb(dbPath);
  delete db;
}

void TestUploadDeleteCommand::uploadTwoDb()
{
  uploadOneDb();
  uploadOneDb();
}

void TestUploadDeleteCommand::updateDbRevision()
{
  QString dbPath = testUtils->generateTempDbPath();
  Database* db = testUtils->createLocalDatabase(dbPath);

  KeePassxDriveSync::Command uploadCommand = remoteDriveApi->upload();
  QFileInfo file(dbPath);
  QVariantMap options(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                 dbPath).addOption(
                        OPTION_LAST_MODIFIED_TIME,
                        file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                       parentDir).build());
  uploadCommand->executeAsync(options);
  uploadCommand->waitForFinish();
  QVERIFY(uploadCommand->getErrorCode() == Errors::NO_ERROR);
  RemoteFile firstRemoteDb = uploadCommand->getResult().at(0).value<RemoteFile>();
  Tools::sleep(1000);
  Entry* entry = new Entry();
  entry->setParent(db->rootGroup());
  testUtils->saveDatabase(db, dbPath);
  uploadCommand->executeAsync(options);
  uploadCommand->waitForFinish();
  RemoteFile secondRemoteDb = uploadCommand->getResult().at(0).value<RemoteFile>();

  // expect both databases are the same dabases with the same id but with different revision
  QCOMPARE(firstRemoteDb.getId(), secondRemoteDb.getId());
  QVERIFY2(secondRemoteDb.getModifiedDate() > firstRemoteDb.getModifiedDate(),
           "Expect second database will have latest modifiedDate");
  QVERIFY2(firstRemoteDb.getHeadRevisionId() != secondRemoteDb.getHeadRevisionId(),
           "Expect second database has different head revision");

  // clean up
  testUtils->deleteLocalDb(dbPath);
  delete db;
}

void TestUploadDeleteCommand::testDownloadOneFile()
{
  // first upload file to the cloud
  QString dbPath = testUtils->generateTempDbPath();
  Database* db = testUtils->createLocalDatabase(dbPath);

  KeePassxDriveSync::Command uploadCommand = remoteDriveApi->upload();
  QFileInfo file(dbPath);
  QVariantMap options(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                 dbPath).addOption(
                        OPTION_LAST_MODIFIED_TIME,
                        file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                       parentDir).build());
  uploadCommand->executeAsync(options);
  uploadCommand->waitForFinish();
  QVERIFY(uploadCommand->getErrorCode() == Errors::NO_ERROR);
  RemoteFile remoteDb = uploadCommand->getResult().at(0).value<RemoteFile>();
  // clean up local database
  testUtils->deleteLocalDb(dbPath);

  // try to download file back
  KeePassxDriveSync::Command downloadCommand = remoteDriveApi->download();
  downloadCommand->executeAsync(OptionsBuilder().addOption(OPTION_FI,
                                                           remoteDb).addOption(OPTION_DB_DIR,
                                                                               QDir::tempPath()).build());
  downloadCommand->waitForFinish();
  QVERIFY(downloadCommand->getErrorCode() == Errors::NO_ERROR);

  // expect database saved locally
  QString expectedPath = QDir::tempPath() + QDir::separator() + remoteDb.getTitle();
  QVERIFY(QFile(expectedPath).exists());

  // clean up
  testUtils->deleteLocalDb(dbPath);
  delete db;
}

void TestUploadDeleteCommand::testDownloadTimeStamp()
{
  // first upload file to the cloud
  QString dbPath = testUtils->generateTempDbPath();
  Database* db = testUtils->createLocalDatabase(dbPath);
  KeePassxDriveSync::Command uploadCommand = remoteDriveApi->upload();
  QFileInfo file(dbPath);
  QDateTime localTimeStamp = file.lastModified();
  QVariantMap options(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                 dbPath).addOption(
                        OPTION_LAST_MODIFIED_TIME,
                        file.lastModified()).addOption(OPTION_PARENT_NAME,
                                                       parentDir).build());
  Tools::sleep(1000);
  uploadCommand->executeAsync(options);
  uploadCommand->waitForFinish();
  QVERIFY(uploadCommand->getErrorCode() == Errors::NO_ERROR);
  RemoteFile remoteDb = uploadCommand->getResult().at(0).value<RemoteFile>();
  // clean up local database
  testUtils->deleteLocalDb(dbPath);
  QDateTime remoteTimeStamp = remoteDb.getModifiedDate();
  qDebug() << QString("localTime = %1").arg(localTimeStamp.toLocalTime().toString());
  qDebug() << QString("remoteTime = %1").arg(remoteTimeStamp.toLocalTime().toString());
  QCOMPARE(localTimeStamp.toLocalTime().toTime_t(), remoteTimeStamp.toLocalTime().toTime_t());

  // try to download file back
  KeePassxDriveSync::Command downloadCommand = remoteDriveApi->download();
  downloadCommand->executeAsync(OptionsBuilder().addOption(OPTION_FI,
                                                           remoteDb).addOption(OPTION_DB_DIR,
                                                                               QDir::tempPath()).build());
  downloadCommand->waitForFinish();
  QVERIFY(downloadCommand->getErrorCode() == Errors::NO_ERROR);
  QString expectedPath = QDir::tempPath() + QDir::separator() + remoteDb.getTitle();
  QVERIFY(QFile(expectedPath).exists());
  QFileInfo downloadedFile(expectedPath);
  QCOMPARE(downloadedFile.lastModified().toLocalTime().toTime_t(),
           remoteTimeStamp.toLocalTime().toTime_t());

  // clean up
  testUtils->deleteLocalDb(dbPath);
  delete db;
}

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  TestUploadDeleteCommand tc;
  return QTest::qExec(&tc, argc, argv);
}
