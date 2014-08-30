#include "GDriveDatabaseSyncFactory.h"

QSharedPointer<GDriveDatabaseSyncBase> GDriveDatabaseSyncFactory::createDatabaseSync(SyncId id,Database* db1,Database* db2) {
QSharedPointer<GDriveDatabaseSyncBase> p;
switch(id) {
case SyncId::ENTRY:
p= QSharedPointer<GDriveDatabaseSyncBase>(new GDriveEntriesSync(db1,db2,false));
break;
case SyncId::ALL:
p=QSharedPointer<GDriveDatabaseSyncBase>(new GDriveEntriesSync(db1,db2,true));
break;
case SyncId::GROUP:
p=QSharedPointer<GDriveDatabaseSyncBase>(new GDriveGroupsSync(db1,db2));
break;
default:
    Q_ASSERT(false);
}
return p;
}
