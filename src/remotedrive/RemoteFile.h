#ifndef REMOTEFILE_H
#define REMOTEFILE_H

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>
#include <QtCore/QSharedData>

class RemoteFileData : public QSharedData
{
public:

    RemoteFileData() {};
    RemoteFileData(const RemoteFileData &other) : QSharedData(other), title(other.title),
        createdDate(other.createdDate), modifiedDate(other.modifiedDate), fileSize(other.fileSize),
        parents(
            other.parents), id(other.id)
    {
    }

    QString title;
    QDateTime createdDate;
    QDateTime modifiedDate;
    qint64 fileSize;
    QStringList parents;
    QString id;
};

class RemoteFile
{
public:

    RemoteFile();
    RemoteFile(const RemoteFile &other);
    const QString     getTitle()     const;
    const QDateTime   getCreatedDate() const;
    const QDateTime   getModifiedDate() const;
    const qint64      getFileSize()    const;
    const QStringList getParents()     const;
    const QString getId() const;

    void           setTitle(const QString &title);
    void         setCreatedDate(const QDateTime &createdDated);
    void         setModifiedDate(const QDateTime modifiedDate);
    void            setFileSize(const quint64 fileSize);
    void       setParents(const QStringList &parents);
    void        setId(const QString &id);

private:
    QSharedDataPointer<RemoteFileData> d;
};
typedef QList<RemoteFile> RemoteFileList;
#endif // REMOTEFILE_H
