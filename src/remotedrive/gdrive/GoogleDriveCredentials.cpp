#include "GoogleDriveCredentials.h"
GoogleDriveCredentials::GoogleDriveCredentials(QObject *parent) : AuthCredentials(parent)
{
}

void GoogleDriveCredentials::init()
{
    creds.insert(CLIENT_ID, CLIENT_ID_VALUE);
    creds.insert(CLIENT_SECRET, CLIENT_SECRET_VALUE);
    QString refreshToken = config()->get("cloud/refresh_token").toString();
    creds.insert(REFRESH_TOKEN, refreshToken);
}

void GoogleDriveCredentials::update()
{
    LoginPage loginPage;
    QScopedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager(this));
    Session tempSession(manager.data(), this);
    loginPage.updateCredentials(tempSession);
    qDebug() << "refresh = " + tempSession.refreshToken();
    tempSession.refreshToken().isEmpty() ? raiseError(
        Errors::AuthorizationError::GENERAL_AUTH_PROBLEM,
        QString("refresh token is empty")) : void();
    creds.insert(REFRESH_TOKEN, tempSession.refreshToken());
    config()->set("cloud/refresh_token", tempSession.refreshToken());
}

void GoogleDriveCredentials::raiseError(Errors::AuthorizationError errorType,
                                        const QString &description)
{
    qDebug() << errorType  <<description;
    Q_ASSERT(false);
}
