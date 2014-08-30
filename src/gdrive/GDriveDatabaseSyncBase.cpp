#include "GDriveDatabaseSyncBase.h"
namespace DatabaseSync {
GDriveDatabaseSyncBase::GDriveDatabaseSyncBase()
{}

SyncEntry::SyncEntry(SyncEntry::ObjectType type) : type(type)   {}

void SyncEntry::increaseRemoteOlderEntries() {
  resultRemoteOlderEntries++;
}

void SyncEntry::increaseRemoteMissingEntries() {
  resultRemoteMissingEntries++;
}

void SyncEntry::increaseRemoteRemovedEntries() {
  resultRemoteRemovedEntries++;
}

void SyncEntry::increaseLocalOlderEntries() {
  resultLocalOlderEntries++;
}

void SyncEntry::increaseLocalMissingEntries() {
  resultLocalMissingEntries++;
}

void SyncEntry::increaseLocalRemovedEntries() {
  resultLocalRemovedEntries++;
}

int SyncEntry::getRemoteOlderEntries() {
  return resultRemoteOlderEntries;
}

int SyncEntry::getRemoteMissingEntries() {
  return resultRemoteMissingEntries;
}

int SyncEntry::getRemoteRemovedEntries() {
  return resultRemoteRemovedEntries;
}

int SyncEntry::getLocalOlderEntries() {
  return resultLocalOlderEntries;
}

int SyncEntry::getLocalMissingEntries() {
  return resultLocalMissingEntries;
}

int SyncEntry::getLocalRemovedEntries() {
  return resultLocalRemovedEntries;
}
QSharedPointer<SyncEntry> GDriveSyncObject::getResultStat(SyncEntry::ObjectType entryType){
  return data[entryType];
}

GDriveSyncObject::GDriveSyncObject()
{
  data[SyncEntry::ObjectType::Entry] = QSharedPointer<SyncEntry>(new SyncEntry(SyncEntry::ObjectType::Entry));
  data[SyncEntry::ObjectType::Group] = QSharedPointer<SyncEntry>(new SyncEntry(SyncEntry::ObjectType::Group));
}


}
