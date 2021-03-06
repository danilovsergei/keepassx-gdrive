#ifndef ENTRIESSYNC_H
#define ENTRIESSYNC_H
#include "core/Database.h"
#include "core/Group.h"
#include "core/Entry.h"
#include "core/Tools.h"
#include "core/Metadata.h"
#include  "GroupsSync.h"
#include "QtCore/QSharedPointer"
using namespace DatabaseSyncObject;
/**
 * @brief The EntriesSync class responsible for entry specific template overrides to peform sync
 */
class EntriesSync : public DatabaseSync<Entry>
{
public:
  EntriesSync(Database *db1, Database *db2);
  ~EntriesSync();
protected:
  void removeEntry(Entry *entry);
  void updateEntryData(Entry* data, Entry* newData);
  bool processEntry(Database *db, Entry *entry);
  ObjectType getObjectType();
  void setParentGroup(Entry *entry, Group *group);
  const Group *getParentGroup(Entry *entry);
  const QString getEntryName(Entry *entry);
  QMap<Uuid, Entry *> getEntriesMap(Database *db);
  QString getType();

private:
  const QString ENTRY_TYPE = "Entry";
};

#endif // ENTRIESSYNC_H
