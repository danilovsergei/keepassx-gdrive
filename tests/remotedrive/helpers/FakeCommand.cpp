#include "FakeCommand.h"
namespace FakeCommands {
void FakeSyncCommand::execute(const QVariantMap options)
{
  setResult(KeePassxDriveSync::ResultBuilder().addValue(QThread::currentThreadId()).build());
  emitSuccess();
}

void FakeAsyncCommand::execute(const QVariantMap options)
{
  setResult(KeePassxDriveSync::ResultBuilder().addValue(QThread::currentThreadId()).build());
  emitSuccess();
}

void FakeFailCommand::execute(const QVariantMap options)
{
  emitError(Errors::AuthorizationError::GENERAL_AUTH_PROBLEM, "Authorization failed for test user");
}

void FakeArgumentsCommand::execute(const QVariantMap options)
{
  if (options.contains("Test") && options.value("Test").toString() == "StringKey")
    emitSuccess();
  else
    emitError(Errors::InternalError::EMPTY_ERROR_CODE,
              "No or incorrect arguments were passed to the command");

}
}
