#include "command_oauth2.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QByteArray>
#include <QStringList>
#include <QtDebug>

#include <qjson/parser.h>

#include "session.h"
#include "command_private.h"

namespace GoogleDrive
{

class CommandOAuth2Private : public CommandPrivate
{
public:
    CommandOAuth2Private()
    {}

    QString scope;

};

CommandOAuth2::CommandOAuth2(Session* s)
    : Command(new CommandOAuth2Private, s)
{
}

QString CommandOAuth2::scope() const
{
    Q_D(const CommandOAuth2);
    return d->scope;
}

void CommandOAuth2::setScope(const QString &v)
{
    Q_D(CommandOAuth2);
    d->scope = v;
}

void CommandOAuth2::setScope(AccessScopes scopes)
{
    Q_D(CommandOAuth2);

    QStringList sl;
    const QString prefix("https://www.googleapis.com/auth/drive");

    if (scopes.testFlag(FileScope))
        sl << prefix + ".file";

    if (scopes.testFlag(AppsReadonyScope))
        sl << prefix + ".apps.readonly";

    if (scopes.testFlag(ReadonlyScope))
        sl << prefix + ".readonly";

    if (scopes.testFlag(ReadonlyMetadataScope))
        sl << prefix + ".readonly.metadata";

    if (scopes.testFlag(FullAccessScope))
        sl << prefix;

    d->scope = sl.join(" ");
}

QUrl CommandOAuth2::getLoginUrl() const
{
    Q_D(const CommandOAuth2);

    Q_ASSERT(!d->scope.isEmpty());
    if (d->scope.isEmpty())
    {
        qCritical() << "Scope isn't specified";
        return QUrl();
    }

    Q_ASSERT(!session()->clientId().isEmpty());
    if (session()->clientId().isEmpty())
    {
        qCritical() << "ClientID isn't specified";
        return QUrl();
    }

    QUrl url("https://accounts.google.com/o/oauth2/auth");
    url.addQueryItem("response_type", "code");
    url.addQueryItem("client_id", session()->clientId());
    url.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
    url.addQueryItem("scope", d->scope);

    return url;
}

void CommandOAuth2::requestAccessToken(const QString &authCode)
{
    emitStarted();
    QNetworkRequest request(QUrl("https://accounts.google.com/o/oauth2/token"));
    QString str = QString("code=%1&client_id=%2&client_secret=%3&grant_type=%4&redirect_uri=%5")
            .arg(authCode)
            .arg(session()->clientId())
            .arg(session()->clientSecret())
            .arg("authorization_code")
            .arg("urn:ietf:wg:oauth:2.0:oob");

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply* reply = session()->networkManager()->post(request, str.toUtf8());
    connect (reply, SIGNAL(finished()), this, SLOT(requestAccessTokenFinished()));
    reply->setParent(this);
}

void CommandOAuth2::requestAccessTokenFinished()
{
    Q_D(CommandOAuth2);

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

    QString accessToken = map.value("access_token").toString();
    QString refreshToken = map.value("refresh_token").toString();
    if (accessToken.isEmpty() || refreshToken.isEmpty())
    {
        emitError(UnknownError, tr("Access or refresh token is empty in reply to \"%1\"")
                  .arg(reply->url().toString()));
        return;
    }

    QString tokenType = map.value("token_type").toString();
    if (tokenType != "Bearer")
    {
        qWarning() << "Unsupported token type: " << tokenType;
    }

    session()->setRefreshToken(refreshToken);
    session()->setAccessToken(accessToken);
    session()->setAccessTokenExpiresIn(map.value("expires_in").toInt());

    emitSuccess();
}

}
