#include "CommandsFactoryImpl.h"

CommandsFactoryImpl::CommandsFactoryImpl(QObject *parent, AuthCredentials *creds) : CommandsFactory(
    parent),
  creds(creds)
{
}

KeePassxDriveSync::Command *CommandsFactoryImpl::download()
{
  return countCommand(new DownloadCommand(creds));
}

KeePassxDriveSync::Command *CommandsFactoryImpl::sync()
{
  return countCommand(new SyncCommand(creds));
}

KeePassxDriveSync::Command *CommandsFactoryImpl::list()
{
  return countCommand(new ListCommand(creds));
}

KeePassxDriveSync::Command *CommandsFactoryImpl::remove()
{
  return countCommand(new DeleteCommand(creds));
}

KeePassxDriveSync::Command *CommandsFactoryImpl::upload()
{
  return countCommand(new UploadCommand(creds));
}
