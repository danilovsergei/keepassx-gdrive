/*
 *  Copyright (C) 2010 Felix Geyer <debfx@fobos.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 or (at your option)
 *  version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEEPASSX_DATABASEWIDGET_H
#define KEEPASSX_DATABASEWIDGET_H

#include <QScopedPointer>
#include <QStackedWidget>

#include "core/Global.h"
#include "core/Uuid.h"

#include "gui/entry/EntryModel.h"

// Remote drive sync includes
#include "remotedrive/SyncObject.h"
#include "remotedrive/RemoteDriveApi.h"
#include "remotedrive/gdrive/CommandsFactoryImpl.h"
#include "remotedrive/gdrive/GoogleDriveCredentials.h"
#include "remotedrive/Errors.h"
#include "remotedrive/OptionsBuilder.h"
#include "config-keepassx.h"

class ChangeMasterKeyWidget;
class DatabaseOpenWidget;
class DatabaseSettingsWidget;
class Database;
class EditEntryWidget;
class EditGroupWidget;
class Entry;
class EntryView;
class Group;
class GroupView;
class KeePass1OpenWidget;
class QFile;
class QMenu;
class QSplitter;
class UnlockDatabaseWidget;

// Required by remote db sync.
class DatabaseOpenWidgetCloud;
using namespace DatabaseSyncObject;

namespace Ui {
    class SearchWidget;
}

class DatabaseWidget : public QStackedWidget
{
    Q_OBJECT

public:
    enum Mode
    {
        None,
        ViewMode,
        EditMode,
        LockedMode
    };

    explicit DatabaseWidget(Database* db, QWidget* parent = Q_NULLPTR);
    ~DatabaseWidget();
    Database* database();
    bool dbHasKey() const;
    bool canDeleteCurrentGroup() const;
    bool isInSearchMode() const;
    int addWidget(QWidget* w);
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget* widget);
    DatabaseWidget::Mode currentMode() const;
    void lock();
    void updateFilename(const QString& filename);
    int numberOfSelectedEntries() const;
    QStringList customEntryAttributes() const;
    bool isGroupSelected() const;
    bool isInEditMode() const;
    QList<int> splitterSizes() const;
    void setSplitterSizes(const QList<int>& sizes);
    QList<int> entryHeaderViewSizes() const;
    void setEntryViewHeaderSizes(const QList<int>& sizes);
    void clearAllWidgets();
    bool currentEntryHasTitle();
    bool currentEntryHasUsername();
    bool currentEntryHasPassword();
    bool currentEntryHasUrl();
    bool currentEntryHasNotes();

Q_SIGNALS:
    void closeRequest();
    void currentModeChanged(DatabaseWidget::Mode mode);
    void groupChanged();
    void entrySelectionChanged();
    void databaseChanged(Database* newDb);
    void groupContextMenuRequested(const QPoint& globalPos);
    void entryContextMenuRequested(const QPoint& globalPos);
    void unlockedDatabase();
    void listModeAboutToActivate();
    void listModeActivated();
    void searchModeAboutToActivate();
    void searchModeActivated();
    void splitterSizesChanged();
    void entryColumnSizesChanged();

    // Remote sync signals.
    void cloudDbSelected(const QString& fileName,Database* db);
    void cloudDbRejected(Database* db);
    void databaseSaved(Database* db,  const QString filePath);
    void requestSaveDatabase(Database* db, bool saveToCloud);

public Q_SLOTS:
    void createEntry();
    void cloneEntry();
    void deleteEntries();
    void copyTitle();
    void copyUsername();
    void copyPassword();
    void copyURL();
    void copyNotes();
    void copyAttribute(QAction* action);
    void performAutoType();
    void openUrl();
    void openUrlForEntry(Entry* entry);
    void createGroup();
    void deleteGroup();
    void switchToEntryEdit();
    void switchToGroupEdit();
    void switchToMasterKeyChange();
    void switchToDatabaseSettings();
    void switchToOpenDatabase(const QString& fileName);
    void switchToOpenDatabase(const QString& fileName, const QString& password, const QString& keyFile);
    void switchToImportKeepass1(const QString& fileName);
    void openSearch();
    // Remote sync slots
    void cloudDbOpen(QString dbPath);
    void syncDone();
    void syncError(int ErrorType, QString description);
    void saveDatabaseToCloud(Database *db, const QString filePath);
    void emitDatabaseSaved(Database *db, const QString filePath);
    void switchToCloudDbOpen();
    //void showSyncLoginPage();

private Q_SLOTS:
    void entryActivationSignalReceived(Entry* entry, EntryModel::ModelColumn column);
    void switchBackToEntryEdit();
    void switchToView(bool accepted);
    void switchToHistoryView(Entry* entry);
    void switchToEntryEdit(Entry* entry);
    void switchToEntryEdit(Entry* entry, bool create);
    void switchToGroupEdit(Group* entry, bool create);
    void emitGroupContextMenuRequested(const QPoint& pos);
    void emitEntryContextMenuRequested(const QPoint& pos);
    void updateMasterKey(bool accepted);
    void openDatabase(bool accepted);
    void unlockDatabase(bool accepted);
    void emitCurrentModeChanged();
    void clearLastGroup(Group* group);
    void search();
    void startSearch();
    void startSearchTimer();
    void showSearch();
    void closeSearch();

    // Remote sync slots
    void rejectDb();
    void syncDatabase();
    // set last modification time of database when entry/group was changed
    void setLastModified(bool accepted);

private:
    void setClipboardTextAndMinimize(const QString& text);
    void setIconFromParent();
    void replaceDatabase(Database* db);

    Database* m_db;
    const QScopedPointer<Ui::SearchWidget> m_searchUi;
    QWidget* const m_searchWidget;
    QWidget* m_mainWidget;
    EditEntryWidget* m_editEntryWidget;
    EditEntryWidget* m_historyEditEntryWidget;
    EditGroupWidget* m_editGroupWidget;
    ChangeMasterKeyWidget* m_changeMasterKeyWidget;
    DatabaseSettingsWidget* m_databaseSettingsWidget;
    DatabaseOpenWidget* m_databaseOpenWidget;
    KeePass1OpenWidget* m_keepass1OpenWidget;
    UnlockDatabaseWidget* m_unlockDatabaseWidget;
    QSplitter* m_splitter;
    GroupView* m_groupView;
    EntryView* m_entryView;
    Group* m_newGroup;
    Entry* m_newEntry;
    Group* m_newParent;
    Group* m_lastGroup;
    QTimer* m_searchTimer;
    QString m_filename;
    Uuid m_groupBeforeLock;

    // Remote db sync needed vars.
    QWidget* m_tabWidget;
    DatabaseOpenWidgetCloud* m_databaseOpenWidgetCloud;
    RemoteDriveApi* remoteDrive;
    KeePassxDriveSync::Command  syncCommand;
    KeePassxDriveSync::Command uploadCommand;
    QString syncCloudDir;
};

#endif // KEEPASSX_DATABASEWIDGET_H
