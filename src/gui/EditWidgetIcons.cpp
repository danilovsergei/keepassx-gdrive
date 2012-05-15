/*
 *  Copyright (C) 2012 Felix Geyer <debfx@fobos.de>
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

#include "EditWidgetIcons.h"
#include "ui_EditWidgetIcons.h"

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include "core/Group.h"
#include "core/Metadata.h"
#include "core/Tools.h"
#include "gui/IconModels.h"

IconStruct::IconStruct()
    : uuid(Uuid())
    , number(0)
{
}

EditWidgetIcons::EditWidgetIcons(QWidget *parent)
    : QWidget(parent)
    ,  m_ui(new Ui::EditWidgetIcons())
{
    m_ui->setupUi(this);

    m_defaultIconModel = new DefaultIconModel(this);
    m_ui->defaultIconsView->setModel(m_defaultIconModel);

    m_customIconModel = new CustomIconModel(this);
    m_ui->customIconsView->setModel(m_customIconModel);

    connect(m_ui->defaultIconsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(updateRadioButtonDefaultIcons()));
    connect(m_ui->customIconsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(updateRadioButtonCustomIcons()));
    connect(m_ui->defaultIconsRadio, SIGNAL(toggled(bool)),
            this, SLOT(updateWidgetsDefaultIcons(bool)));
    connect(m_ui->customIconsRadio, SIGNAL(toggled(bool)),
            this, SLOT(updateWidgetsCustomIcons(bool)));
    connect(m_ui->addButton, SIGNAL(clicked()), SLOT(addCustomIcon()));
    connect(m_ui->deleteButton, SIGNAL(clicked()), SLOT(removeCustomIcon()));
}

EditWidgetIcons::~EditWidgetIcons()
{
}

IconStruct EditWidgetIcons::save()
{
    IconStruct iconStruct;
    if (m_ui->defaultIconsRadio->isChecked()) {
        QModelIndex index = m_ui->defaultIconsView->currentIndex();
        if (index.isValid()) {
            iconStruct.number = index.row();
        }
        else {
            iconStruct.number = 0;
        }
    }
    else {
        QModelIndex index = m_ui->customIconsView->currentIndex();
        if (index.isValid()) {
            iconStruct.uuid = m_customIconModel->uuidFromIndex(m_ui->customIconsView->currentIndex());
        }
        else {
            iconStruct.number = 0;
        }
    }

    return iconStruct;
}

void EditWidgetIcons::load(Uuid currentUuid, Database* database, IconStruct iconStruct)
{
    m_database = database;
    m_currentUuid = currentUuid;

    if (database) {
        this->setEnabled(true);

        m_customIconModel->setIcons(database->metadata()->customIcons(),
                                    database->metadata()->customIconsOrder());

        Uuid iconUuid = iconStruct.uuid;
        if (iconUuid.isNull()) {
            int iconNumber = iconStruct.number;
            m_ui->defaultIconsView->setCurrentIndex(m_defaultIconModel->index(iconNumber, 0));
            m_ui->defaultIconsRadio->setChecked(true);
        }
        else {
            QModelIndex index = m_customIconModel->indexFromUuid(iconUuid);
            if (index.isValid()) {
                m_ui->customIconsView->setCurrentIndex(index);
                m_ui->customIconsRadio->setChecked(true);
            }
            else {
                m_ui->defaultIconsView->setCurrentIndex(m_defaultIconModel->index(0, 0));
                m_ui->defaultIconsRadio->setChecked(true);
            }
        }
    }
    else {
        this->setEnabled(false);
    }
}

void EditWidgetIcons::addCustomIcon()
{
    if (m_database) {
        QString filter = QString("%1 (%2);;%3 (*.*)").arg(tr("Images"),
                    Tools::imageReaderFilter(), tr("All files"));

        QString filename = QFileDialog::getOpenFileName(
                    this, tr("Select Image"), "", filter);
        if (!filename.isEmpty()) {
            QImage image(filename);
            if (!image.isNull()) {
                Uuid uuid = Uuid::random();
                m_database->metadata()->addCustomIcon(uuid, image.scaled(16, 16));
                m_customIconModel->setIcons(m_database->metadata()->customIcons(),
                                            m_database->metadata()->customIconsOrder());
                QModelIndex index = m_customIconModel->indexFromUuid(uuid);
                m_ui->customIconsView->setCurrentIndex(index);
            }
            else {
                // TODO: show error
            }
        }
    }
}

void EditWidgetIcons::removeCustomIcon()
{
    if (m_database) {
        QModelIndex index = m_ui->customIconsView->currentIndex();
        if (index.isValid()) {
            Uuid iconUuid = m_customIconModel->uuidFromIndex(index);
            int iconUsedCount = 0;

            QList<Entry*> allEntries = m_database->rootGroup()->entriesRecursive(false);
            Q_FOREACH (const Entry* entry, allEntries) {
                if (iconUuid == entry->iconUuid() && m_currentUuid != entry->uuid()) {
                    iconUsedCount++;
                }
            }

            QList<const Group*> allGroups = m_database->rootGroup()->groupsRecursive(true);
            Q_FOREACH (const Group* group, allGroups) {
                if (iconUuid == group->iconUuid() && m_currentUuid != group->uuid()) {
                    iconUsedCount++;
                }
            }

            if (iconUsedCount == 0) {
                m_database->metadata()->removeCustomIcon(iconUuid);
                // TODO update icons of history items
                // with updateTimeinfo = false
                m_customIconModel->setIcons(m_database->metadata()->customIcons(),
                                            m_database->metadata()->customIconsOrder());
                if (m_customIconModel->rowCount() > 0) {
                    m_ui->customIconsView->setCurrentIndex(m_customIconModel->index(0, 0));
                }
                else {
                    updateRadioButtonDefaultIcons();
                }
            }
            else {
                QMessageBox::information(this, tr("Can't delete icon!"),
                                         tr("Can't delete icon. Still used by %1 items.")
                                         .arg(iconUsedCount));
            }
        }
    }
}

void EditWidgetIcons::updateWidgetsDefaultIcons(bool check)
{
    if (check) {
        QModelIndex index = m_ui->defaultIconsView->currentIndex();
        if (!index.isValid()) {
            m_ui->defaultIconsView->setCurrentIndex(m_defaultIconModel->index(0, 0));
        }
        else {
            m_ui->defaultIconsView->setCurrentIndex(index);
        }
        m_ui->customIconsView->selectionModel()->clearSelection();
        m_ui->addButton->setEnabled(false);
        m_ui->deleteButton->setEnabled(false);
    }
}

void EditWidgetIcons::updateWidgetsCustomIcons(bool check)
{
    if (check) {
        QModelIndex index = m_ui->customIconsView->currentIndex();
        if (!index.isValid()) {
            m_ui->customIconsView->setCurrentIndex(m_customIconModel->index(0, 0));
        }
        else {
            m_ui->customIconsView->setCurrentIndex(index);
        }
        m_ui->defaultIconsView->selectionModel()->clearSelection();
        m_ui->addButton->setEnabled(true);
        m_ui->deleteButton->setEnabled(true);
    }
}

void EditWidgetIcons::updateRadioButtonDefaultIcons()
{
    m_ui->defaultIconsRadio->setChecked(true);
}

void EditWidgetIcons::updateRadioButtonCustomIcons()
{
    m_ui->customIconsRadio->setChecked(true);
}
