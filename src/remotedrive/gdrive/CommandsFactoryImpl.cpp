#include "CommandsFactoryImpl.h"

CommandsFactoryImpl::CommandsFactoryImpl(QObject *parent, AuthCredentials *creds) : CommandsFactory(
    parent),
  creds(creds)
{
}

KeePassxDriveSync::Command CommandsFactoryImpl::download()
{
  return KeePassxDriveSync::Command(new DownloadCommand(creds));
}

KeePassxDriveSync::Command CommandsFactoryImpl::sync()
{
   return KeePassxDriveSync::Command(new SyncCommand(creds));
}

KeePassxDriveSync::Command CommandsFactoryImpl::list()
{
  return KeePassxDriveSync::Command(new ListCommand(creds));
}

KeePassxDriveSync::Command CommandsFactoryImpl::remove()
{
   return KeePassxDriveSync::Command(new DeleteCommand(creds));
}

KeePassxDriveSync::Command CommandsFactoryImpl::upload()
{
   return KeePassxDriveSync::Command(new UploadCommand(creds));
}
