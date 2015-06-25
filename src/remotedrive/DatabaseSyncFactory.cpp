#include "DatabaseSyncFactory.h"

QSharedPointer<DatabaseSyncBase> DatabaseSyncFactory::createDatabaseSync(SyncId id,
                                                                         Database *db1,
                                                                        Database *db2)
{
    QSharedPointer<DatabaseSyncBase> p;
    switch (id) {
    case SyncId::ENTRY:
        p = QSharedPointer<DatabaseSyncBase>(new EntriesSync(db1, db2));
        break;
    case SyncId::ALL:
        p = QSharedPointer<DatabaseSyncBase>(new AllSync(db1, db2));
        break;
    case SyncId::GROUP:
        p = QSharedPointer<DatabaseSyncBase>(new GroupsSync(db1, db2));
        break;
    default:
        Q_ASSERT(false);
    }
    return p;
}
