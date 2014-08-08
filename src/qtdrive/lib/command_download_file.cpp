#include "command_download_file.h"

#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QByteArray>
#include <QPointer>

#include "session.h"
#include "defines.h"
#include "tools.h"
#include "command_private.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandDownloadFilePrivate : public CommandPrivate
{
public:
    CommandDownloadFilePrivate()
        : bufferSize(16*1024)
    {}

    qint64 bufferSize;
    QUrl downloadUrl;
    QPointer<QIODevice> out;
};

CommandDownloadFile::CommandDownloadFile(Session* session)
    : AuthorizedCommand(new CommandDownloadFilePrivate, session)
{
}


qint64 CommandDownloadFile::bufferSize() const
{
    Q_D(const CommandDownloadFile);
    return d->bufferSize;
}

void CommandDownloadFile::setBufferSize(qint64 v)
{
    Q_D(CommandDownloadFile);
    d->bufferSize = v;
}

void CommandDownloadFile::exec(const QUrl &downloadUrl, QIODevice *out)
{
    Q_D(CommandDownloadFile);
    d->downloadUrl = downloadUrl;
    d->out = out;
    executeQuery();
}

void CommandDownloadFile::reexecuteQuery()
{
    Q_D(CommandDownloadFile);
    QNetworkRequest request( d->downloadUrl );
    setRequestAccessToken(request, session()->accessToken());
    QNetworkReply* reply = session()->networkManager()->get(request);
    reply->setReadBufferSize(d->bufferSize);
    connect(reply, SIGNAL(finished()), SLOT(requestFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
    reply->setParent(this);
    qDebug() <<"download function finished";
}

void CommandDownloadFile::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

    if (reply->error() == QNetworkReply::AuthenticationRequiredError)
    {
        refreshToken();
        return;
    }

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    emitSuccess();
}

void CommandDownloadFile::readyRead()
{
    Q_D(CommandDownloadFile);

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (!reply)
        return;

    qint64 sz = reply->bytesAvailable();
    QByteArray ba = reply->read(sz);

    QIODevice* out = d->out;
    out->write(ba);
}

}
