#include "session.h"

#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QTime>

namespace GoogleDrive
{

class SessionPrivate
{
public:
    QPointer<QNetworkAccessManager> manager;
    QString clientId;
    QString clientSecret;
    QString refreshToken;
    QString accessToken;
    int accessTokenExpiresIn;
    QTime accessTokenExpiresTime;
};

Session::Session(QNetworkAccessManager* manager, QObject* parent)
    : QObject(parent)
    , d_ptr(new SessionPrivate)
{
    Q_D(Session);
    d->manager = manager;
}

Session::~Session()
{
    qDebug() << " delete session object";
    delete d_ptr;
    qDebug() << "session object was deleted";
}

QNetworkAccessManager *Session::networkManager() const
{
    Q_D(const Session);
    return d->manager;
}

QString Session::clientId() const
{
    Q_D(const Session);
    return d->clientId;
}

void Session::setClientId(const QString &v)
{
    Q_D(Session);
    d->clientId = v;
}

QString Session::clientSecret() const
{
    Q_D(const Session);
    return d->clientSecret;
}

void Session::setClientSecret(const QString &v)
{
    Q_D(Session);
    d->clientSecret = v;
}

QString Session::accessToken() const
{
    Q_D(const Session);

    if (d->accessTokenExpiresTime.elapsed() < d->accessTokenExpiresIn * 100)
        return d->accessToken;
    else
        return QString(); // expired
}

int Session::accessTokenExpiresIn() const
{
    Q_D(const Session);
    return d->accessTokenExpiresIn;
}

void Session::setAccessToken(const QString &v)
{
    Q_D(Session);
    d->accessToken = v;
}

void Session::setAccessTokenExpiresIn(int v)
{
    Q_D(Session);
    d->accessTokenExpiresIn = v;
    d->accessTokenExpiresTime.start();
}

QString Session::refreshToken() const
{
    Q_D(const Session);
    return d->refreshToken;
}

void Session::setRefreshToken(const QString &v)
{
    Q_D(Session);
    d->refreshToken = v;
}

}
