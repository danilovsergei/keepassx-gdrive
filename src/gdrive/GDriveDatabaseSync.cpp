#include "GDriveDatabaseSync.h"
GDriveDatabaseSync::GDriveDatabaseSync(Database* db1,Database* db2)
    :db1(db1),db2(db2){}

int GDriveDatabaseSync::getLocalOlderEntries() {
    return GDriveDatabaseSync::resultLocalOlderEntries;
}
int GDriveDatabaseSync::getLocalRemovedEntries() {
    return GDriveDatabaseSync::resultLocalRemovedEntries;
}
int GDriveDatabaseSync::getLocalMissingEntries() {
    return GDriveDatabaseSync::resultLocalMissingEntries;
}

int GDriveDatabaseSync::getLocalOlderGroups() {
    return GDriveDatabaseSync::resultLocalOlderGroups;
}
int GDriveDatabaseSync::getLocalRemovedGroups() {
    return GDriveDatabaseSync::resultLocalRemovedGroups;
}
int GDriveDatabaseSync::getLocalMissingGroups() {
    return GDriveDatabaseSync::resultLocalMissingGroups;
}



int GDriveDatabaseSync::getRemoteOlderEntries() {
    return GDriveDatabaseSync::resultRemoteOlderEntries;
}
int GDriveDatabaseSync::getRemoteRemovedEntries() {
    return GDriveDatabaseSync::resultRemoteRemovedEntries;
}
int GDriveDatabaseSync::getRemoteMissingEntries() {
    return GDriveDatabaseSync::resultRemoteMissingEntries;
}

int GDriveDatabaseSync::getRemoteOlderGroups() {
    return GDriveDatabaseSync::resultRemoteOlderGroups;
}
int GDriveDatabaseSync::getRemoteRemovedGroups() {
    return GDriveDatabaseSync::resultRemoteRemovedGroups;
}
int GDriveDatabaseSync::getRemoteMissingGroups() {
    return GDriveDatabaseSync::resultRemoteMissingGroups;
}
