/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2010-2012  Oleg Linkin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#include "drive_manager.h"

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QtDebug>
#include <QFileInfo>
//#include <util/util.h>
#include <qjson/parser.h>
#include <qjson/serializer.h>
//#include "account.h"
//#include "core.h"

namespace GoogleDrive
{
DriveManager::DriveManager (Account *acc, QObject *parent)
    : QObject (parent)
    , directoryId_ ("application/vnd.google-apps.folder")
    , account_ (acc)
    , manager_()
{
#ifdef HAVE_MAGIC
    Magic_ = magic_open (MAGIC_MIME_TYPE);
    magic_load (Magic_, NULL);
#endif
}

DriveManager::~DriveManager ()
{
#ifdef HAVE_MAGIC
    magic_close (Magic_);
#endif
}

void DriveManager::refreshListing ()
{
    apiCallQueue_ << boost::bind(&DriveManager::requestFiles, this, _1);
    //[this] (const QString& key) { RequestFiles (key); };
    requestAccessToken ();
}

void DriveManager::removeEntry (const QString& id)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestEntryRemoving, this, id, _1);
    //[this, id] (const QString& key) { RequestEntryRemoving (id, key); };
    requestAccessToken ();
}

void DriveManager::moveEntryToTrash (const QString& id)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestMovingEntryToTrash, this, id, _1);
    //[this, id] (const QString& key) { RequestMovingEntryToTrash (id, key); };
    requestAccessToken ();
}

void DriveManager::restoreEntryFromTrash (const QString& id)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestRestoreEntryFromTrash, this, id, _1);
    //[this, id] (const QString& key) { RequestRestoreEntryFromTrash (id, key); };
    requestAccessToken ();
}

void DriveManager::shareEntry (const QString& id)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestSharingEntry, this, id, _1);
    //[this, id] (const QString& key) { RequestSharingEntry (id, key); };
    requestAccessToken ();
}

void DriveManager::upload (const QString& filePath, const QStringList& parentId)
{
    QString parent = parentId.value (0);
    apiCallQueue_ << boost::bind(&DriveManager::requestUpload, this, filePath, parent, _1);
    //[this, filePath, parent] (const QString& key) { RequestUpload (filePath, parent, key); };
    requestAccessToken ();
}

void DriveManager::createDirectory (const QString& name, const QString& parentId)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestCreateDirectory, this, name, parentId, _1);
    //[this, name, parentId] (const QString& key) { RequestCreateDirectory (name, parentId, key); };
    requestAccessToken ();
}

void DriveManager::copy (const QString& id, const QString& parentId)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestCopyItem, this, id, parentId, _1);
    //		ApiCallQueue_ << [this, id, parentId] (const QString& key) { RequestCopyItem (id, parentId, key); };
    requestAccessToken ();
}

void DriveManager::move (const QString& id, const QString& parentId)
{
    apiCallQueue_ << boost::bind(&DriveManager::requestMoveItem, this, id, parentId, _1);
    //		ApiCallQueue_ << [this, id, parentId] (const QString& key) { RequestMoveItem (id, parentId, key); };
    requestAccessToken ();
}

void DriveManager::requestFiles (const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files?access_token=%1")
            .arg (key);
    QNetworkRequest request (str);

    request.setHeader (QNetworkRequest::ContentTypeHeader,
                       "application/x-www-form-urlencoded");

    QNetworkReply *reply = manager_->get(request);

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleGotFiles ()));
}

void DriveManager::requestSharingEntry (const QString& id,
                                        const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1/permissions?access_token=%2")
            .arg (id, key);
    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");

    QVariantMap map;
    map.insert ("kind", "drive#permission");
    map.insert ("id", "anyoneWithLink");
    map.insert ("role", "reader");
    map.insert ("type", "anyone");
    map.insert ("withLink", true);

    QJson::Serializer serializer;

    QNetworkReply *reply = manager_->post(request, serializer.serialize (map));
    reply2Id_ [reply] = id;

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleRequestFileSharing ()));
}

void DriveManager::requestEntryRemoving (const QString& id,
                                         const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1?access_token=%2")
            .arg (id, key);
    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager_->deleteResource (request);

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleRequestEntryRemoving ()));
}

void DriveManager::requestMovingEntryToTrash (const QString& id,
                                              const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1/trash?access_token=%2")
            .arg (id, key);
    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager_->post (request, QByteArray ());

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleRequestMovingEntryToTrash ()));
}

void DriveManager::requestRestoreEntryFromTrash (const QString& id,
                                                 const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1/untrash?access_token=%2")
            .arg (id, key);
    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager_->post (request, QByteArray ());

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleRequestRestoreEntryFromTrash ()));
}

void DriveManager::requestAccessToken ()
{
    QNetworkRequest request (QUrl ("https://accounts.google.com/o/oauth2/token"));

    // TODO: needed to realize
    //        QString refreshToken = Account_->GetRefreshToken ();
    QString refreshToken;

    QString str = QString ("refresh_token=%1&client_id=%2&client_secret=%3&grant_type=%4")
            .arg (refreshToken)
            .arg ("844868161425.apps.googleusercontent.com")
            .arg ("l09HkM6nbPMEYcMdcdeGBdaV")
            .arg ("refresh_token");

    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = manager_->post (request, str.toUtf8 ());

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleAuthTokenRequestFinished ()));
}

void DriveManager::requestUpload (const QString& filePath,
                                  const QString& parent, const QString& key)
{
    Q_EMIT uploadStatusChanged (tr ("Initializing..."), filePath);

    QFileInfo info (filePath);
    const QUrl initiateUrl (QString ("https://www.googleapis.com/upload/drive/v2/files?access_token=%1&uploadType=resumable")
                            .arg (key));
    QNetworkRequest request (initiateUrl);
    request.setPriority (QNetworkRequest::LowPriority);
#ifdef HAVE_MAGIC
    request.setRawHeader ("X-Upload-Content-Type",
                          magic_file (Magic_, filePath.toUtf8 ()));
#endif
    request.setRawHeader ("X-Upload-Content-Length",
                          QString::number (QFileInfo (filePath).size ()).toUtf8 ());
    QVariantMap map;
    map ["title"] = QFileInfo (filePath).fileName ();
    if (!parent.isEmpty ())
    {
        QVariantList parents;
        QVariantMap parentMap;
        parentMap ["id"] = parent;
        parents << parentMap;
        map ["parents"] = parents;
    }

    const QByteArray& data = QJson::Serializer ().serialize (map);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader (QNetworkRequest::ContentLengthHeader, data.size ());

    QNetworkReply *reply = manager_->post(request, data);
    reply2FilePath_ [reply] = filePath;

    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleUploadRequestFinished ()));
}

void DriveManager::requestCreateDirectory (const QString& name,
                                           const QString& parentId, const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files?access_token=%1")
            .arg (key);

    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");
    QVariantMap data;
    data ["title"] = name;
    data ["mimeType"] = directoryId_;
    if (!parentId.isEmpty ())
    {
        QVariantList parents;
        QVariantMap parent;
        parent ["id"] = parentId;
        parents << parent;
        data ["parents"] = parents;
    }

    QNetworkReply *reply = manager_->post (request, QJson::Serializer ().serialize (data));
    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleCreateDirectory ()));
}

void DriveManager::requestCopyItem (const QString& id,
                                    const QString& parentId, const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1/copy?access_token=%2")
            .arg (id)
            .arg (key);

    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");
    QVariantMap data;
    if (!parentId.isEmpty ())
    {
        QVariantList parents;
        QVariantMap parent;
        parent ["id"] = parentId;
        parents << parent;
        data ["parents"] = parents;
    }

    QNetworkReply *reply = manager_->post (request, QJson::Serializer ().serialize (data));
    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleCopyItem ()));
}

void DriveManager::requestMoveItem (const QString& id,
                                    const QString& parentId, const QString& key)
{
    QString str = QString ("https://www.googleapis.com/drive/v2/files/%1?access_token=%2")
            .arg (id)
            .arg (key);

    QNetworkRequest request (str);
    request.setHeader (QNetworkRequest::ContentTypeHeader, "application/json");
    QVariantMap data;
    if (!parentId.isEmpty ())
    {
        QVariantList parents;
        QVariantMap parent;
        parent ["id"] = parentId;
        parents << parent;
        data ["parents"] = parents;
    }

    QNetworkReply *reply = manager_->put (request, QJson::Serializer ().serialize (data));
    connect (reply,
             SIGNAL (finished ()),
             this,
             SLOT (handleMoveItem ()));
}

void DriveManager::parseError (const QVariantMap& map)
{
    const QVariantMap& errorMap = map ["error"].toMap ();
    const QString& code = errorMap ["code"].toString ();
    QString msg = errorMap ["message"].toString ();

    if (code == "500")
        msg = tr ("Google Drive API v.2 doesn't support directory copying.");


    // TODO: need realization
    //		Core::Instance ().SendEntity (Util::MakeNotification ("NetStoreManager",
    //				msg,
    //				PWarning_));
}

void DriveManager::handleAuthTokenRequestFinished ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QByteArray ba = reply->readAll ();
    QVariant res = QJson::Parser ().parse (ba, &ok);

    if (!ok)
    {
        qDebug () << Q_FUNC_INFO << "parse error";
        return;
    }

    QString accessKey = res.toMap ().value ("access_token").toString ();
    qDebug () << accessKey;
    if (accessKey.isEmpty ())
    {
        qDebug () << Q_FUNC_INFO << "access token is empty";
        return;
    }

    if (apiCallQueue_.isEmpty ())
        return;

    apiCallQueue_.dequeue () (accessKey);
}

namespace
{
DriveItem CreateDriveItem (const QVariant& itemData)
{
    QVariantMap map = itemData.toMap ();

    const QVariantMap& permission = map ["userPermission"].toMap ();
    const QString& role = permission ["role"].toString ();

    if (role != "owner")
        return DriveItem ();

    DriveItem driveItem;

    const QString& type = permission ["type"].toString ();

    driveItem.permissionAdditionalRole = DriveItem::ARNone;
    if (permission ["additionalRoles"].toList ().contains ("commenter"))
        driveItem.permissionAdditionalRole |= DriveItem::ARCommenter;

    if (role == "owner")
        driveItem.permissionRole = DriveItem::OwnerRole;
    else if (role == "writer")
        driveItem.permissionRole = DriveItem::WriterRole;
    else if (role == "reader")
        driveItem.permissionRole = DriveItem::ReaderRole;

    if (type == "user")
        driveItem.permissionType = DriveItem::UserPerm;

    driveItem.id = map ["id"].toString ();
    driveItem.name = map ["title"].toString ();
    driveItem.isFolder = map ["mimeType"].toString () ==
            "application/vnd.google-apps.folder";
    driveItem.mime = map ["mimeType"].toString ();

    driveItem.downloadUrl = map ["downloadUrl"].toUrl ();

    const QVariantMap& labels = map ["labels"].toMap ();
    driveItem.labels = DriveItem::ILNone;
    if (labels ["starred"].toBool ())
        driveItem.labels |= DriveItem::ILFavorite;
    if (labels ["hidden"].toBool ())
        driveItem.labels |= DriveItem::ILHidden;
    if (labels ["trashed"].toBool ())
        driveItem.labels |= DriveItem::ILRemoved;
    if (labels ["restricted"].toBool ())
        driveItem.labels |= DriveItem::ILShared;
    if (labels ["viewed"].toBool ())
        driveItem.labels |= DriveItem::ILViewed;
    driveItem.createDate = QDateTime::fromString (map ["createdDate"].toString (),
                                                  Qt::ISODate);
    driveItem.modifiedDate = QDateTime::fromString (map ["modifiedDate"].toString (),
                                                    Qt::ISODate);
    driveItem.lastViewedByMe = QDateTime::fromString (map ["lastViewedByMeDate"].toString (),
                                                      Qt::ISODate);

    driveItem.originalFileName = map ["originalFilename"].toString ();
    driveItem.md5 = map ["md5Checksum"].toString ();
    driveItem.fileSize = map ["fileSize"].toLongLong ();

    Q_FOREACH (const QVariant& ownerName, map ["ownerNames"].toList ())
        driveItem.ownerNames << ownerName.toString ();

    driveItem.lastModifiedBy = map ["lastModifyingUserName"].toString ();
    driveItem.editable = map ["editable"].toBool ();
    driveItem.writersCanShare = map ["writersCanShare"].toBool ();

    const QVariantMap& parent = map ["parents"].toList ().value (0).toMap ();
    if (!parent.isEmpty ())
    {
        driveItem.parentId = parent ["id"].toString ();
        driveItem.parentIsRoot = parent ["isRoot"].toBool ();
    }

    return driveItem;
}
}

void DriveManager::handleGotFiles ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);

    if (!ok)
    {
        qDebug () << Q_FUNC_INFO << "parse error";
        return;
    }

    QVariantMap resMap = res.toMap ();
    if (!resMap.contains ("items"))
    {
        qDebug () << Q_FUNC_INFO << "there are no items";
        return;
    }

    if (resMap.contains ("error"))
    {
        parseError (res.toMap ());
        return;
    }

    QList<DriveItem> resList;
    Q_FOREACH (const QVariant& item, resMap ["items"].toList ())
    {
        DriveItem driveItem = CreateDriveItem (item);
        if (driveItem.name.isEmpty ())
            continue;
        resList << driveItem;
    }

    Q_EMIT gotFiles (resList);
}

void DriveManager::handleRequestFileSharing ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);

    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "file shared successfully";
        Q_EMIT gotSharedFileId (reply2Id_.take (reply));
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleRequestEntryRemoving ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "file removed successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleRequestMovingEntryToTrash ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "file moved to trash successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleRequestRestoreEntryFromTrash ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "file restored from trash successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleUploadRequestFinished ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();
    QString path = reply2FilePath_.take (reply);

    const int code = reply->
            attribute (QNetworkRequest::HttpStatusCodeAttribute).toInt ();
    if (code != 200)
    {
        qWarning () << Q_FUNC_INFO
                    << "upload initiating failed with code:"
                    << code;
        return;
    }

    Q_EMIT uploadStatusChanged (tr ("Uploading..."), path);

    QFile *file = new QFile (path);
    if (!file->open (QIODevice::ReadOnly))
    {
        qWarning () << Q_FUNC_INFO
                    << "unable to open file: "
                    << file->errorString ();
        return;
    }

    QUrl url (reply->rawHeader ("Location"));
    QNetworkRequest request (url);
#ifdef HAVE_MAGIC
    request.setHeader (QNetworkRequest::ContentTypeHeader,
                       magic_file (Magic_, path.toUtf8 ()));
#endif
    request.setHeader (QNetworkRequest::ContentLengthHeader,
                       QString::number (QFileInfo (path).size ()).toUtf8 ());

    QNetworkReply *uploadReply = manager_->put (request, file);
    file->setParent (uploadReply);
    reply2FilePath_ [uploadReply] = path;

    connect (uploadReply,
             SIGNAL (finished ()),
             this,
             SLOT (handleUploadFinished ()));
    connect (uploadReply,
             SIGNAL (error (QNetworkReply::NetworkError)),
             this,
             SLOT (handleUploadError (QNetworkReply::NetworkError)));
    connect (uploadReply,
             SIGNAL (uploadProgress (qint64, qint64)),
             this,
             SLOT (handleUploadProgress (qint64, qint64)));
}

void DriveManager::handleUploadFinished ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    QVariantMap map = res.toMap ();
    QString id = map ["id"].toString ();

    if (!map.contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "file uploaded successfully";
        refreshListing ();
        Q_EMIT finished (id, reply2FilePath_.take (reply));
        return;
    }

    parseError (map);
}

void DriveManager::handleUploadProgress (qint64 uploaded, qint64 total)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    Q_EMIT uploadProgress (uploaded, total, reply2FilePath_ [reply]);
}

void DriveManager::handleUploadError (QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    Q_EMIT uploadError ("Error", reply2FilePath_.take (reply));
    if (error == QNetworkReply::ProtocolUnknownError)
        ;//TODO resume upload
}

void DriveManager::handleCreateDirectory ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "directory created successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleCopyItem ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "entry copied successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

void DriveManager::handleMoveItem ()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*> (sender ());
    if (!reply)
        return;

    reply->deleteLater ();

    bool ok = false;
    QVariant res = QJson::Parser ().parse (reply->readAll (), &ok);
    if (!ok)
    {
        qDebug () << Q_FUNC_INFO
                  << "parse error";
        return;
    }

    if (!res.toMap ().contains ("error"))
    {
        qDebug () << Q_FUNC_INFO
                  << "entry moved successfully";
        refreshListing ();
        return;
    }

    parseError (res.toMap ());
}

}

