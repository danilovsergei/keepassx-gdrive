#ifndef DATABASESYNCBASE_H
#define DATABASESYNCBASE_H
#include <QSharedPointer>
#include <QMap>
#include <QList>
#include <QMutableMapIterator>
#include "remotedrive/SyncObject.h"
#include <QDebug>
#include <core/Database.h>
#include <core/Metadata.h>
#include <core/Group.h>

using namespace DatabaseSyncObject;
class DatabaseSyncBase
{
public:

  DatabaseSyncBase(Database *db1, Database *db2);
  virtual ~DatabaseSyncBase()
  {
  }

  /**
   * @brief syncDatabases - syncs items between two provided by derived class
   * constructor databases
   */
  virtual QSharedPointer<SyncObject> syncDatabases() = 0;
  virtual QSharedPointer<SyncObject> syncEntries() = 0;

  virtual void                            setSyncObject(
    QSharedPointer<SyncObject> syncObject) = 0;

protected:
  Database *db1;
  Database *db2;
  QSharedPointer<SyncObject> syncObject;
  void syncMetadata();

  QSharedPointer<SyncObject> getSyncObject()
  {
    // TODO add proper error handling with notifying through UI
    if (syncObject.isNull())
      qDebug() << "Failed to get sync object. It was not constructed";
    Q_ASSERT(!syncObject.isNull());
    return syncObject;
  }
};
#endif // DATABASESYNCBASE_H
