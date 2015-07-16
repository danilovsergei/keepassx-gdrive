#include "Command.h"
#include <QtCore/QObject>

namespace KeePassxDriveSync {
Command::Command()
{
  connect(this, SIGNAL(emitExecuteAsync(const QVariantMap)), this,
          SLOT(execute(const QVariantMap)), Qt::UniqueConnection);
  connect(this, SIGNAL(finished()), this, SLOT(finishEventLoop()), Qt::UniqueConnection);
}

void Command::emitError(const int errorCode, const QString &errorString)
{
  status = Status::NotStarted;
  qDebug() << QString("%1 : %2").arg(QString::number(errorCode), errorString);
  this->errorCode = errorCode;
  this->errorString = errorString;
  Q_EMIT finished();
}

void Command::emitSuccess()
{
  status = Status::NotStarted;
  this->errorCode = Errors::NO_ERROR;
  this->errorString = "";
  Q_EMIT finished();
}

void Command::waitForFinish()
{
  if (status == Status::NotStarted)
    return;
  QEventLoop loop;
  this->loop = &loop;
  // processUserInputEvents ? QEventLoop::AllEvents : QEventLoop::ExcludeUserInputEvents
  loop.exec(QEventLoop::ExcludeUserInputEvents);
  this->loop = Q_NULLPTR;
}

void Command::finishEventLoop()
{
  if (loop)
    loop->quit();
}

const QVariantList Command::getResult()
{
  return result;
}

const QString Command::getErrorString()
{
  return errorString;
}

const int Command::getErrorCode()
{
  return errorCode;
}

void Command::executeAsync(const QVariantMap options)
{
  status = Status::InProgress;
  Q_EMIT emitExecuteAsync(options);
}

void Command::setResult(QVariantList result)
{
  this->result.clear();
  this->result = result;
}

const bool Command::checkForError(Command *cmd)
{
  bool result = false;
  if (cmd->getErrorCode() != Errors::NO_ERROR) {
    emitError(cmd->getErrorCode(), cmd->getErrorString());
    result = true;
  }
  return result;
}
}
