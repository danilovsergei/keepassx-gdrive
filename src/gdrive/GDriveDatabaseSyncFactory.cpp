#include "GDriveDatabaseSyncFactory.h"

QSharedPointer<GDriveDatabaseSync> GDriveDatabaseSyncFactory::createDatabaseSync(SyncId id,Database* db1,Database* db2) {
QSharedPointer<GDriveDatabaseSync> p;
switch(id) {
case SyncId::ENTRY:
p= QSharedPointer<GDriveDatabaseSync>(new GDriveEntriesSync(db1,db2,false));
break;
case SyncId::ALL:
p=QSharedPointer<GDriveDatabaseSync>(new GDriveEntriesSync(db1,db2,true));
break;
case SyncId::GROUP:
p=QSharedPointer<GDriveDatabaseSync>(new GDriveGroupsSync(db1,db2));
break;
default:
    Q_ASSERT(false);
}
return p;
}
