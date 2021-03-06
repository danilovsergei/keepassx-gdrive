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

#include "DatabaseWidget.h"
#include "ui_SearchWidget.h"

#include <QAction>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QTimer>
#include <QProcess>

#include "autotype/AutoType.h"
#include "core/Config.h"
#include "core/EntrySearcher.h"
#include "core/FilePath.h"
#include "core/Group.h"
#include "core/Metadata.h"
#include "core/Tools.h"
#include "gui/ChangeMasterKeyWidget.h"
#include "gui/Clipboard.h"
#include "gui/DatabaseOpenWidget.h"
#include "gui/DatabaseSettingsWidget.h"
#include "gui/KeePass1OpenWidget.h"
#include "gui/MessageBox.h"
#include "gui/UnlockDatabaseWidget.h"
#include "gui/entry/EditEntryWidget.h"
#include "gui/entry/EntryView.h"
#include "gui/group/EditGroupWidget.h"
#include "gui/group/GroupView.h"

// Remote sync includes and metatypes.
#include "gui/DatabaseOpenWidgetCloud.h"
Q_DECLARE_METATYPE(Database*)
Q_DECLARE_METATYPE(QSharedPointer<SyncObject >)

DatabaseWidget::DatabaseWidget(Database* db, QWidget* parent)
    : QStackedWidget(parent)
    , m_db(db)
    , m_searchUi(new Ui::SearchWidget())
    , m_searchWidget(new QWidget())
    , m_newGroup(Q_NULLPTR)
    , m_newEntry(Q_NULLPTR)
    , m_newParent(Q_NULLPTR)
    , m_tabWidget(parent)
{
    m_searchUi->setupUi(m_searchWidget);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);

    m_mainWidget = new QWidget(this);
    QLayout* layout = new QHBoxLayout(m_mainWidget);
    m_splitter = new QSplitter(m_mainWidget);
    m_splitter->setChildrenCollapsible(false);

    QWidget* rightHandSideWidget = new QWidget(m_splitter);
    m_searchWidget->setParent(rightHandSideWidget);

    m_groupView = new GroupView(db, m_splitter);
    m_groupView->setObjectName("groupView");
    m_groupView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_groupView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(emitGroupContextMenuRequested(QPoint)));

    m_entryView = new EntryView(rightHandSideWidget);
    m_entryView->setObjectName("entryView");
    m_entryView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_entryView->setGroup(db->rootGroup());
    connect(m_entryView, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(emitEntryContextMenuRequested(QPoint)));

    QAction* closeAction = new QAction(m_searchWidget);
    QIcon closeIcon = filePath()->icon("actions", "dialog-close");
    closeAction->setIcon(closeIcon);
    m_searchUi->closeSearchButton->setDefaultAction(closeAction);
    m_searchUi->closeSearchButton->setShortcut(Qt::Key_Escape);
    m_searchWidget->hide();
    m_searchUi->caseSensitiveCheckBox->setVisible(false);

    QVBoxLayout* vLayout = new QVBoxLayout(rightHandSideWidget);
    vLayout->setMargin(0);
    vLayout->addWidget(m_searchWidget);
    vLayout->addWidget(m_entryView);

    rightHandSideWidget->setLayout(vLayout);

    setTabOrder(m_searchUi->searchRootRadioButton, m_entryView);
    setTabOrder(m_entryView, m_groupView);
    setTabOrder(m_groupView, m_searchWidget);

    m_splitter->addWidget(m_groupView);
    m_splitter->addWidget(rightHandSideWidget);

    m_splitter->setStretchFactor(0, 30);
    m_splitter->setStretchFactor(1, 70);

    layout->addWidget(m_splitter);
    m_mainWidget->setLayout(layout);

    m_editEntryWidget = new EditEntryWidget();
    m_editEntryWidget->setObjectName("editEntryWidget");
    m_historyEditEntryWidget = new EditEntryWidget();
    m_editGroupWidget = new EditGroupWidget();
    m_editGroupWidget->setObjectName("editGroupWidget");
    m_changeMasterKeyWidget = new ChangeMasterKeyWidget();
    m_changeMasterKeyWidget->headlineLabel()->setText(tr("Change master key"));
    QFont headlineLabelFont = m_changeMasterKeyWidget->headlineLabel()->font();
    headlineLabelFont.setBold(true);
    headlineLabelFont.setPointSize(headlineLabelFont.pointSize() + 2);
    m_changeMasterKeyWidget->headlineLabel()->setFont(headlineLabelFont);
    m_databaseSettingsWidget = new DatabaseSettingsWidget();
    m_databaseSettingsWidget->setObjectName("databaseSettingsWidget");
    m_databaseOpenWidget = new DatabaseOpenWidget();
    m_databaseOpenWidget->setObjectName("databaseOpenWidget");
    m_keepass1OpenWidget = new KeePass1OpenWidget();
    m_keepass1OpenWidget->setObjectName("keepass1OpenWidget");
    m_unlockDatabaseWidget = new UnlockDatabaseWidget();
    m_unlockDatabaseWidget->setObjectName("unlockDatabaseWidget");

    // Remote db open widget.
    m_databaseOpenWidgetCloud= new DatabaseOpenWidgetCloud();
    m_databaseOpenWidgetCloud->headlineLabel()->setText(tr("Open cloud database"));
    m_databaseOpenWidgetCloud->headlineLabel()->setFont(headlineLabelFont);

    addWidget(m_mainWidget);
    addWidget(m_editEntryWidget);
    addWidget(m_editGroupWidget);
    addWidget(m_changeMasterKeyWidget);
    addWidget(m_databaseSettingsWidget);
    addWidget(m_historyEditEntryWidget);
    addWidget(m_databaseOpenWidget);
    addWidget(m_keepass1OpenWidget);
    addWidget(m_unlockDatabaseWidget);
    addWidget(m_databaseOpenWidgetCloud);

    connect(m_splitter, SIGNAL(splitterMoved(int,int)), SIGNAL(splitterSizesChanged()));
    connect(m_entryView->header(), SIGNAL(sectionResized(int,int,int)), SIGNAL(entryColumnSizesChanged()));
    connect(m_groupView, SIGNAL(groupChanged(Group*)), this, SLOT(clearLastGroup(Group*)));
    connect(m_groupView, SIGNAL(groupChanged(Group*)), SIGNAL(groupChanged()));
    connect(m_groupView, SIGNAL(groupChanged(Group*)), m_entryView, SLOT(setGroup(Group*)));
    connect(m_entryView, SIGNAL(entryActivated(Entry*, EntryModel::ModelColumn)),
            SLOT(entryActivationSignalReceived(Entry*, EntryModel::ModelColumn)));
    connect(m_entryView, SIGNAL(entrySelectionChanged()), SIGNAL(entrySelectionChanged()));
    connect(m_editEntryWidget, SIGNAL(editFinished(bool)), SLOT(switchToView(bool)));
    connect(m_editEntryWidget, SIGNAL(historyEntryActivated(Entry*)), SLOT(switchToHistoryView(Entry*)));
    connect(m_historyEditEntryWidget, SIGNAL(editFinished(bool)), SLOT(switchBackToEntryEdit()));
    connect(m_editGroupWidget, SIGNAL(editFinished(bool)), SLOT(switchToView(bool)));
    connect(m_changeMasterKeyWidget, SIGNAL(editFinished(bool)), SLOT(updateMasterKey(bool)));
    connect(m_databaseSettingsWidget, SIGNAL(editFinished(bool)), SLOT(switchToView(bool)));
    connect(m_databaseOpenWidget, SIGNAL(editFinished(bool)), SLOT(openDatabase(bool)));
    connect(m_keepass1OpenWidget, SIGNAL(editFinished(bool)), SLOT(openDatabase(bool)));
    connect(m_unlockDatabaseWidget, SIGNAL(editFinished(bool)), SLOT(unlockDatabase(bool)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(emitCurrentModeChanged()));
    connect(m_searchUi->searchEdit, SIGNAL(textChanged(QString)), this, SLOT(startSearchTimer()));
    connect(m_searchUi->caseSensitiveCheckBox, SIGNAL(toggled(bool)), this, SLOT(startSearch()));
    connect(m_searchUi->searchCurrentRadioButton, SIGNAL(toggled(bool)), this, SLOT(startSearch()));
    connect(m_searchUi->searchRootRadioButton, SIGNAL(toggled(bool)), this, SLOT(startSearch()));
    connect(m_searchUi->searchEdit, SIGNAL(returnPressed()), m_entryView, SLOT(setFocus()));
    connect(m_searchTimer, SIGNAL(timeout()), this, SLOT(search()));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeSearch()));

    // Remote db open  connect.
    // TODO find better way to check whether cmake google drive is enabled.
    if (QString(GOOGLE_DRIVE_SYNC) == "ON") {
        // initialize credentials in gui thread because it could contain UI pages
        // to let user approve access to accounts like GMAIL
        AuthCredentials* creds = new GoogleDriveCredentials(this);
        CommandsFactory* commandsFactory = new CommandsFactoryImpl(this,creds);
        // remote drive will be used to call all remote drive functions like sync , upload, download
        remoteDrive = new RemoteDriveApi(this,commandsFactory);
        syncCommand = remoteDrive->sync();
        uploadCommand  = remoteDrive->upload();
         syncCloudDir = config()->get(RemoteDrive::CONFIG_KEEPASS_REMOTE_FOLDER).toString();
        // update last db last modification time to use it for sync with remote database
        connect(m_databaseOpenWidgetCloud, SIGNAL(dbRejected()),SLOT(rejectDb()));
        connect(m_databaseOpenWidgetCloud,SIGNAL(dbSelected(QString)),this,SLOT(cloudDbOpen(QString)));
        connect(m_editEntryWidget, SIGNAL(editFinished(bool)), SLOT(setLastModified(bool)));
        connect(m_editGroupWidget, SIGNAL(editFinished(bool)), SLOT(setLastModified(bool)));
        connect(syncCommand.data(), SIGNAL(finished()),      this, SLOT(syncDone()));
        connect(this, SIGNAL(databaseSaved(Database*, QString)), this, SLOT(saveDatabaseToCloud(Database*, QString)));
        connect(this, SIGNAL(requestSaveDatabase(Database*, bool)), m_tabWidget, SLOT(saveDatabase(Database*, bool)));
    }
    setCurrentWidget(m_mainWidget);
}

DatabaseWidget::~DatabaseWidget()
{
}

DatabaseWidget::Mode DatabaseWidget::currentMode() const
{
    if (currentWidget() == Q_NULLPTR) {
        return DatabaseWidget::None;
    }
    else if (currentWidget() == m_mainWidget) {
        return DatabaseWidget::ViewMode;
    }
    else if (currentWidget() == m_unlockDatabaseWidget) {
        return DatabaseWidget::LockedMode;
    }
    else {
        return DatabaseWidget::EditMode;
    }
}

bool DatabaseWidget::isInEditMode() const
{
    return currentMode() == DatabaseWidget::EditMode;
}

QList<int> DatabaseWidget::splitterSizes() const
{
    return m_splitter->sizes();
}

void DatabaseWidget::setSplitterSizes(const QList<int>& sizes)
{
    m_splitter->setSizes(sizes);
}

QList<int> DatabaseWidget::entryHeaderViewSizes() const
{
    QList<int> sizes;

    for (int i = 0; i < m_entryView->header()->count(); i++) {
        sizes.append(m_entryView->header()->sectionSize(i));
    }

    return sizes;
}

void DatabaseWidget::setEntryViewHeaderSizes(const QList<int>& sizes)
{
    if (sizes.size() != m_entryView->header()->count()) {
        Q_ASSERT(false);
        return;
    }

    for (int i = 0; i < sizes.size(); i++) {
        m_entryView->header()->resizeSection(i, sizes[i]);
    }
}

void DatabaseWidget::clearAllWidgets()
{
    m_editEntryWidget->clear();
    m_historyEditEntryWidget->clear();
    m_editGroupWidget->clear();
}

void DatabaseWidget::emitCurrentModeChanged()
{
    Q_EMIT currentModeChanged(currentMode());
}

Database* DatabaseWidget::database()
{
    return m_db;
}

void DatabaseWidget::createEntry()
{
    if (!m_groupView->currentGroup()) {
        Q_ASSERT(false);
        return;
    }

    m_newEntry = new Entry();
    m_newEntry->setUuid(Uuid::random());
    m_newEntry->setUsername(m_db->metadata()->defaultUserName());
    m_newParent = m_groupView->currentGroup();
    setIconFromParent();
    switchToEntryEdit(m_newEntry, true);
}

void DatabaseWidget::setIconFromParent()
{
    if (!config()->get("UseGroupIconOnEntryCreation").toBool()) {
        return;
    }

    if (m_newParent->iconNumber() == Group::DefaultIconNumber && m_newParent->iconUuid().isNull()) {
        return;
    }

    if (m_newParent->iconUuid().isNull()) {
        m_newEntry->setIcon(m_newParent->iconNumber());
    }
    else {
        m_newEntry->setIcon(m_newParent->iconUuid());
    }
}

void DatabaseWidget::replaceDatabase(Database* db)
{
    Database* oldDb = m_db;
    m_db = db;
    m_groupView->changeDatabase(m_db);
    Q_EMIT databaseChanged(m_db);
    delete oldDb;
}

void DatabaseWidget::cloneEntry()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    Entry* entry = currentEntry->clone(Entry::CloneNewUuid | Entry::CloneResetTimeInfo);
    entry->setGroup(currentEntry->group());
    m_entryView->setFocus();
    m_entryView->setCurrentEntry(entry);
}

void DatabaseWidget::deleteEntries()
{
    const QModelIndexList selected = m_entryView->selectionModel()->selectedRows();

    if (selected.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    // get all entry pointers as the indexes change when removing multiple entries
    QList<Entry*> selectedEntries;
    Q_FOREACH (const QModelIndex& index, selected) {
        selectedEntries.append(m_entryView->entryFromIndex(index));
    }

    bool inRecylceBin = Tools::hasChild(m_db->metadata()->recycleBin(), selectedEntries.first());
    if (inRecylceBin || !m_db->metadata()->recycleBinEnabled()) {
        QMessageBox::StandardButton result;

        if (selected.size() == 1) {
            result = MessageBox::question(
            this, tr("Delete entry?"),
            tr("Do you really want to delete the entry \"%1\" for good?")
                .arg(selectedEntries.first()->title()),
            QMessageBox::Yes | QMessageBox::No);
        }
        else {
            result = MessageBox::question(
                this, tr("Delete entries?"),
                tr("Do you really want to delete %1 entries for good?")
                .arg(selected.size()),
                QMessageBox::Yes | QMessageBox::No);
        }

        if (result == QMessageBox::Yes) {
            Q_FOREACH (Entry* entry, selectedEntries) {
                delete entry;
        }
    }
    }
    else {
        if (selected.size() > 1) {
            QMessageBox::StandardButton result = MessageBox::question(
                this, tr("Move entries to recycle bin?"),
                tr("Do you really want to move %n entry(s) to the recycle bin?", 0, selected.size()),
                QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::No) {
                return;
            }
        }

        Q_FOREACH (Entry* entry, selectedEntries) {
            m_db->recycleEntry(entry);
        }
    }
}

void DatabaseWidget::copyTitle()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->title());
}

void DatabaseWidget::copyUsername()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->username());
}

void DatabaseWidget::copyPassword()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->password());
}

void DatabaseWidget::copyURL()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->url());
}

void DatabaseWidget::copyNotes()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->notes());
}

void DatabaseWidget::copyAttribute(QAction* action)
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    setClipboardTextAndMinimize(currentEntry->attributes()->value(action->text()));
}

void DatabaseWidget::setClipboardTextAndMinimize(const QString& text)
{
    clipboard()->setText(text);
    if (config()->get("MinimizeOnCopy").toBool()) {
        window()->showMinimized();
    }
}

void DatabaseWidget::performAutoType()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    autoType()->performAutoType(currentEntry, window());
}

void DatabaseWidget::openUrl()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return;
    }

    openUrlForEntry(currentEntry);
}

void DatabaseWidget::openUrlForEntry(Entry* entry)
{
    QString urlString = entry->resolvePlaceholders(entry->url());
    if (urlString.isEmpty()) {
        return;
    }

    if (urlString.startsWith("cmd://")) {
        if (urlString.length() > 6) {
            QProcess::startDetached(urlString.mid(6));
    }
}
    else {
        QUrl url = QUrl::fromUserInput(urlString);
        QDesktopServices::openUrl(url);
    }
}

void DatabaseWidget::createGroup()
{
    if (!m_groupView->currentGroup()) {
        Q_ASSERT(false);
        return;
    }

    m_newGroup = new Group();
    m_newGroup->setUuid(Uuid::random());
    m_newParent = m_groupView->currentGroup();
    switchToGroupEdit(m_newGroup, true);
}

void DatabaseWidget::deleteGroup()
{
    Group* currentGroup = m_groupView->currentGroup();
    if (!currentGroup || !canDeleteCurrentGroup()) {
        Q_ASSERT(false);
        return;
    }

    bool inRecylceBin = Tools::hasChild(m_db->metadata()->recycleBin(), currentGroup);
    if (inRecylceBin || !m_db->metadata()->recycleBinEnabled()) {
        QMessageBox::StandardButton result = MessageBox::question(
            this, tr("Delete group?"),
            tr("Do you really want to delete the group \"%1\" for good?")
            .arg(currentGroup->name()),
            QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            delete currentGroup;
        }
    }
    else {
        m_db->recycleGroup(currentGroup);
    }
}

int DatabaseWidget::addWidget(QWidget* w)
{
    w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    int index = QStackedWidget::addWidget(w);

    adjustSize();

    return index;
}

void DatabaseWidget::setCurrentIndex(int index)
{
    // use setCurrentWidget() instead
    // index is not reliable
    Q_UNUSED(index);
    Q_ASSERT(false);
}

void DatabaseWidget::setCurrentWidget(QWidget* widget)
{
    if (currentWidget()) {
        currentWidget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }

    QStackedWidget::setCurrentWidget(widget);

    if (currentWidget()) {
        currentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }

    adjustSize();
}

void DatabaseWidget::switchToView(bool accepted)
{
    if (m_newGroup) {
        if (accepted) {
            m_newGroup->setParent(m_newParent);
            m_groupView->setCurrentGroup(m_newGroup);
            m_groupView->expandGroup(m_newParent);
        }
        else {
            delete m_newGroup;
        }

        m_newGroup = Q_NULLPTR;
        m_newParent = Q_NULLPTR;
    }
    else if (m_newEntry) {
        if (accepted) {
            m_newEntry->setGroup(m_newParent);
            m_entryView->setFocus();
            m_entryView->setCurrentEntry(m_newEntry);
        }
        else {
            delete m_newEntry;
        }

        m_newEntry = Q_NULLPTR;
        m_newParent = Q_NULLPTR;
    }

    setCurrentWidget(m_mainWidget);
}

void DatabaseWidget::switchToHistoryView(Entry* entry)
{
    m_historyEditEntryWidget->loadEntry(entry, false, true, m_editEntryWidget->entryTitle(), m_db);
    setCurrentWidget(m_historyEditEntryWidget);
}

void DatabaseWidget::switchBackToEntryEdit()
{
    setCurrentWidget(m_editEntryWidget);
}

void DatabaseWidget::switchToEntryEdit(Entry* entry)
{
    switchToEntryEdit(entry, false);
}

void DatabaseWidget::switchToEntryEdit(Entry* entry, bool create)
{
    Group* group = m_groupView->currentGroup();
    if (!group) {
        Q_ASSERT(m_entryView->inEntryListMode());
        group = m_lastGroup;
    }
    Q_ASSERT(group);

    m_editEntryWidget->loadEntry(entry, create, false, group->name(), m_db);
    setCurrentWidget(m_editEntryWidget);
}

void DatabaseWidget::switchToGroupEdit(Group* group, bool create)
{
    m_editGroupWidget->loadGroup(group, create, m_db);
    setCurrentWidget(m_editGroupWidget);
}

void DatabaseWidget::updateMasterKey(bool accepted)
{
    if (accepted) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool result = m_db->setKey(m_changeMasterKeyWidget->newMasterKey());
        QApplication::restoreOverrideCursor();

        if (!result) {
            MessageBox::critical(this, tr("Error"), tr("Unable to calculate master key"));
            return;
        }
    }
    else if (!m_db->hasKey()) {
        Q_EMIT closeRequest();
        return;
    }

    setCurrentWidget(m_mainWidget);
}

/**
 * @brief DatabaseWidget::openDatabase loads db view to the current widget
 * after user entered correct credentials on DatabaseOpenWidget
 * @param accepted
 */
void DatabaseWidget::openDatabase(bool accepted)
{
    if (accepted) {
        replaceDatabase(static_cast<DatabaseOpenWidget*>(sender())->database());
        setCurrentWidget(m_mainWidget);

        // Save the time when initial db opened.
        // Will update it each time when database in memory changed and use to decide where to run sync.
        m_db->metadata()->setLastModifiedDate(QFileInfo(m_filename).lastModified());
        syncDatabase();

        // We won't need those anymore and KeePass1OpenWidget closes
        // the file in its dtor.
        delete m_databaseOpenWidget;
        m_databaseOpenWidget = Q_NULLPTR;
        delete m_keepass1OpenWidget;
        m_keepass1OpenWidget = Q_NULLPTR;
    }
    else {
        if (m_databaseOpenWidget->database()) {
            delete m_databaseOpenWidget->database();
        }
        Q_EMIT closeRequest();
    }
}

void DatabaseWidget::unlockDatabase(bool accepted)
{
    if (!accepted) {
        Q_EMIT closeRequest();
        return;
    }

    replaceDatabase(static_cast<DatabaseOpenWidget*>(sender())->database());

    QList<Group*> groups = m_db->rootGroup()->groupsRecursive(true);
    Q_FOREACH (Group* group, groups) {
        if (group->uuid() == m_groupBeforeLock) {
            m_groupView->setCurrentGroup(group);
            break;
        }
    }

    m_groupBeforeLock = Uuid();
    setCurrentWidget(m_mainWidget);
    m_unlockDatabaseWidget->clearForms();
    Q_EMIT unlockedDatabase();
}

void DatabaseWidget::entryActivationSignalReceived(Entry* entry, EntryModel::ModelColumn column)
{
    if (column == EntryModel::Url && !entry->url().isEmpty()) {
        openUrlForEntry(entry);
    }
    else {
        switchToEntryEdit(entry);
    }
}

void DatabaseWidget::switchToEntryEdit()
{
    Entry* entry = m_entryView->currentEntry();
    Q_ASSERT(entry);
    if (!entry) {
        return;
}

    switchToEntryEdit(entry, false);
}

void DatabaseWidget::switchToGroupEdit()
{
    Group* group = m_groupView->currentGroup();
    Q_ASSERT(group);
    if (!group) {
        return;
    }

    switchToGroupEdit(group, false);
}

void DatabaseWidget::switchToMasterKeyChange()
{
    m_changeMasterKeyWidget->clearForms();
    setCurrentWidget(m_changeMasterKeyWidget);
}

void DatabaseWidget::switchToDatabaseSettings()
{
    m_databaseSettingsWidget->load(m_db);
    setCurrentWidget(m_databaseSettingsWidget);
}

/**
 * @brief DatabaseWidget::switchToOpenDatabase passes control to databaseOpenWidget
 * Used by DatabaseOpenWidgetCloud
 * @param fileName - full path of database which used to initialize the widget
 */
void DatabaseWidget::switchToOpenDatabase(const QString& fileName)
{
    updateFilename(fileName);
    m_databaseOpenWidget->load(fileName);
    setCurrentWidget(m_databaseOpenWidget);
}

void DatabaseWidget::switchToOpenDatabase(const QString& fileName, const QString& password,
                                          const QString& keyFile)
{
    updateFilename(fileName);
    switchToOpenDatabase(fileName);
    m_databaseOpenWidget->enterKey(password, keyFile);
}

void DatabaseWidget::switchToImportKeepass1(const QString& fileName)
{
    updateFilename(fileName);
    m_keepass1OpenWidget->load(fileName);
    setCurrentWidget(m_keepass1OpenWidget);
}

void DatabaseWidget::openSearch()
{
    if (isInSearchMode()) {
        m_searchUi->searchEdit->selectAll();

        if (!m_searchUi->searchEdit->hasFocus()) {
            m_searchUi->searchEdit->setFocus();
            // make sure the search action is checked again
            emitCurrentModeChanged();
        }
    }
    else {
        showSearch();
    }
}

void DatabaseWidget::closeSearch()
{
    Q_ASSERT(m_lastGroup);

    Q_EMIT listModeAboutToActivate();

    m_groupView->setCurrentGroup(m_lastGroup);
    m_searchTimer->stop();

    Q_EMIT listModeActivated();
}

void DatabaseWidget::showSearch()
{
    Q_EMIT searchModeAboutToActivate();

    m_searchUi->searchEdit->blockSignals(true);
    m_searchUi->searchEdit->clear();
    m_searchUi->searchEdit->blockSignals(false);

    m_searchUi->searchCurrentRadioButton->blockSignals(true);
    m_searchUi->searchRootRadioButton->blockSignals(true);
    m_searchUi->searchRootRadioButton->setChecked(true);
    m_searchUi->searchCurrentRadioButton->blockSignals(false);
    m_searchUi->searchRootRadioButton->blockSignals(false);

    m_lastGroup = m_groupView->currentGroup();

    Q_ASSERT(m_lastGroup);

    if (m_lastGroup == m_db->rootGroup()) {
        m_searchUi->optionsWidget->hide();
        m_searchUi->searchCurrentRadioButton->hide();
        m_searchUi->searchRootRadioButton->hide();
    }
    else {
        m_searchUi->optionsWidget->show();
        m_searchUi->searchCurrentRadioButton->show();
        m_searchUi->searchRootRadioButton->show();
        m_searchUi->searchCurrentRadioButton->setText(tr("Current group")
                                                      .append(" (")
                                                      .append(m_lastGroup->name())
                                                      .append(")"));
    }
    m_groupView->setCurrentIndex(QModelIndex());

    m_searchWidget->show();
    search();
    m_searchUi->searchEdit->setFocus();

    Q_EMIT searchModeActivated();
}

void DatabaseWidget::search()
{
    Q_ASSERT(m_lastGroup);

    Group* searchGroup;
    if (m_searchUi->searchCurrentRadioButton->isChecked()) {
        searchGroup = m_lastGroup;
    }
    else if (m_searchUi->searchRootRadioButton->isChecked()) {
        searchGroup = m_db->rootGroup();
    }
    else {
        Q_ASSERT(false);
        return;
    }

    Qt::CaseSensitivity sensitivity;
    if (m_searchUi->caseSensitiveCheckBox->isChecked()) {
        sensitivity = Qt::CaseSensitive;
    }
    else {
        sensitivity = Qt::CaseInsensitive;
    }

    QList<Entry*> searchResult = EntrySearcher().search(m_searchUi->searchEdit->text(), searchGroup, sensitivity);

    m_entryView->setEntryList(searchResult);
}

void DatabaseWidget::startSearchTimer()
{
    if (!m_searchTimer->isActive()) {
        m_searchTimer->stop();
    }
    m_searchTimer->start(100);
}

void DatabaseWidget::startSearch()
{
    if (!m_searchTimer->isActive()) {
        m_searchTimer->stop();
    }
    search();
}

void DatabaseWidget::emitGroupContextMenuRequested(const QPoint& pos)
{
    Q_EMIT groupContextMenuRequested(m_groupView->viewport()->mapToGlobal(pos));
}

void DatabaseWidget::emitEntryContextMenuRequested(const QPoint& pos)
{
    Q_EMIT entryContextMenuRequested(m_entryView->viewport()->mapToGlobal(pos));
}

bool DatabaseWidget::dbHasKey() const
{
    return m_db->hasKey();
}

bool DatabaseWidget::canDeleteCurrentGroup() const
{
    bool isRootGroup = m_db->rootGroup() == m_groupView->currentGroup();
    bool isRecycleBin = m_db->metadata()->recycleBin() == m_groupView->currentGroup();
    return !isRootGroup && !isRecycleBin;
}

bool DatabaseWidget::isInSearchMode() const
{
    return m_entryView->inEntryListMode();
}

void DatabaseWidget::clearLastGroup(Group* group)
{
    if (group) {
        m_lastGroup = Q_NULLPTR;
        m_searchWidget->hide();
    }
}

void DatabaseWidget::lock()
{
    Q_ASSERT(currentMode() != DatabaseWidget::LockedMode);
    if (isInSearchMode()) {
        closeSearch();
    }

    if (m_groupView->currentGroup()) {
        m_groupBeforeLock = m_groupView->currentGroup()->uuid();
    }
    else {
        m_groupBeforeLock = m_db->rootGroup()->uuid();
    }

    clearAllWidgets();
    m_unlockDatabaseWidget->load(m_filename);
    setCurrentWidget(m_unlockDatabaseWidget);
    Database* newDb = new Database();
    newDb->metadata()->setName(m_db->metadata()->name());
    replaceDatabase(newDb);
}

void DatabaseWidget::updateFilename(const QString& fileName)
{
    m_filename = fileName;
}

int DatabaseWidget::numberOfSelectedEntries() const
{
    return m_entryView->numberOfSelectedEntries();
}

QStringList DatabaseWidget::customEntryAttributes() const
{
    Entry* entry = m_entryView->currentEntry();
    if (!entry) {
        return QStringList();
   }
    return entry->attributes()->customKeys();
}

bool DatabaseWidget::isGroupSelected() const
{
    return m_groupView->currentGroup() != Q_NULLPTR;
  }

bool DatabaseWidget::currentEntryHasTitle()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return false;
  }
    return !currentEntry->title().isEmpty();
  }

bool DatabaseWidget::currentEntryHasUsername()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return false;
}
    return !currentEntry->username().isEmpty();
}

bool DatabaseWidget::currentEntryHasPassword()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return false;
}
    return !currentEntry->password().isEmpty();
}

bool DatabaseWidget::currentEntryHasUrl()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return false;
    }
    return !currentEntry->url().isEmpty();
}

bool DatabaseWidget::currentEntryHasNotes()
{
    Entry* currentEntry = m_entryView->currentEntry();
    if (!currentEntry) {
        Q_ASSERT(false);
        return false;
    }
    return !currentEntry->notes().isEmpty();
}

/**
 * @brief DatabaseWidget::syncDone executed when database sync finished
 */
void DatabaseWidget::syncDone() {
   if (syncCommand->getErrorCode()!=static_cast<int>(Errors::NO_ERROR)) {
       syncError(syncCommand->getErrorCode(), syncCommand->getErrorString());
   }
  typedef QSharedPointer < DatabaseSyncObject::SyncObject > SyncType;
   qRegisterMetaType < QSharedPointer < DatabaseSyncObject::SyncObject >>
    ("QSharedPointer<SyncObject>");
    qRegisterMetaType<SyncType>("SyncType");
  Q_ASSERT(syncCommand->getResult().length() == 1);
  QVariant vSyncObject = syncCommand->getResult().at(0);
  QSharedPointer<SyncObject> syncObject = vSyncObject.value<SyncType>();

  qDebug() << "Successfully synced database on " + QDateTime::currentDateTime().toString();
  // number of entries which was modified locally.
  int localModifiedEntries = syncObject->get(SEntry(), SMissing(),SLocal()) +
  syncObject->get(SGroup(), SMissing(),SLocal()) +
  syncObject->get(SEntry(), SOlder(),SLocal()) +
  syncObject->get(SGroup(), SOlder(),SLocal()) +
  syncObject->get(SEntry(), SRemoved(),SLocal()) +
  syncObject->get(SGroup(), SRemoved(),SLocal());
  // number of entries which should be modified remotely
  int remoteModifiedEntries = syncObject->get(SEntry(), SMissing(),SLocal()) +
  syncObject->get(SGroup(), SMissing(),SRemote()) +
  syncObject->get(SEntry(), SOlder(),SRemote()) +
  syncObject->get(SGroup(), SOlder(),SRemote()) +
  syncObject->get(SEntry(), SRemoved(),SRemote()) +
  syncObject->get(SGroup(), SRemoved(),SRemote());

  if (localModifiedEntries > 0 || syncObject->getRemoteModificationDate().toTime_t() > syncObject->getLocalModificationDate().toTime_t()) {
    // save local database. Assume last modified was updated during sync??
    qDebug() << "Saving local database after sync because it's older or has missing entries";
    qDebug() <<QString("modified=%1, localTime = %2, remoteTime = %3").arg(localModifiedEntries).arg(syncObject->getLocalModificationDate().toLocalTime().toString()).arg(syncObject->getRemoteModificationDate().toLocalTime().toString());
    Q_EMIT requestSaveDatabase(m_db, false);
  }

  if (remoteModifiedEntries > 0) {
      qDebug() << "Saving remote database after sync because it's older or has missing entries";
      saveDatabaseToCloud(m_db, m_filename);
  }

  // getRemoteModificationDate() could be not valid in the case when remote database not present in the cloud.
  if (!syncObject->getRemoteModificationDate().isValid()) {
      qDebug() << "Saving remote database to the cloud for the first time.";
      saveDatabaseToCloud(m_db, m_filename);
  }

  if (remoteModifiedEntries ==0 && syncObject->getRemoteModificationDate() < syncObject->getLocalModificationDate()) {
    // user just touched local database without updating data. Take care about remote db date
    // Just update remote last modification date without uploading new revision
    qDebug() << "Updating modification date of the remote database";
  }
}
/**
 * @brief DatabaseOpenWidget::syncError shows warning to user that sync was not done correctly
 */
void DatabaseWidget::syncError(int ErrorType, QString description) {
  Q_ASSERT(ErrorType);
  Q_ASSERT(!description.isNull());
  QMessageBox::warning(this, tr("Error"),
                       tr(
                         "Unable to sync database with remote source: \n%1: %2").arg(
                         QString::number(
                           ErrorType),
                         description));
}

/**
 * @brief DatabaseWidget::setLastModified updates last modified time of the database
 * @param accepted
 */
void DatabaseWidget::setLastModified(bool accepted) {
  accepted?m_db->metadata()->setLastModifiedDate(QDateTime::currentDateTime()): void();
}

/**
 * @brief DatabaseWidget::switchToCloudDbOpen switches current widget to the databaseOpenWidgetCloud
 */
void DatabaseWidget::switchToCloudDbOpen()
{
    m_databaseOpenWidgetCloud->loadSupportedCloudEngines();
    setCurrentWidget(m_databaseOpenWidgetCloud);
}

/**
 * @brief DatabaseWidget::cloudDbOpen  emits a signal
 *  that db from the cloud was open by databaseOpenWidgetCloud
 * @param dbPath
 */
void DatabaseWidget::cloudDbOpen(QString dbPath) {
Q_ASSERT(!dbPath.isNull() && !dbPath.isEmpty());
qDebug() <<"DB was selected and passed to a slot::"+dbPath;
//Pass path to the downloaded database and key to dbStruct array to get this struct from another widget
Q_EMIT cloudDbSelected(dbPath,m_db);

}

/**
 * @brief DatabaseWidget::rejectDb emits a signal when user cancelled cloud db open  dialog
 */
void DatabaseWidget::rejectDb() {
    qDebug() << "Emitting cloudDbRejected";
    Q_EMIT cloudDbRejected(m_db);

}

/**
 * @brief DatabaseWidget::syncDatabase performs sync of the currently open database with database in the cloud
 * local database file will be updated as well to prevent loosing changes
 */
void DatabaseWidget::syncDatabase() {
    // Perfom asyncronous sync of recent database with remote database if
    // GOOGLE_DRIVE_SYNC feature enabled through CMAKE
    if (QString(GOOGLE_DRIVE_SYNC) != "ON") return;
      // write general method to run commands in thread
      syncCommand->executeAsync(OptionsBuilder().addOption(OPTION_DB_POINTER,m_db).addOption(OPTION_ABSOLUTE_DB_NAME,m_filename).build());
      //Changes will be applied to the already opened database
      //typically it will take 1-2 seconds while user will browse to an item
      //TODO prevent or warn user copy the value if database is still in sync
}

/**
 * @brief DatabaseWidget::emitDatabaseSaved emits a signal database was saved.
 * Used to connect saveDatabase in DatabaseTabWidget with connected slots in DatabaseWidget
 * @param db database which was saved
 * @param filePath path to the local database file
 * @param saveToCloud should signal trigger saving to the cloud.
 *   In such cases when local databases saved locally after sync with newer remote database
 *   we don't need to save it again remotely
 */
void DatabaseWidget::emitDatabaseSaved(Database *db, const QString filePath) {
    Q_EMIT databaseSaved(db, filePath);
}

/**
 * @brief DatabaseWidget::saveDatabaseToCloud - uploads current version of the database to the selected cloud
 * @param db - database which will be loaded to cloud
 */
void DatabaseWidget::saveDatabaseToCloud(Database *db, const QString filePath)
{
  // Perfom asyncronous upload of the local database to the cloud
  // GOOGLE_DRIVE_SYNC feature enabled through CMAKE
  if (QString(GOOGLE_DRIVE_SYNC) != "ON") return;
  uploadCommand->executeAsync(OptionsBuilder().addOption(OPTION_ABSOLUTE_DB_NAME,
                                                       filePath).addOption(
                              OPTION_LAST_MODIFIED_TIME,
                              db->metadata()->
                              lastModifiedDate()).addOption(OPTION_PARENT_NAME, syncCloudDir).build());
}


//void DatabaseWidget::showSyncLoginPage(QWebView* view) {
//view->show();
//}

