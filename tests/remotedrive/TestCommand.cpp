#include "TestCommand.h"

void TestCommand::initTestCase()
{
  commandsFactory = new FakeCommandsFactoryImpl(this);
  remoteDriveApi = new FakeRemoteDriveApi(this, commandsFactory);
}

void TestCommand::cleanupTestCase()
{}

void TestCommand::init()
{
}

void TestCommand::testSyncCommandExecution()
{
  // remote drive will be used to call all remote drive functions like sync , upload, download
  unsigned long mainThread = QThread::currentThreadId();
  KeePassxDriveSync::Command fakeCommand = remoteDriveApi->fakeSyncCommand();
  fakeCommand->execute(QVariantMap());
  QVariantList result = fakeCommand->getResult();
  unsigned long commandThreadId = result.at(0).value<unsigned long>();
  // threads should be the same since command invoked through execute method
  QVERIFY(mainThread == commandThreadId);
  QVERIFY(fakeCommand->getErrorCode() == Errors::NO_ERROR);
}

void TestCommand::testAsyncCommandExecution()
{
  unsigned long mainThread = QThread::currentThreadId();
  KeePassxDriveSync::Command fakeCommand = remoteDriveApi->fakeAsyncCommand();
  QSignalSpy spy(fakeCommand.data(), SIGNAL(finished()));
  fakeCommand->executeAsync(QVariantMap());
  int waitTime = 0;

  while (spy.count() == 0 && waitTime < 10000) {
    QTest::qWait(200);
    waitTime += 200;
  }
  // make sure finished was emitted
  QCOMPARE(spy.count(), 1);

  QVariantList result = fakeCommand->getResult();
  unsigned long commandThreadId = result.at(0).value<unsigned long>();

  // threads should be the same since command invoked through execute method
  QVERIFY(mainThread != commandThreadId);
  QVERIFY(fakeCommand->getErrorCode() == Errors::NO_ERROR);
}

void TestCommand::testFailCommandExecution()
{
  KeePassxDriveSync::Command fakeCommand = remoteDriveApi->fakeFailCommand();
   fakeCommand->executeAsync(QVariantMap());
   fakeCommand->waitForFinish();
  // make we get predefined error message
  QVERIFY(fakeCommand->getErrorCode() == Errors::AuthorizationError::GENERAL_AUTH_PROBLEM);
  QVERIFY(fakeCommand->getErrorString() == "Authorization failed for test user");
}

void TestCommand::testArgumentsCommandExecution()
{
  KeePassxDriveSync::Command fakeCommand = remoteDriveApi->fakeArgumentsCommand();
  fakeCommand->executeAsync(OptionsBuilder().addOption(QString("Test"), QString(
                                                            "StringKey")).build());
   fakeCommand->waitForFinish();
  // make we get predefined error message
  QVERIFY(fakeCommand->getErrorCode() == Errors::NO_ERROR);
}

QTEST_GUILESS_MAIN(TestCommand)
