#ifndef REMOTEDRIVEAPI_H
#define REMOTEDRIVEAPI_H
#include "remotedrive/CommandsFactory.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QVariantMap>
#include <QtCore/QThread>
#include "Errors.h"
#include "remotedrive/Command.h"
#include "core/Global.h"

class RemoteDriveApi : public QObject
{
    Q_OBJECT

public:

    RemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl);
    ~RemoteDriveApi();

    KeePassxDriveSync::Command *download();
    KeePassxDriveSync::Command *upload();
    KeePassxDriveSync::Command *list();
    KeePassxDriveSync::Command *sync();
    KeePassxDriveSync::Command *remove();

private:
    CommandsFactory *getFactory();
    void raiseError(int errorType, const QString &description);

protected:
   CommandsFactory *factoryImpl = Q_NULLPTR;
   QThread driveWorkerThread;

};

#endif // REMOTEDRIVEAPI_H
