#ifndef GDRIVEDATABASESYNCFACTORY_H
#define GDRIVEDATABASESYNCFACTORY_H

#include "GDriveGroupsSync.h"
#include "GDriveEntriesSync.h"
#include "GDriveDatabaseSyncBase.h"

class GDriveDatabaseSyncFactory {
public:

  enum class SyncId { ENTRY, GROUP, ALL };
  static  QSharedPointer<GDriveDatabaseSyncBase>createDatabaseSync(SyncId    id,
                                                                   const Database *db1,
                                                                   const Database *db2);
};

#endif // GDRIVEDATABASESYNCFACTORY_H
