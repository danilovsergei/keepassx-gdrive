#include "command_get_properties.h"

#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <qjson/parser.h>

#include "session.h"
#include "tools.h"
#include "command_private.h"
#include <qjson/serializer.h>

const QString KEY_STRING="key";
const QString VALUE_STRING="value";
const QString VISIBILITY_STRING="visibility";
const QString ITEMS_STRING="items";

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandGetFilePropertiesPrivate : public CommandPrivate
{
public:
FileInfo fileInfo;
QMap<QString,QString> properties;



};

CommandGetFileProperties::CommandGetFileProperties(Session *session)
    : AuthorizedCommand(new CommandGetFilePropertiesPrivate, session)
{
}


/**
 * @brief CommandGetFileProperties::exec - main entry point to get custom properties
 * @param fileInfo - object of google drive file we want to get properties for
 */
void CommandGetFileProperties::exec(const FileInfo& fileInfo)
{
    Q_D(CommandGetFileProperties);
    d->fileInfo=fileInfo;
    executeQuery();
}


QMap<QString,QString> CommandGetFileProperties::getProperties() {
Q_D(const CommandGetFileProperties);
return d->properties;
}





void CommandGetFileProperties::queryFailed(QNetworkReply::NetworkError error){
qDebug() << QString("NetworkError::"+error);
}



void CommandGetFileProperties::queryFinished()
{
    Q_D(CommandGetFileProperties);
    tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        qDebug() << "There is no reply::"+reply->errorString();
        return;
    }

    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply)) {
        qDebug() << "There is invalid reply and refresh token::"+reply->errorString();
        return;
    }


    bool ok;
    qDebug()<< reply->request().url();
    QVariant res = QJson::Parser().parse(reply, &ok);
    if (!ok)
    {
        emitError(UnknownError, tr("Set File properties parse error"));
        return;
    }


    if (res.toMap().contains(ITEMS_STRING)) {
      QList<QVariant> items=res.toMap()[ITEMS_STRING].toList();
      Q_FOREACH (const QVariant & item,items) {
          QMap<QString,QVariant> itemMap=item.toMap();
          d->properties.insert(itemMap[KEY_STRING].toString(),itemMap[VALUE_STRING].toString());
      }

    }
    else {
     qDebug() <<"Response does not contain items object with properties";
     emitError(UnknownError, tr("Get properties error.No Items object returned"));
    }


    emitSuccess();
}

void CommandGetFileProperties::reexecuteQuery()
{
    Q_D(CommandGetFileProperties);
    qDebug() << "Get properties from File";

    QString urlStr("https://www.googleapis.com/drive/v2/files");
    if (!d->fileInfo.id().isEmpty())
        urlStr += QString("/%1/properties").arg(d->fileInfo.id());
    QUrl url(urlStr);
    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    QNetworkReply* reply = session()->networkManager()->get(request);


    connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));


    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(queryFailed(QNetworkReply::NetworkError)));

    reply->setParent(this);
}

}
