#ifndef COMMANDSFACTORYIMPL_H
#define COMMANDSFACTORYIMPL_H

#include "qtdrive/lib/session.h"
#include "remotedrive/AuthCredentials.h"
#include "GDriveConstants.h"
#include <QtNetwork/QNetworkAccessManager>
#include "GDriveConstants.h"

#include "remotedrive/CommandsFactory.h"
#include "remotedrive/gdrive/commands/DownloadCommand.h"
#include "remotedrive/gdrive/commands/SyncCommand.h"
#include "remotedrive/gdrive/commands/ListCommand.h"
#include "remotedrive/gdrive/commands/UploadCommand.h"
#include "remotedrive/Command.h"

using namespace GoogleDrive;
class CommandsFactoryImpl : public CommandsFactory
{
    Q_OBJECT
public:
    CommandsFactoryImpl(QObject *parent, AuthCredentials *creds);
    KeePassxDriveSync::Command *download();
    KeePassxDriveSync::Command *sync();
    KeePassxDriveSync::Command *list();
    KeePassxDriveSync::Command *upload();
private:
    Session *session = Q_NULLPTR;
    AuthCredentials *creds;
    Session *getSession();
    void waitForCredsRefresh();

Q_SIGNALS:
    void updateCredentials();
};

#endif // COMMANDSFACTORYIMPL_H
