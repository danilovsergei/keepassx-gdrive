#ifndef COMMANDSFACTORY_H
#define COMMANDSFACTORY_H
#include <QObject>
#include <QVariantMap>
#include "remotedrive/RemoteCommand.h"
#include <QThread>

class CommandsFactory : public QObject
{
  Q_OBJECT
public:
  CommandsFactory(QObject *parent);
  ~CommandsFactory();
  virtual KeePassxDriveSync::Command download() = 0;
  virtual KeePassxDriveSync::Command sync() = 0;
  virtual KeePassxDriveSync::Command list() = 0;
  virtual KeePassxDriveSync::Command upload() = 0;
  virtual KeePassxDriveSync::Command remove() = 0;
};

#endif // COMMANDSFACTORY_H
