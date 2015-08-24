#ifndef SyncObject_H
#define SyncObject_H

#include <QtCore/QSharedPointer>
#include <QtCore/QPair>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QChar>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

namespace DatabaseSyncObject {
enum class ObjectType : char
{
    Group = 'g', Entry = 'e'
};
enum class ObjectName : char
{
    Missing = 'm', Older = 'o', Removed = 'r'
};
enum class ObjectLocation : char
{
    Local = 'l', Remote = 'r'
};

class SyncMapKey
{
public:
    SyncMapKey(ObjectType type, ObjectName name, ObjectLocation location);
    bool operator==(const SyncMapKey &other) const;
    bool operator<(const SyncMapKey &other) const;
    const QString getKeyData() const;
    const QString toString() const;
    const char get(int index) const;
private:
    QString keyData;
};

class SyncObject
{
public:
    SyncObject();
    ~SyncObject();
    void increase(ObjectType type, ObjectName name, ObjectLocation location);
    void set(ObjectType type, ObjectName name, ObjectLocation location, int amount);
    const int get(ObjectType type, ObjectName name, ObjectLocation location) const;
    QMap<SyncMapKey, int> get();
    QMap<SyncMapKey, QPair<int, int> > compare(QSharedPointer<SyncObject> other);
    void setLocalModificationDate(const QDateTime& dateTime);
    void setRemoteModificationDate(const QDateTime& dateTime);
    const QDateTime getLocalModificationDate();
    const QDateTime getRemoteModificationDate();

private:
    QMap<SyncMapKey, int> data;
    QDateTime localModificationDate;
    QDateTime remoteModificationDate;
};
inline uint qHash(const SyncMapKey &var)
{
    return qHash(var.getKeyData());
}

inline ObjectType SGroup()
{
    return ObjectType::Group;
}

inline ObjectType SEntry()
{
    return ObjectType::Entry;
}

inline ObjectName SMissing()
{
    return ObjectName::Missing;
}

inline ObjectName SOlder()
{
    return ObjectName::Older;
}

inline ObjectName SRemoved()
{
    return ObjectName::Removed;
}

inline ObjectLocation SLocal()
{
    return ObjectLocation::Local;
}

inline ObjectLocation SRemote()
{
    return ObjectLocation::Remote;
}
}

#endif // GDRIVEDATABASESYNCBASE_H
