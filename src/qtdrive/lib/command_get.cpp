#include "command_get.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "command_private.h"
#include "tools.h"
#include "session.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandGetPrivate : public CommandPrivate
{
public:
    QString id;
    QString fields;
    FileInfo fileInfo;
};

CommandGet::CommandGet(Session *session)
    : AuthorizedCommand(new CommandGetPrivate, session)
{
}

QString CommandGet::fields() const
{
    Q_D(const CommandGet);
    return d->fields;
}

void CommandGet::setFields(const QString &v)
{
    Q_D(CommandGet);
    d->fields = v;
}

const FileInfo &CommandGet::resultFileInfo() const
{
    Q_D(const CommandGet);
    return d->fileInfo;
}

void CommandGet::exec(const QString &id)
{
    Q_D(CommandGet);
    d->id = id;
    executeQuery();
}

void CommandGet::reexecuteQuery()
{
    Q_D(CommandGet);
    QUrl url(QString("https://www.googleapis.com/drive/v2/files/%1").arg(d->id));
    if (!d->fields.isEmpty())
        url.addQueryItem("fields", d->fields);

    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    QNetworkReply* reply = session()->networkManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    reply->setParent(this);
}

void CommandGet::queryFinished()
{
    Q_D(CommandGet);
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    QVariantMap map;
    if (!parseJsonReply(reply, map))
        return;

    d->fileInfo = FileInfo(map);

    emitSuccess();
}

}
