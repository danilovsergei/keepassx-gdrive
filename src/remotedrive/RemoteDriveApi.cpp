#include "RemoteDriveApi.h"
static const int EXECUTION_TIMEOUT = 60 * 1000;

RemoteDriveApi::RemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl) : QObject(parent),
    factoryImpl(
        factoryImpl)
{
    // thread will execute all remote drive commands like upload , download etc
    QThread driveWorkerThread;

    this->factoryImpl->moveToThread(&driveWorkerThread);
    driveWorkerThread.start();
}

void RemoteDriveApi::init()
{
}

CommandsFactory *RemoteDriveApi::getFactory()
{
    factoryImpl == Q_NULLPTR ? raiseError(
        Errors::RemoteApiError::API_INITIALIZATION_PROBLEM,
        QString(
            "Factory initialization error. Please init RemoteDriveApi before using it's other methods"))
    : void();
    return factoryImpl;
}

Command *RemoteDriveApi::download()
{
    return factoryImpl->download();
}

Command *RemoteDriveApi::list()
{
    return factoryImpl->list();
}

void RemoteDriveApi::upload(const QVariantMap &args)
{
    // TODO add implementation
    qFatal("No implementation for upload yet");
}

KeePassxDriveSync::Command *RemoteDriveApi::sync()
{
    return factoryImpl->sync();
}

void RemoteDriveApi::raiseError(int errorType, const QString &description)
{
    qDebug() << errorType << description;
    Q_ASSERT(false);
}

void RemoteDriveApi::execute(Command *cmd, const QVariantMap &args)
{
    cmd->execute(args);
}

void RemoteDriveApi::executeAsync(Command *cmd, const QVariantMap &args)
{
    QMetaObject::invokeMethod(cmd, "executeAsync", Q_ARG(QVariantMap, args));
}
