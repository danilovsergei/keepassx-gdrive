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

QVariantList Command::getResult() {
  return result;
}


QString Command::getErrorString() {
    return errorString;
}
int Command::getErrorCode() {
    return errorCode;
}

<typename T>
T getOption(QVariant option,T optionalValue);


}



