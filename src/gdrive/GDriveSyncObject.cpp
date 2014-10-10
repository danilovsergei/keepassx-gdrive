#include "GDriveSyncObject.h"

typedef QPair<int, int>resultPair;
namespace DatabaseSync {
SyncMapKey::SyncMapKey(ObjectType     type,
                       ObjectName     name,
                       ObjectLocation location)
{
  const char data[4] =
  { static_cast<char>(type), static_cast<char>(name),
    static_cast<char>(location), '\0' };

  keyData =    QString(data);
}

const QString SyncMapKey::getKeyData() const {
  return keyData;
}

bool SyncMapKey::operator==(const SyncMapKey& other) const {
  return keyData == other.getKeyData();
}

bool SyncMapKey::operator<(const SyncMapKey& other) const {
  return keyData < other.getKeyData();
}

const QString SyncMapKey::toString() const {
return keyData;
}

const char SyncMapKey::get(int index) const {
    Q_ASSERT(index<=2);
    return keyData[index].toLatin1();
}

void GDriveSyncObject::increase(ObjectType     type,
                                ObjectName     name,
                                ObjectLocation location) {
  // increase current value for the given list +1
  int increment = 1; // data[getList(type,name,location)]+1;

  set(type, name, location, increment);
}

void GDriveSyncObject::set(ObjectType     type,
                           ObjectName     name,
                           ObjectLocation location,
                           int            amount) {
  data[SyncMapKey(type, name, location)] = amount;
}

const int GDriveSyncObject::get(ObjectType     type,
                                ObjectName     name,
                                ObjectLocation location) const {
  return data[SyncMapKey(type, name, location)];
}

GDriveSyncObject::GDriveSyncObject()
{
  data = QMap<SyncMapKey, int>();
}

QMap<SyncMapKey, int>GDriveSyncObject::get() {
  return data;
}

QMap < SyncMapKey, QPair < int, int >>
GDriveSyncObject::compare(QSharedPointer<GDriveSyncObject>other) {
  Q_ASSERT(!other.isNull());
  QMap<SyncMapKey, int> otherData = other->get();
  QMap<SyncMapKey, resultPair> result;
  QSet<SyncMapKey> allKeys = QSet<SyncMapKey>::fromList(data.keys()).unite(
    otherData.keys().toSet());
  Q_FOREACH(SyncMapKey key, allKeys) {
    // key exists in both sets but values are different
    if (data.contains(key) && otherData.contains(key) &&
        (data.value(key) != otherData.value(key))) {
      result.insert(key, QPair<int, int>(data.value(key),
                                         otherData.value(key)));
    }
    // second source does not have a key, replace value with zero
    else if (data.contains(key) && !otherData.contains(key)) {
      result.insert(key, QPair<int, int>(data.value(key), 0));
    }
    // first source does not have a key, replace value with zero
    else if (!data.contains(key) && otherData.contains(key)) {
      result.insert(key, QPair<int, int>(0, otherData.value(key)));
    }
  }
  return result;
}
}
