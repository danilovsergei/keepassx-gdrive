#include "RemoteDriveApi.h"
static const int EXECUTION_TIMEOUT = 60 * 1000;

RemoteDriveApi::RemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl) : QObject(parent),
    factoryImpl(
        factoryImpl)
{
    driveWorkerThread.start();
}

RemoteDriveApi::~RemoteDriveApi() {
    driveWorkerThread.quit();
    if(!driveWorkerThread.wait(3000))
    {
        driveWorkerThread.terminate();
        driveWorkerThread.wait();
    }
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
    return getThreadedCommand(factoryImpl->download());
}

Command *RemoteDriveApi::list()
{
    return getThreadedCommand(factoryImpl->list());
}

Command *RemoteDriveApi::upload()
{
    return getThreadedCommand(factoryImpl->upload());
}

Command *RemoteDriveApi::remove()
{
    return getThreadedCommand(factoryImpl->remove());
}


Command *RemoteDriveApi::sync()
{
    return getThreadedCommand(factoryImpl->sync());
}

void RemoteDriveApi::raiseError(int errorType, const QString &description)
{
    qDebug() << errorType << description;
    Q_ASSERT(false);
}

KeePassxDriveSync::Command *RemoteDriveApi::getThreadedCommand(
  KeePassxDriveSync::Command *command)
{
  command->moveToThread(&driveWorkerThread);
  return command;
}

