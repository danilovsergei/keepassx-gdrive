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
    {

    }

    FileInfo info;
    FileInfo resultInfo;
    //list of options fields which will be passed with post request url.
    QMap<QString,QString> urlFields;

    //list of options fields which will be passed in the post request body.
    QMap<QString,QString> bodyFields;


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

void CommandUpdate::exec(const FileInfo& info,const QMap<QString,QString>& urlFields,const QMap<QString,QString>& bodyFields) {
Q_D(CommandUpdate);
d->urlFields=urlFields;
d->bodyFields=bodyFields;
CommandUpdate::exec(info);
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
    Q_FOREACH(QString key,d->urlFields.keys()) {
    url.addQueryItem(key,d->urlFields[key]);
    }

    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    Q_FOREACH(QString key,d->bodyFields.keys()) {
    d->info.rawData()[key]=d->bodyFields[key];
    qDebug()<< QString("Add to body:%1=%2").arg(key,d->bodyFields[key]);
    }
    qDebug()<< "pass url: "+url.toString();
    QVariantMap fileInfoMap = d->info.rawData();
    QByteArray data = QJson::Serializer().serialize(fileInfoMap);

    QNetworkReply* reply = session()->networkManager()->put(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    reply->setParent(this);
}

}
