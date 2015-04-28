#ifndef DATABASESYNCFACTORY_H
#define DATABASESYNCFACTORY_H

#include "GroupsSync.h"
#include "EntriesSync.h"
#include "DatabaseSyncBase.h"

class DatabaseSyncFactory {
public:

  enum class SyncId { ENTRY, GROUP, ALL };
  static  QSharedPointer<DatabaseSyncBase>createDatabaseSync(SyncId    id,
                                                                   Database *db1,
                                                                   Database *db2);
};

#endif // DATABASESYNCFACTORY_H
