#ifndef GDRIVEDATABASESYNC_H
#define GDRIVEDATABASESYNC_H
#include "core/Database.h"

class GDriveDatabaseSync:public QObject
{
    Q_OBJECT
protected:
    Database* db1;
    Database* db2;
    //groups/entries missed in local database, but present in remote database
    int resultLocalMissingEntries=0;
    int resultLocalMissingGroups=0;

    //groups/entries present in local database but missing in remote database
    int resultRemoteMissingGroups=0;
    int resultRemoteMissingEntries=0;



    //groups/entries which have older timestamp in local database
    int resultLocalOlderEntries=0;
    int resultLocalOlderGroups=0;

    void setOlderEntries(int value);
    //groups/entries which have older timestamp in remote database
    int resultRemoteOlderEntries=0;
    int resultRemoteOlderGroups=0;

    //groups/entries which were deleted in local db
    int resultLocalRemovedEntries=0;
    int resultLocalRemovedGroups=0;


    //groups/entries which were deleted in remote db
    int resultRemoteRemovedEntries=0;
    int resultRemoteRemovedGroups=0;



public:
    GDriveDatabaseSync(Database* db1, Database* db2);
    virtual void syncDatabases()=0;
    virtual ~GDriveDatabaseSync() {}

    int getLocalOlderEntries();
    int getLocalRemovedEntries();
    int getLocalMissingEntries();

    int getLocalOlderGroups();
    int getLocalRemovedGroups();
    int getLocalMissingGroups();

    int getRemoteOlderEntries();
    int getRemoteRemovedEntries();
    int getRemoteMissingEntries();

    int getRemoteOlderGroups();
    int getRemoteRemovedGroups();
    int getRemoteMissingGroups();



};

#endif // GDRIVEDATABASESYNC_H
