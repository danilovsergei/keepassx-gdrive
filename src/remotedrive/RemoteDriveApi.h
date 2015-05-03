#ifndef REMOTEDRIVEAPI_H
#define REMOTEDRIVEAPI_H
#include "CommandsFactory.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QVariantMap>
#include <QtCore/QThread>
#include "Errors.h"
#include "remotedrive/Command.h"
#include "core/Global.h"
//using namespace KeePassxDriveSync;
class RemoteDriveApi : QObject
{
    Q_OBJECT

public:

    RemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl);
    void     init();
    void     executeAsync(KeePassxDriveSync::Command *cmd, const QVariantMap &args);
    void     execute(KeePassxDriveSync::Command *cmd, const QVariantMap &args);

    KeePassxDriveSync::Command *download();
    KeePassxDriveSync::Command *upload();
    KeePassxDriveSync::Command *list();
    KeePassxDriveSync::Command *sync();

private:
    CommandsFactory *factoryImpl = Q_NULLPTR;
    CommandsFactory *getFactory();
    void raiseError(int errorType, const QString &description);
};

#endif // REMOTEDRIVEAPI_H
