#include "command_refresh_token.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "session.h"

namespace GoogleDrive
{

CommandRefreshToken::CommandRefreshToken(Session* session)
    : Command(session)
{
}

void CommandRefreshToken::exec()
{
    const QString refreshToken = session()->refreshToken();
    const QString clientId = session()->clientId();
    const QString clientSecret = session()->clientSecret();

    if (refreshToken.isEmpty())
    {
        emitError(UnknownError, tr("Refresh token is empty"));
        return;
    }
    if (clientId.isEmpty())
    {
        emitError(UnknownError, tr("Client ID is empty"));
        return;
    }
    if (clientSecret.isEmpty())
    {
        emitError(UnknownError, tr("Client secret is empty"));
        return;
    }

    QNetworkRequest request(QUrl("https://accounts.google.com/o/oauth2/token"));

    QString str = QString ("refresh_token=%1&client_id=%2&client_secret=%3&grant_type=%4")
            .arg(refreshToken)
            .arg(clientId)
            .arg(clientSecret)
            .arg("refresh_token");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = session()->networkManager()->post(request, str.toUtf8());
    connect(reply, SIGNAL(finished()), SLOT(queryFinished()));
    reply->setParent(this);
}

void CommandRefreshToken::queryFinished()
{
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReply(reply))
        return;

    QVariantMap map;
    if (!parseJsonReply(reply, map))
        return;

    QString accessToken = map["access_token"].toString();
    if (accessToken.isEmpty())
    {
        emitError(UnknownError, tr("Empty access token in reply"));
        return;
    }

    int expiresIn = map.value("expires_in").toInt();
    QString token_type = map.value("token_type").toString();
    if (token_type != "Bearer")
    {
        qWarning() << "Unsupported token type" << token_type;
    }

    session()->setAccessToken(accessToken);
    session()->setAccessTokenExpiresIn(expiresIn);

    emitSuccess();
}

}
