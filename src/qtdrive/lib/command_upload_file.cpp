#include "command_upload_file.h"

#include <QIODevice>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHttpMultiPart>
#include <QtDebug>
#include <qjson/serializer.h>
#include <qjson/parser.h>

#include "session.h"
#include "tools.h"
#include "command_private.h"

using namespace GoogleDrive::Internal;

namespace GoogleDrive
{

class CommandUploadFilePrivate : public CommandPrivate
{
public:
    CommandUploadFilePrivate()
        : uploadType(CommandUploadFile::Multipart)
        , fileData(0)
    {
    }

    CommandUploadFile::UploadType uploadType;
    FileInfo fileInfo;
    FileInfo resultFileInfo;
    QIODevice* fileData;
};

CommandUploadFile::CommandUploadFile(Session* session)
    : AuthorizedCommand(new CommandUploadFilePrivate, session)
{
}

CommandUploadFile::UploadType CommandUploadFile::uploadType() const
{
    Q_D(const CommandUploadFile);
    return d->uploadType;
}

void CommandUploadFile::setUploadType(CommandUploadFile::UploadType val)
{
    Q_D(CommandUploadFile);
    d->uploadType = val;
}

const FileInfo &CommandUploadFile::resultFileInfo() const
{
    Q_D(const CommandUploadFile);
    return d->resultFileInfo;
}

void CommandUploadFile::exec(const FileInfo &fileInfo, QIODevice* fileData)
{
    Q_D(CommandUploadFile);

    d->fileInfo = fileInfo;
    d->fileData = fileData;

    executeQuery();
}

void CommandUploadFile::reexecuteQuery()
{
    Q_D(CommandUploadFile);

    switch (d->uploadType)
    {
        case Multipart: multipartUpload(); break;
        case Resumable:
        {
            Q_ASSERT(false && "Not Implemented");
        }
    }
}

void CommandUploadFile::requestFinished()
{
    Q_D(CommandUploadFile);
    //close the file opened during multipartUpload
    if (d->fileData->isOpen())
      d->fileData->close();



    //tryAutoDelete();
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    if (checkInvalidReplyAndRefreshToken(reply))
        return;

    bool ok;
    qDebug()<< reply->request().url();
    QVariant res = QJson::Parser().parse(reply, &ok);
    if (!ok)
    {
        emitError(UnknownError, tr("File info parse error"));
        return;
    }
    FileInfo fileInfo(res.toMap());
    d->resultFileInfo = fileInfo;

    emitSuccess();
}

void CommandUploadFile::multipartUpload()
{
    Q_D(CommandUploadFile);

    // prepare url
    QString urlStr("https://www.googleapis.com/upload/drive/v2/files");
    if (!d->fileInfo.id().isEmpty())
        urlStr += QString("/%1").arg(d->fileInfo.id());

    QUrl url(urlStr);
    url.addQueryItem("uploadType", "multipart");

    // prepare multipart body
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::RelatedType);

    QHttpPart metaPart;
    metaPart.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    QVariantMap fileInfoMap = d->fileInfo.rawData();
    QByteArray ba = QJson::Serializer().serialize(fileInfoMap);
    metaPart.setBody(ba);
    QHttpPart dataPart;
    dataPart.setBodyDevice(d->fileData);
    multiPart->append(metaPart);
    multiPart->append(dataPart);

    QNetworkRequest request( url );
    setRequestAccessToken(request, session()->accessToken());


    bool b = d->fileData->open(QIODevice::ReadOnly);

    if (!b) {
        qDebug() <<"Can't open file for reading";
        return;
    }

    // send request
    QNetworkReply* reply = session()->networkManager()->post(request, multiPart);
    multiPart->setParent(reply);
    connect(reply, SIGNAL(finished()), SLOT(requestFinished()));
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(progress(qint64,qint64)));
    reply->setParent(this);
}

}
