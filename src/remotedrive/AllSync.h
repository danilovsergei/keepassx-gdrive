#ifndef ALLSYNC_H
#define ALLSYNC_H
#include <remotedrive/GroupsSync.h>
#include <QtCore/QDebug>
#include <remotedrive/DatabaseSyncFactory.h>

class AllSync : public GroupsSync
{
public:
  AllSync(Database *db1, Database *db2);
  /**
   * @brief syncDatabases - syncs groups between two provided databases
   * @param db1  -first source/destination database.Will contain modifications from db2 database after sync
   * @param db2 - second source database.
   */
  QSharedPointer<SyncObject> syncDatabases();
private:
  const QString ENTRY_TYPE = "All";
};

#endif // ALLSYNC_H
