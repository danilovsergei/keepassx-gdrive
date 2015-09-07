#include "file_info.h"

#include <QUrl>
#include <map>
#include <QtDebug>

namespace GoogleDrive
{

namespace
{
const char* cId = "id";
const char* cTitle = "title";
const char* cParents = "parents";
}

struct FileInfo::Impl
{
    Impl(const QVariantMap& d)
        : data(d)
    {
    }

    QVariantMap data;
};

FileInfo::FileInfo()
    : d(new Impl(QVariantMap()))
{
}

FileInfo::FileInfo(const QVariantMap& data)
    : d(new Impl(data))
{
}

FileInfo::FileInfo(const FileInfo &src)
    : d(new Impl(src.d->data))
{
}

FileInfo::~FileInfo()
{
    delete d;
}

FileInfo &FileInfo::operator =(const FileInfo &src)
{
    d->data = src.d->data;
}

bool FileInfo::isEmpty() const
{
    return d->data.isEmpty();
}

const QVariantMap& FileInfo::rawData() const
{
    return d->data;
}

QVariantMap& FileInfo::rawData()
{
    return d->data;
}


QString FileInfo::id() const
{
    return d->data.value(cId).toString();
}

void FileInfo::setId(const QString& id)
{
    d->data[cId] = id;
}

QString FileInfo::title() const
{
    return d->data.value(cTitle).toString();
}

void FileInfo::setTitle(const QString& title)
{
    d->data[cTitle] = title;
}

QString FileInfo::mimeType() const
{
    return d->data.value("mimeType").toString();
}

bool FileInfo::isFolder() const
{
    return mimeType() == "application/vnd.google-apps.folder";
}

QDateTime FileInfo::createdDate() const
{
    return QDateTime::fromString(d->data.value("createdDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedDate() const
{
    return QDateTime::fromString(d->data.value("modifiedDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::modifiedByMeDate() const
{
    return QDateTime::fromString(d->data.value("modifiedByMeDate").toString(), Qt::ISODate);
}

QDateTime FileInfo::lastViewedByMeDate() const
{
    return QDateTime::fromString(d->data.value("lastViewedByMeDate").toString(), Qt::ISODate);
}

bool FileInfo::isEditable() const
{
    return d->data.value("editable").toBool();
}

QUrl FileInfo::downloadUrl() const
{
    return QUrl(d->data.value("downloadUrl").toString());
}

QMap<QString, QUrl> FileInfo::exportList() const
{
    const QVariantMap vm = d->data.value("exportLinks").toMap();

    QMap<QString, QUrl> res;

    QVariantMap::ConstIterator it = vm.begin();
    for (; it != vm.end(); ++it)
        res[it.key()] = it.value().toString();

    return res;
}

QString FileInfo::extension(const QString& format) const
{
    if(!d->data.value("fileExtension").toString().isEmpty())
    {
        return d->data.value("fileExtension").toString();
    }
    else
    {
        int ext = exportList()[format].toString().lastIndexOf("=");
        return exportList()[format].toString().mid(++ext, -1);
    }
}


qint64 FileInfo::fileSize() const
{
    qint64 res = d->data.value("fileSize").toULongLong();
    return res;
}


QString FileInfo::description() const {
QString str(d->data.value("description").toString());
return str;
}

QString FileInfo::headRevisionId() const {
QString str(d->data.value("headRevisionId").toString());
return str;
}

QStringList FileInfo::parents() const
{
    QStringList res;
    Q_FOREACH (const QVariant& v, d->data.value(cParents).toList())
        res << v.toMap()[cId].toString();
    return res;
}

/**
 * @brief FileInfo::setParents - Set parent folders for the file. Each file in google drive could have several parent folders
 * @param l -List of parent folder. Should consist from QStringList of folder id's
 */
void FileInfo::setParents(const QStringList& l)
{
    QVariantList vl;
    Q_FOREACH (const QString& s, l)
        //vl << s;
    {
        QVariantMap parent;
        parent["id"]=s;
         vl << parent;
    }

    d->data[cParents] = vl;
}


namespace {

const std::map<FileInfo::Labels, QString>& labels()
{
    static std::map<FileInfo::Labels, QString> m;
    if (m.empty())
    {
        m[FileInfo::Hidden]     = "hidden";
        m[FileInfo::Restricted] = "restricted";
        m[FileInfo::Starred]    = "starred";
        m[FileInfo::Trashed]    = "trashed";
        m[FileInfo::Viewed]     = "viewed";
    }
    return m;
}

const QString& name(FileInfo::Labels label)
{
    return labels().at(label);
}

}

FileInfo::LabelsMask FileInfo::labels() const
{
    const QVariantMap& labels = d->data.value("labels").toMap();
    return (labels.value(name(Hidden)).toBool())    ? Hidden    : None
        | (labels.value(name(Restricted)).toBool()) ? Restricted: None
        | (labels.value(name(Starred)).toBool())    ? Starred   : None
        | (labels.value(name(Trashed)).toBool())    ? Trashed   : None
        | (labels.value(name(Viewed)).toBool())     ? Viewed    : None
        ;
}

bool GoogleDrive::FileInfo::hasLabel(FileInfo::Labels label) const
{
    return labels() & label;
}



}
