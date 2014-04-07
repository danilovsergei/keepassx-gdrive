#include "command_delete.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "command_private.h"
#include "tools.h"
#include "session.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandDeletePrivate : public CommandPrivate
{
public:
    QString id;
};

CommandDelete::CommandDelete(Session *session)
    : AuthorizedCommand(new CommandDeletePrivate, session)
{
}

void CommandDelete::exec(const QString &id)
{
    Q_D(CommandDelete);
    d->id = id;
    executeQuery();
}

void CommandDelete::reexecuteQuery()
{
    Q_D(CommandDelete);
    QUrl url(QString("https://www.googleapis.com/drive/v2/files/%1").arg(d->id));
    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    QNetworkReply* reply = session()->networkManager()->deleteResource(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    reply->setParent(this);
}

void CommandDelete::queryFinished()
{
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();
    if (checkInvalidReplyAndRefreshToken(reply))
        return;
    if (!reply->readAll().isEmpty())
    {
        emitError(UnknownError, tr("Delete query returns non empty reply body"));
        return;
    }

    emitSuccess();
}

}
