#include "RemoteDriveApi.h"

RemoteDriveApi::RemoteDriveApi()
{
}

void RemoteDriveApi::init(CommandsFactory* factoryImpl) {
this->factoryImpl.reset(factoryImpl);
// thread will execute all remote drive commands like upload , download etc
QThread driveWorkerThread;
this-factoryImpl->moveToThread(&driveWorkerThread);
driveWorkerThread.start();
}

QScopedPointer<CommandsFactory> RemoteDriveApi::getFactory() {
    factoryImpl.isNull() ?  raiseError(
      Errors::RemoteApiError::API_INITIALIZATION_PROBLEM,
                           QString("Factory initialization error. Please init RemoteDriveApi before using it's other methods")) : void();
    return factoryImpl;
}
void RemoteDriveApi::download(const QVariantMap& args) {
    factoryImpl->exe

}


void RemoteDriveApi::raiseError(Errors::AuthorizationError errorType,
                           const QString            & description) {
    qDebug() << description;
  errorValue       = errorType;
  errorStringValue = description;
  Q_EMIT finished();
}
