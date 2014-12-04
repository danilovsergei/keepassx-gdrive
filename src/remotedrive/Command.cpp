#include "Command.h"
namespace KeePassxDriveSync {
Command::Command()
{}

void Command::emitError(const int errorCode, const QString& errorString) {
  qDebug() << QString("%1 : %2").arg(QString::number(errorCode), errorString);
  this->errorCode   = errorCode;
  this->errorString = errorString;
  Q_EMIT finished();
}

void Command::emitSuccess() {
  Q_EMIT finished();
}

const QVariantList Command::getResult() {
  return result;
}

const QString Command::getErrorString() {
  return errorString;
}

const int Command::getErrorCode() {
  return errorCode;
}

void Command::executeAsync(const QVariantMap& options) {
    QtConcurrent::run(this,
                      &Command::execute,options);
}


OptionsBuilder * OptionsBuilder::newInstance() {
  return new OptionsBuilder();
}

const QVariantMap OptionsBuilder::build() {
  // return by value since QVariantMap is relatively cheap object
  return instance;
}
}
