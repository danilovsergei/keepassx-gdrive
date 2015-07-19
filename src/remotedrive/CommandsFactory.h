#ifndef COMMANDSFACTORY_H
#define COMMANDSFACTORY_H
#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include "remotedrive/RemoteCommand.h"
#include <QtCore/QThread>

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
