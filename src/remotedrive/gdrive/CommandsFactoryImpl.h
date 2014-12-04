#ifndef COMMANDSFACTORYIMPL_H
#define COMMANDSFACTORYIMPL_H
#include "remotedrive/CommandsFactory.h"
#include "qtdrive/lib/session.h"
#include "remotedrive/gdrive/commands/DownloadCommand.h"
#include "remotedrive/AuthCredentials.h"
#include "GDriveConstants.h"
#include <QtNetwork/QNetworkAccessManager>
#include "GDriveConstants.h"

using namespace GoogleDrive;
class CommandsFactoryImpl : public CommandsFactory
{
    Q_OBJECT
public:
    CommandsFactoryImpl(Session *session);
private:
    Session* session =Q_NULLPTR;
    AuthCredentials* creds;
    Session* getSession();
    waitForCredsRefresh();


Q_SIGNALS:
    updateCredentials();
};

#endif // COMMANDSFACTORYIMPL_H
