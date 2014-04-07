#include "command_update.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qjson/parser.h>
#include <qjson/serializer.h>

#include "session.h"
#include "tools.h"
#include "command_private.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandUpdatePrivate : public CommandPrivate
{
public:
    CommandUpdatePrivate()
        : convert(false)
    {}

    FileInfo info;
    FileInfo resultInfo;
    bool convert;
};

CommandUpdate::CommandUpdate(Session *session)
    : AuthorizedCommand(new CommandUpdatePrivate, session)
{
}

FileInfo CommandUpdate::resultFileInfo() const
{
    Q_D(const CommandUpdate);
    return d->resultInfo;
}

bool CommandUpdate::convert() const
{
    Q_D(const CommandUpdate);
    return d->convert;
}

void CommandUpdate::setConvert(bool v)
{
    Q_D(CommandUpdate);
    d->convert = v;
}

void CommandUpdate::exec(const FileInfo& info)
{
    Q_D(CommandUpdate);

    if (info.id().isEmpty())
    {
        emitError(UnknownError, tr("Valid file ID is required"));
        return;
    }

    d->info = info;
    executeQuery();
}

void CommandUpdate::queryFinished()
{
    Q_D(CommandUpdate);
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    bool ok;
    QVariant res = QJson::Parser().parse(reply, &ok);
    if (!ok)
    {
        emitError(UnknownError, tr("File info parse error"));
        return;
    }
    FileInfo fileInfo(res.toMap());
    d->resultInfo = fileInfo;

    emitSuccess();
}

void CommandUpdate::reexecuteQuery()
{
    Q_D(CommandUpdate);

    // prepare url
    QString urlStr = QString("https://www.googleapis.com/drive/v2/files/%1").arg(d->info.id());
    QUrl url(urlStr);
    if (d->convert)
        url.addQueryItem("convert", "true");

    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    QVariantMap fileInfoMap = d->info.rawData();
    QByteArray data = QJson::Serializer().serialize(fileInfoMap);

    QNetworkReply* reply = session()->networkManager()->put(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    reply->setParent(this);
}

}
