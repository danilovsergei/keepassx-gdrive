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

#pragma once

#include <boost/function.hpp>
#include <QObject>
#include <QQueue>
#include <QDateTime>
#include <QUrl>
#include <QStringList>
#include <QVariant>
#include <QNetworkReply>
#ifdef HAVE_MAGIC
#include <magic.h>
#endif

class QFile;
class QNetworkAccessManager;

namespace GoogleDrive
{

class Account;

struct DriveItem
{
    enum ItemLabel
    {
        ILNone = 0x00,
        ILFavorite = 0x01,
        ILHidden = 0x02,
        ILRemoved = 0x04,
        ILShared = 0x08,
        ILViewed = 0x10
    };
    Q_DECLARE_FLAGS (ItemLabels, ItemLabel)

    enum Roles
    {
        OwnerRole,
        WriterRole,
        ReaderRole
    };

    enum AdditionalRole
    {
        ARNone = 0x00,
        ARCommenter = 0x01
    };
    Q_DECLARE_FLAGS (AdditionalRoles, AdditionalRole)

    enum PermissionTypes
    {
        UserPerm,
        GroupPerm,
        DomainPerm,
        AnyonePerm
    };

    QString id;

    QString parentId;
    bool parentIsRoot;

    QString name;
    QString originalFileName;
    QString md5;
    QString mime;

    qint64 fileSize;

    QStringList ownerNames;
    QString lastModifiedBy;

    bool editable;
    bool writersCanShare;

    bool isFolder;

    ItemLabels labels;

    QDateTime createDate;
    QDateTime modifiedDate;
    QDateTime lastViewedByMe;

    QUrl downloadUrl;

    Roles permissionRole;
    AdditionalRoles permissionAdditionalRole;
    PermissionTypes permissionType;
};

class DriveManager : public QObject
{
    Q_OBJECT
public:
    DriveManager (Account *acc, QObject *parent = 0);
    ~DriveManager ();

    void refreshListing ();
    void removeEntry (const QString& id);
    void moveEntryToTrash (const QString& id);
    void restoreEntryFromTrash (const QString& id);
    void shareEntry (const QString& id);
    void upload (const QString& filePath,
                 const QStringList& parentId = QStringList ());
    void createDirectory (const QString& name, const QString& parentId = QString ());
    void copy (const QString& id, const QString& parentId);
    void move (const QString& id, const QString& parentId);
private:
    void requestFiles (const QString& key);
    void requestSharingEntry (const QString& id, const QString& key);
    void requestEntryRemoving (const QString& id, const QString& key);
    void requestMovingEntryToTrash (const QString& id, const QString& key);
    void requestRestoreEntryFromTrash (const QString& id, const QString& key);
    void requestUpload (const QString& filePath, const QString& parent,
                        const QString& key);
    void requestCreateDirectory (const QString& name,
                                 const QString& parentId, const QString& key);
    void requestCopyItem (const QString& id,
                          const QString& parentId, const QString& key);
    void requestMoveItem (const QString& id,
                          const QString& parentId, const QString& key);
private:
    void requestAccessToken ();
    void parseError (const QVariantMap& map);

private Q_SLOTS:
    void handleAuthTokenRequestFinished ();
    void handleGotFiles ();
    void handleRequestFileSharing ();
    void handleRequestEntryRemoving ();
    void handleRequestMovingEntryToTrash ();
    void handleRequestRestoreEntryFromTrash ();
    void handleUploadRequestFinished ();
    void handleUploadFinished ();
    void handleUploadProgress (qint64 uploaded, qint64 total);
    void handleUploadError (QNetworkReply::NetworkError error);
    void handleCreateDirectory ();
    void handleCopyItem ();
    void handleMoveItem ();

Q_SIGNALS:
    void gotFiles (const QList<DriveItem>& items);
    void gotSharedFileId (const QString& id);
    void uploadProgress (qint64 sent, qint64 total, const QString& filePath);
    void uploadStatusChanged (const QString& status, const QString& filePath);
    void uploadError (const QString& str, const QString& filePath);
    void finished (const QString& id, const QString& path);

private:
    const QString directoryId_;
    Account *account_;
    QQueue<boost::function<void (const QString&)> > apiCallQueue_;
    QHash<QNetworkReply*, QString> reply2Id_;
    QHash<QNetworkReply*, QString> reply2FilePath_;
#ifdef HAVE_MAGIC
    magic_t Magic_;
#endif
    QNetworkAccessManager* manager_;
};

}
