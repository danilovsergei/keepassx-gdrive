#include "RemoteCommand.h"
#include <QtCore/QObject>

namespace KeePassxDriveSync {
RemoteCommand::RemoteCommand()
{
  qDebug() << "Create command with ID = " + ID.toString();
  connect(this, SIGNAL(emitExecuteAsync(const QVariantMap)), this,
          SLOT(execute(const QVariantMap)), Qt::UniqueConnection);
  connect(this, SIGNAL(finished()), this, SLOT(finishEventLoop()), Qt::UniqueConnection);
}

RemoteCommand::~RemoteCommand()
{
  qDebug() << "Destroy command with ID = " + ID.toString();
}

void RemoteCommand::emitError(const int errorCode, const QString &errorString)
{
  qDebug() << "Emit finished: fail";
  status = Status::NotStarted;
  if (this->errorCode == Errors::InternalError::EMPTY_ERROR_CODE) {
      qDebug() << QString("%1 : %2").arg(QString::number(errorCode), errorString);
      this->errorCode = errorCode;
      this->errorString = errorString;
  }
  Q_EMIT finished();
}

void RemoteCommand::emitSuccess()
{
  qDebug() << "Emit finished: success";
  status = Status::NotStarted;
  this->errorCode = Errors::NO_ERROR;
  this->errorString = "";
  Q_EMIT finished();
}

void RemoteCommand::waitForFinish()
{
  if (status == Status::NotStarted)
    return;
  QEventLoop loop;
  this->loop = &loop;
  // processUserInputEvents ? QEventLoop::AllEvents : QEventLoop::ExcludeUserInputEvents
  loop.exec(QEventLoop::AllEvents);
  this->loop = Q_NULLPTR;
}

void RemoteCommand::finishEventLoop()
{
  if (loop)
    loop->quit();
}

const QVariantList RemoteCommand::getResult()
{
  return result;
}

const QString RemoteCommand::getErrorString()
{
  return errorString;
}

const int RemoteCommand::getErrorCode()
{
  return errorCode;
}

void RemoteCommand::executeAsync(const QVariantMap options)
{
  status = Status::InProgress;
  Q_EMIT emitExecuteAsync(options);
}

void RemoteCommand::setResult(QVariantList result)
{
  this->result.clear();
  this->result = result;
}

const bool RemoteCommand::checkForError(Command cmd)
{
  bool result = false;
  if (cmd->getErrorCode() != Errors::NO_ERROR) {
    emitError(cmd->getErrorCode(), cmd->getErrorString());
    result = true;
  }
  return result;
}
}
