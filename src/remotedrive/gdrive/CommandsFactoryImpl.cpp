#include "CommandsFactoryImpl.h"

CommandsFactoryImpl::CommandsFactoryImpl(QObject *parent, AuthCredentials *creds) : CommandsFactory(
        parent), creds(creds)
{
    // creds are living in the GUI thread while this in worker thread.
    connect(this, SIGNAL(updateCredentials()), creds, SLOT(update()));
}

// AuthCredentials * CommandsFactoryImpl::getCreds() {}

Session *CommandsFactoryImpl::getSession()
{
    if (creds->getCredentials()->value(REFRESH_TOKEN).toString().isEmpty()) {
        Q_EMIT updateCredentials();
        waitForCredsRefresh();
    }

    if (session == Q_NULLPTR) {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        session = new Session(manager, this);
        session->setClientId(CLIENT_ID_VALUE);
        session->setClientSecret(CLIENT_SECRET_VALUE);
        session->setRefreshToken(creds->getOption(REFRESH_TOKEN));
    }

    return session;
}

void CommandsFactoryImpl::waitForCredsRefresh()
{
    QEventLoop qE;
    QTimer tT;

    tT.setSingleShot(true);
    connect(&tT, SIGNAL(timeout()), &qE, SLOT(quit()));
    connect(creds, SIGNAL(updated()), &qE, SLOT(quit()));
    tT.start(LOGIN_WAIT_TIMEOUT);
    qE.exec();
}

KeePassxDriveSync::Command *CommandsFactoryImpl::download()
{
    return new DownloadCommand(getSession());
}

KeePassxDriveSync::Command *CommandsFactoryImpl::sync()
{
    return new SyncCommand(getSession());
}

KeePassxDriveSync::Command *CommandsFactoryImpl::list()
{
    return new ListCommand(getSession());
}

KeePassxDriveSync::Command *CommandsFactoryImpl::remove()
{
    return new DeleteCommand(getSession());
}

KeePassxDriveSync::Command *CommandsFactoryImpl::upload()
{
    return new UploadCommand(getSession());
}

