#ifndef REMOTEDRIVEAPI_H
#define REMOTEDRIVEAPI_H
#include "remotedrive/CommandsFactory.h"
#include <QtCore/QScopedPointer>
#include <QtCore/QVariantMap>
#include <QtCore/QThread>
#include "Errors.h"
#include "remotedrive/RemoteCommand.h"
#include "core/Global.h"
using namespace KeePassxDriveSync;

class RemoteDriveApi : public QObject
{
  Q_OBJECT

public:

  RemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl);
  ~RemoteDriveApi();

  KeePassxDriveSync::Command download();
  KeePassxDriveSync::Command upload();
  KeePassxDriveSync::Command list();
  KeePassxDriveSync::Command sync();
  KeePassxDriveSync::Command remove();

private:
  CommandsFactory *getFactory();
  void raiseError(int errorType, const QString &description);

protected:
  CommandsFactory *factoryImpl = Q_NULLPTR;
private:
  QThread driveWorkerThread;
protected:
    // make protected level to be available for fakes
    KeePassxDriveSync::Command getThreadedCommand(KeePassxDriveSync::Command  command);
};

#endif // REMOTEDRIVEAPI_H
