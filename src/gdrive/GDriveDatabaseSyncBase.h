#ifndef GDRIVEDATABASESYNCBASE_H
#define GDRIVEDATABASESYNCBASE_H
#include <QtCore/QSharedPointer>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QMutableMapIterator>
#include <gdrive/GDriveSyncObject.h>
using namespace DatabaseSync;
class GDriveDatabaseSyncBase {
public:

  GDriveDatabaseSyncBase();
  virtual ~GDriveDatabaseSyncBase() {}

  /**
   * @brief syncDatabases - syncs items between two provided by derived class
   * constructor databases
   */
  virtual QSharedPointer<GDriveSyncObject>syncDatabases() = 0;
  virtual void                            setSyncObject(
    QSharedPointer<GDriveSyncObject> syncObject) = 0;

protected:

  QSharedPointer<GDriveSyncObject> syncObject;
  QSharedPointer<GDriveSyncObject> getSyncObject() {
    Q_ASSERT(!syncObject.isNull());  return syncObject;
  }
};
#endif // GDRIVEDATABASESYNCBASE_H
