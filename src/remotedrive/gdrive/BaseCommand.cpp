#include "BaseCommand.h"
QThreadStorage<Session *> BaseCommand::session;

BaseCommand::BaseCommand(AuthCredentials* creds)
    : creds(creds)
{
    // creds are living in the GUI thread while this in worker thread.
    connect(this, SIGNAL(updateCredentials()), creds, SLOT(update()));
}

Session *BaseCommand::getSession()
{
  QVariantMap credentials = creds->getCredentials();
  Q_ASSERT(credentials.size() > 0);
  Q_ASSERT(credentials.contains(REFRESH_TOKEN) > 0);
  if (credentials.value(REFRESH_TOKEN).toString().isEmpty()) {
    Q_EMIT updateCredentials();
    waitForCredsRefresh();
  }
  if (!session.hasLocalData()) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    Session *sessionObj = new Session(manager, this);
    sessionObj->setClientId(CLIENT_ID_VALUE);
    sessionObj->setClientSecret(CLIENT_SECRET_VALUE);
    sessionObj->setRefreshToken(creds->getOption(REFRESH_TOKEN));
    session.setLocalData(sessionObj);
  }
return session.localData();
}

void BaseCommand::waitForCredsRefresh()
{
  QEventLoop qE;
  QTimer tT;

  tT.setSingleShot(true);
  connect(&tT, SIGNAL(timeout()), &qE, SLOT(quit()));
  connect(creds, SIGNAL(updated()), &qE, SLOT(quit()));
  tT.start(LOGIN_WAIT_TIMEOUT);
  qE.exec();
}
