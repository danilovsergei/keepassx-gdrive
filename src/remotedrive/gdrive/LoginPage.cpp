#include "LoginPage.h"

LoginPage::LoginPage() : approvalView(new QWebView)
{
  connect(approvalView, SIGNAL(loadFinished(bool)), this,
          SLOT(loadFinished(const bool &)));
}

void LoginPage::updateCredentials(Session &session)
{
  Q_ASSERT(approvalView);
  CommandOAuth2 cmd(&session);
  cmd.setScope(CommandOAuth2::FullAccessScope);
  approvalView->setGeometry(0, 0, geometry.first, geometry.second);
  approvalView->show();
  approvalView->load(cmd.getLoginUrl());

  // waits LOGIN_WAIT_TIMEOUT or until user approves page
  waitForUserApproval();
  approvalView->close();
  !approved ? raiseError(Errors::AuthorizationError::USER_APPROVE_PROBLEM,
                         QString(
                           "User did not approve access to google drive during %1 seconds").arg(
                           LOGIN_WAIT_TIMEOUT)) : void();
  authorizationCode.isEmpty() ? raiseError(
    Errors::AuthorizationError::GENERAL_AUTH_PROBLEM,
    QString("Failed to get authorization code")) : void();
  cmd.requestAccessToken(authorizationCode);
  cmd.waitForFinish(false);
  cmd.error() != cmd.NoError ? raiseError(
    Errors::AuthorizationError::GENERAL_AUTH_PROBLEM,
    QString("Failed to get access token")) : void();
  qDebug() << "sucessfully generated refresh token";
  Q_EMIT finished();
}

void LoginPage::waitForUserApproval()
{
  QEventLoop qE;
  QTimer tT;

  tT.setSingleShot(true);
  connect(&tT, SIGNAL(timeout()), &qE, SLOT(quit()));
  connect(this, SIGNAL(pageApproved()), &qE, SLOT(quit()));
  tT.start(LOGIN_WAIT_TIMEOUT);
  qE.exec();
}

void LoginPage::loadFinished(const bool &finished)
{
  if (finished && approvalView->url().toString().contains(approvalPageName)) {
    qDebug() << approvalView->url().toString();
    qDebug() << approvalView->title();
    authorizationCode = approvalView->title().remove(0, 13);
    qDebug() << authorizationCode;
    approved = true;
    Q_EMIT pageApproved();
  }
}

void LoginPage::raiseError(Errors::AuthorizationError errorType, const QString &description)
{
  qDebug() << description;
  errorValue = errorType;
  errorStringValue = description;
  Q_EMIT finished();
}

QString LoginPage::errorString()
{
  return errorStringValue;
}

Errors::AuthorizationError LoginPage::error()
{
  return errorValue;
}
