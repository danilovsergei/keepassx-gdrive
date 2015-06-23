#ifndef DATABASESYNCBASE_H
#define DATABASESYNCBASE_H
#include <QtCore/QSharedPointer>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QMutableMapIterator>
#include "remotedrive/SyncObject.h"
#include <QtCore/QDebug>

using namespace DatabaseSyncObject;
class DatabaseSyncBase
{
public:

  DatabaseSyncBase();
  virtual ~DatabaseSyncBase()
  {
  }

  /**
   * @brief syncDatabases - syncs items between two provided by derived class
   * constructor databases
   */
  virtual QSharedPointer<SyncObject> syncDatabases() = 0;
  virtual void                            setSyncObject(
    QSharedPointer<SyncObject> syncObject) = 0;

protected:

  QSharedPointer<SyncObject> syncObject;

  QSharedPointer<SyncObject> getSyncObject()
  {
     // TODO add proper error handling with notifying through UI
    if (syncObject.isNull()) {
        qDebug() << "Failed to construct sync object.";
    }
    Q_ASSERT(!syncObject.isNull());
    return syncObject;
  }
};
#endif // DATABASESYNCBASE_H
