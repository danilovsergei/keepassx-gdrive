#include "FakeRemoteDriveApi.h"
FakeRemoteDriveApi::FakeRemoteDriveApi(QObject *parent, CommandsFactory *factoryImpl) :
  RemoteDriveApi(parent, factoryImpl)
{
  fakeFactory = static_cast<FakeCommandsFactoryImpl *>(factoryImpl);
}

KeePassxDriveSync::Command FakeRemoteDriveApi::fakeSyncCommand()
{
  return getThreadedCommand(fakeFactory->fakeSyncCommand());
}

KeePassxDriveSync::Command FakeRemoteDriveApi::fakeAsyncCommand()
{
  return getThreadedCommand(fakeFactory->fakeAsyncCommand());
}

KeePassxDriveSync::Command FakeRemoteDriveApi::fakeFailCommand()
{
  return getThreadedCommand(fakeFactory->fakeFailCommand());
}

KeePassxDriveSync::Command FakeRemoteDriveApi::fakeArgumentsCommand()
{
  return getThreadedCommand(fakeFactory->fakeArgumentsCommand());
}


