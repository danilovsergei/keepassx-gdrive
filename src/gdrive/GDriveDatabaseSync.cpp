#include "GDriveDatabaseSync.h"
#include "GDriveGroupsSync.h"
#include "GDriveEntriesSync.h"
enum SyncId {entryId=0 , groupId=1};

GDriveDatabaseSync::GDriveDatabaseSync(Database* db1,Database* db2)
{
    this->db1=db1;
    this->db2=db2;
}


GDriveDatabaseSync GDriveDatabaseSync::sync(int id) {
GDriveDatabaseSync* p;
switch(id) {
case entryId:
p=new GDriveEntriesSync(db1,db2);

}


}

