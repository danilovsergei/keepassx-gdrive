#include "FakeCommandsFactoryImpl.h"

FakeCommandsFactoryImpl::FakeCommandsFactoryImpl(QObject *parent) : CommandsFactoryImpl(parent,
                                                                                        Q_NULLPTR)
{
}

KeePassxDriveSync::Command *FakeCommandsFactoryImpl::fakeSyncCommand()
{
  return new FakeSyncCommand();
}

KeePassxDriveSync::Command *FakeCommandsFactoryImpl::fakeAsyncCommand()
{
  return new FakeAsyncCommand();
}

KeePassxDriveSync::Command *FakeCommandsFactoryImpl::fakeFailCommand()
{
  return new FakeFailCommand();
}

KeePassxDriveSync::Command *FakeCommandsFactoryImpl::fakeArgumentsCommand()
{
  return new FakeArgumentsCommand();
}
