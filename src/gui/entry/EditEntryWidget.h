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

#ifndef KEEPASSX_EDITENTRYWIDGET_H
#define KEEPASSX_EDITENTRYWIDGET_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QScopedPointer>

#include "gui/EditWidget.h"

class Database;
class EditWidgetIcons;
class Entry;
class EntryAttachments;
class EntryAttachmentsModel;
class EntryAttributes;
class EntryAttributesModel;
class EntryHistoryModel;
class QStackedLayout;

namespace Ui {
    class EditEntryWidgetAdvanced;
    class EditEntryWidgetMain;
    class EditEntryWidgetHistory;
    class EditEntryWidgetNotes;
    class EditWidget;
}

class EditEntryWidget : public EditWidget
{
    Q_OBJECT

public:
    explicit EditEntryWidget(QWidget* parent = 0);
    ~EditEntryWidget();

    void loadEntry(Entry* entry, bool create, bool history, const QString& groupName,
                   Database *database);

    static const QColor CorrectSoFarColor;
    static const QColor ErrorColor;

Q_SIGNALS:
    void editFinished(bool accepted);
    void historyEntryActivated(Entry* entry);

private Q_SLOTS:
    void saveEntry();
    void cancel();
    void togglePassword(bool checked);
    void setPasswordCheckColors();
    void insertAttribute();
    void editCurrentAttribute();
    void removeCurrentAttribute();
    void updateCurrentAttribute();
    void insertAttachment();
    void saveCurrentAttachment();
    void removeCurrentAttachment();
    void showHistoryEntry();
    void deleteHistoryEntry();
    void emitHistoryEntryActivated(const QModelIndex &index);
    void updateHistoryButtons(const QModelIndex& current, const QModelIndex& previous);

private:
    bool passwordsEqual();
    void setForms(const Entry* entry);

    Entry* m_entry;
    Database* m_database;

    bool m_create;
    bool m_history;
    const QScopedPointer<Ui::EditEntryWidgetMain> m_mainUi;
    const QScopedPointer<Ui::EditEntryWidgetNotes> m_notesUi;
    const QScopedPointer<Ui::EditEntryWidgetAdvanced> m_advancedUi;
    const QScopedPointer<Ui::EditEntryWidgetHistory> m_historyUi;
    QWidget* const m_mainWidget;
    QWidget* const m_notesWidget;
    QWidget* const m_advancedWidget;
    EditWidgetIcons* const m_iconsWidget;
    QWidget* const m_historyWidget;
    EntryAttachmentsModel* m_attachmentsModel;
    EntryAttributesModel* m_attributesModel;
    EntryHistoryModel* m_historyModel;
    EntryAttachments* m_entryAttachments;
    EntryAttributes* m_entryAttributes;
    QPersistentModelIndex m_currentAttribute;

    Q_DISABLE_COPY(EditEntryWidget)
};

#endif // KEEPASSX_EDITENTRYWIDGET_H
