#include "GoogleDriveCredentials.h"
const QString CLIENT_ID_VALUE     = "521376217688.apps.googleusercontent.com";
const QString CLIENT_SECRET_VALUE = "SXF0r3tMchlKw1WXD6rzZldJ";

GoogleDriveCredentials::GoogleDriveCredentials()
{}

void GoogleDriveCredentials::init() {
  creds->insert(CLIENT_ID, CLIENT_ID_VALUE);
  creds->insert(CLIENT_SECRET, CLIENT_SECRET_VALUE);
  QString refreshToken =  config()->get("cloud/refresh_token").toString();
  creds->insert(REFRESH_TOKEN, refreshToken);
}

void GoogleDriveCredentials::update() {
  LoginPage loginPage;
  Session   tempSession;
  loginPage.updateCredentials(tempSession);
  loginPage.error() != cmd.NoError ?  raiseError(
    Errors::AuthorizationError::GENERAL_AUTH_PROBLEM,
    QString("Failed to get access token")) : void();
  qDebug() << "refresh = " + tempSession.refreshToken();
  tempSession.refreshToken().isEmpty() ?  raiseError(
    Errors::AuthorizationError::GENERAL_AUTH_PROBLEM,
    QString("refresh token is empty")) : void();
  creds->insert(REFRESH_TOKEN, refreshToken);
  config()->set("cloud/refresh_token", session->refreshToken());
}

void GoogleDriveCredentials::raiseError(Errors::AuthorizationError errorType,
                                        const QString            & description) {
  qDebug() << description;
  errorValue       = errorType;
  errorStringValue = description;
  Q_EMIT finished();
}

AuthCredentials * GoogleDriveCredentials::newInstance() {
  return new GoogleDriveCredentials();
}
