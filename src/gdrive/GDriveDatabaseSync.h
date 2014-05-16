#ifndef GDRIVEDATABASESYNC_H
#define GDRIVEDATABASESYNC_H
#include "core/Database.h"

class GDriveDatabaseSync
{
public:
    GDriveDatabaseSync(Database *db1, Database *db2);
    static GDriveDatabaseSync sync(int id);

private:
    Database* db1;
    Database* db2;

};

#endif // GDRIVEDATABASESYNC_H
