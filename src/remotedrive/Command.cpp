#include "Command.h"
#include <QtCore/QObject>

namespace KeePassxDriveSync {
Command::Command()
{
    connect(this, SIGNAL(emitExecuteAsync(const QVariantMap)), this,
            SLOT(execute(const QVariantMap)));
}

void Command::emitError(const int errorCode, const QString &errorString)
{
  qDebug() << QString("%1 : %2").arg(QString::number(errorCode), errorString);
  this->errorCode = errorCode;
  this->errorString = errorString;
  Q_EMIT finished();
}

void Command::emitSuccess()
{
  this->errorCode = Errors::NO_ERROR;
  this->errorString = "";
  Q_EMIT finished();
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
  Q_EMIT emitExecuteAsync(options);
}
}
