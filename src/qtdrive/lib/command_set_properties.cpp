#include "command_set_properties.h"

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

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandSetFilePropertiesPrivate : public CommandPrivate
{
public:
QMap<QString,QString> fields;
FileInfo fileInfo;

private:
QString currentKey;




};

CommandSetFileProperties::CommandSetFileProperties(Session *session)
    : AuthorizedCommand(new CommandSetFilePropertiesPrivate, session)
{
}


/**
 * @brief CommandSetFileProperties::exec - main entry point to run set custom fields command
 * @param fileInfo - object of google drive file we want to modify
 * @param fields - key/value single property to set
 */
void CommandSetFileProperties::exec(const FileInfo& fileInfo,const QPair<QString,QString>& field)
{
    Q_D(CommandSetFileProperties);
    d->fileInfo = fileInfo;
    d->fields[field.first]=field.second;
    executeQuery();
}

void CommandSetFileProperties::exec(const FileInfo& fileInfo,const QMap<QString,QString>& fields)
{
    Q_D(CommandSetFileProperties);
    d->fileInfo = fileInfo;
    d->fields = fields;
    executeQuery();
}



void CommandSetFileProperties::queryFailed(QNetworkReply::NetworkError error){
qDebug() << QString("NetworkError::"+error);
}




void CommandSetFileProperties::queryFinished()
{
    Q_D(CommandSetFileProperties);

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

    QString reason = reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ).toString();
    if (reason!="OK") {
            qDebug() <<"Set File properties response  returned error "+reason;
            emitError(UnknownError, tr("Set File properties response returned error"));
            return;



}
    QNetworkRequest req=reply->request();
    qDebug()<< req.url();
    bool ok;
    QVariant res = QJson::Parser().parse(reply, &ok);
    QMap<QString,QVariant> responseMap=res.toMap();

    if (!ok)
    {
        emitError(UnknownError, tr("Set File properties response parse error"));
        return;
    }
    if (!(responseMap.contains(KEY_STRING)&&responseMap[KEY_STRING].toString()==currentKey && responseMap[VALUE_STRING]==d->fields[currentKey])) {
    qDebug() << "Failed to create property "+currentKey+"="+d->fields[currentKey];
    emitError(UnknownError, tr("Set File properties response has error.No property created"));
    return;
    }


    emitSuccess();
}

void CommandSetFileProperties::reexecuteQuery()
{
    Q_D(CommandSetFileProperties);


    QString urlStr("https://www.googleapis.com/drive/v2/files");
    //TODO just crash here is file not found!!!
    if (!d->fileInfo.id().isEmpty())
        urlStr += QString("/%1/properties").arg(d->fileInfo.id());
    QUrl url(urlStr);
    QNetworkRequest request(url);
    setRequestAccessToken(request, session()->accessToken());

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    Q_FOREACH (const QString& key,d->fields.keys()) {
        currentKey=key;
        QVariantMap fileInfoMap;
        fileInfoMap.insert(KEY_STRING,key);
        fileInfoMap.insert(VALUE_STRING,d->fields[key]);
        fileInfoMap.insert(VISIBILITY_STRING,"PUBLIC");
        QByteArray body = QJson::Serializer().serialize(fileInfoMap);

        QNetworkReply* reply = session()->networkManager()->post(request, body);


        connect(reply, SIGNAL(finished()), this, SLOT(queryFinished()));


        connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(queryFailed(QNetworkReply::NetworkError)));

        reply->setParent(this);
    }




}

}
