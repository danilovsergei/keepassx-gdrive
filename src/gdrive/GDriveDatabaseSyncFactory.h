#ifndef GDRIVEDATABASESYNCFACTORY_H
#define GDRIVEDATABASESYNCFACTORY_H

#include "GDriveGroupsSync.h"
#include "GDriveEntriesSync.h"
#include "GDriveDatabaseSync.h"

class GDriveDatabaseSyncFactory
{
public:
    enum class SyncId {ENTRY,GROUP,ALL};
    static  QSharedPointer<GDriveDatabaseSync> createDatabaseSync(SyncId id, Database *db1, Database *db2);
};

#endif // GDRIVEDATABASESYNCFACTORY_H
