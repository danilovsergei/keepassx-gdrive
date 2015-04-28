#include "Command.h"
namespace KeePassxDriveSync {
Command::Command()
{
    // executeAsync signal designed to be emmited from another thread that given function
//    connect(this, SIGNAL(executeAsync(const QVariantMap &)), this, SLOT(execute(
//                                                                            const QVariantMap &)));
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

void Command::executeAsync(const QVariantMap &options)
{
    QtConcurrent::run(this,
                      &Command::execute, options);
}
}
