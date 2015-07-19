#include "FakeCommandsFactoryImpl.h"

FakeCommandsFactoryImpl::FakeCommandsFactoryImpl(QObject *parent) : CommandsFactoryImpl(parent,
                                                                                        Q_NULLPTR)
{
}

KeePassxDriveSync::Command FakeCommandsFactoryImpl::fakeSyncCommand()
{
  return KeePassxDriveSync::Command(new FakeSyncCommand());
}

KeePassxDriveSync::Command FakeCommandsFactoryImpl::fakeAsyncCommand()
{
  return KeePassxDriveSync::Command(new FakeAsyncCommand());
}

KeePassxDriveSync::Command FakeCommandsFactoryImpl::fakeFailCommand()
{
  return KeePassxDriveSync::Command(new FakeFailCommand());
}

KeePassxDriveSync::Command FakeCommandsFactoryImpl::fakeArgumentsCommand()
{
  return KeePassxDriveSync::Command(new FakeArgumentsCommand());
}
