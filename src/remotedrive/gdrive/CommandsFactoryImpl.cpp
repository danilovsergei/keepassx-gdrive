#include "CommandsFactoryImpl.h"

CommandsFactoryImpl::CommandsFactoryImpl(AuthCredentials *creds)
  : creds(creds)
{
  // creds are living in the GUI thread while this in worker thread.
  connect(this, signal(updateCredentials), creds, SLOT(update()));
}

AuthCredentials * CommandsFactoryImpl::getCreds() {

}


Session* CommandsFactoryImpl::getSession() {
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

CommandsFactoryImpl::waitForCredsRefresh() {
    QEventLoop qE;
    QTimer     tT;

    tT.setSingleShot(true);
    connect(&tT,  SIGNAL(timeout()),      &qE, SLOT(quit()));
    connect(creds, SIGNAL(updated()), &qE, SLOT(quit()));
    tT.start(LOGIN_WAIT_TIMEOUT);
    qE.exec();
}


CommandsFactoryImpl::download(QVariantMap arg1s) {
  DownloadCommand cmd(getSession());

  cmd.execute(args);
}


