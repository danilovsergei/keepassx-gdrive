#include "RemoteFile.h"

RemoteFile::RemoteFile()
{
    d = new RemoteFileData();
}

RemoteFile::RemoteFile(const RemoteFile &other) : d(other.d)
{
}

const QString RemoteFile::getTitle()     const
{
    return d->title;
}

const QDateTime RemoteFile::getCreatedDate() const
{
    return d->createdDate;
}

const QDateTime RemoteFile::getModifiedDate() const
{
    return d->modifiedDate;
}

const qint64 RemoteFile::getFileSize()    const
{
    return d->fileSize;
}

const QStringList RemoteFile::getParents()     const
{
    return d->parents;
}

const QString RemoteFile::getId() const
{
    return d->id;
}

void RemoteFile::setTitle(const QString &title)
{
    d->title = title;
}

void RemoteFile::setCreatedDate(const QDateTime &createdDated)
{
    d->createdDate = createdDated;
}

void RemoteFile::setModifiedDate(const QDateTime modifiedDate)
{
    d->modifiedDate = modifiedDate;
}

void RemoteFile::setFileSize(const quint64 fileSize)
{
    d->fileSize = fileSize;
}

void RemoteFile::setParents(const QStringList &parents)
{
    d->parents = parents;
}

void RemoteFile::setId(const QString &id)
{
    d->id = id;
}
