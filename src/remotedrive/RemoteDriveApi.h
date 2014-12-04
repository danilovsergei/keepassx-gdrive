#ifndef REMOTEDRIVEAPI_H
#define REMOTEDRIVEAPI_H
#include "CommandsFactory.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QVariantMap>
#include <QtCore/QThread>
#include "Errors.h"

class RemoteDriveApi
{
public:
    void init(CommandsFactory* factoryImpl);
    void download(const QVariantMap& args);

private:
    RemoteDriveApi();
    QScopedPointer<CommandsFactory> factoryImpl;
    void raiseError(Errors::AuthorizationError errorType,const QString& description);
    QScopedPointer<CommandsFactory> getFactory();


};

#endif // REMOTEDRIVEAPI_H
