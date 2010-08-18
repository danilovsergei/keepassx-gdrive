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

#include "Entry.h"

#include "Database.h"
#include "Group.h"

Entry::Entry()
{
    m_group = 0;
    m_db = 0;
}

Entry::~Entry()
{
    // TODO notify group
}

Uuid Entry::uuid() const
{
    return m_uuid;
}

QImage Entry::icon() const
{
    Q_ASSERT(m_iconNumber != 0 || !m_customIcon.isNull());

    if (m_iconNumber == 0)
        return m_db->customIcon(m_customIcon);
    else
        return Database::icon(m_iconNumber);
}

QColor Entry::foregroundColor() const
{
    return m_foregroundColor;
}

QColor Entry::backgroundColor() const
{
    return m_backgroundColor;
}

QString Entry::overrideUrl() const
{
    return m_overrideUrl;
}

TimeInfo Entry::timeInfo() const
{
    return m_timeInfo;
}

bool Entry::autoTypeEnabled() const
{
    return m_autoTypeEnabled;
}

int Entry::autoTypeObfuscation() const
{
    return m_autoTypeObfuscation;
}

QString Entry::defaultAutoTypeSequence() const
{
    return m_defaultAutoTypeSequence;
}

const QList<AutoTypeAssociation>& Entry::autoTypeAssociations() const
{
    return m_autoTypeAssociations;
}

const QHash<QString, QString>& Entry::attributes() const
{
    return m_attributes;
}

const QHash<QString, QByteArray>& Entry::attachments() const
{
    return m_binaries;
}

QString Entry::title() const
{
    return m_attributes.value("Title");
}

QString Entry::url() const
{
    return m_attributes.value("URL");
}

QString Entry::username() const
{
    return m_attributes.value("UserName");
}

QString Entry::password() const
{
    return m_attributes.value("Password");
}

QString Entry::notes() const
{
    return m_attributes.value("Notes");
}

void Entry::setUuid(const Uuid& uuid)
{
    Q_ASSERT(!uuid.isNull());

    m_uuid = uuid;
}

void Entry::setIcon(int iconNumber)
{
    Q_ASSERT(iconNumber >= 0);

    m_iconNumber = iconNumber;
    m_customIcon = Uuid();
}

void Entry::setIcon(const Uuid& uuid)
{
    Q_ASSERT(!uuid.isNull());

    m_iconNumber = 0;
    m_customIcon = uuid;
}

void Entry::setForegroundColor(const QColor& color)
{
    m_foregroundColor = color;
}

void Entry::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
}

void Entry::setOverrideUrl(const QString& url)
{
    m_overrideUrl = url;
}

void Entry::setTimeInfo(const TimeInfo& timeInfo)
{
    m_timeInfo = timeInfo;
}

void Entry::setAutoTypeEnabled(bool enable)
{
    m_autoTypeEnabled = enable;
}

void Entry::setAutoTypeObfuscation(int obfuscation)
{
    m_autoTypeObfuscation = obfuscation;
}

void Entry::setDefaultAutoTypeSequence(const QString& sequence)
{
    m_defaultAutoTypeSequence = sequence;
}

void Entry::addAutoTypeAssociation(const AutoTypeAssociation& assoc)
{
    m_autoTypeAssociations << assoc;
}

void Entry::addAttribute(const QString& key, const QString& value)
{
    m_attributes.insert(key, value);

    // TODO add all visible oclumns
    if (key == "Title") {
        Q_EMIT dataChanged(this);
    }
}

void Entry::addAttachment(const QString& key, const QByteArray& value)
{
    m_binaries.insert(key, value);
}

void Entry::setTitle(const QString& title)
{
    addAttribute("Title", title);
}

void Entry::setUrl(const QString& url)
{
    addAttribute("URL", url);
}

void Entry::setUsername(const QString& username)
{
    addAttribute("UserName", username);
}

void Entry::setPassword(const QString& password)
{
    addAttribute("Password", password);
}

void Entry::setNotes(const QString& notes)
{
    addAttribute("Notes", notes);
}

void Entry::setGroup(Group* group)
{
    if (m_group) {
        m_group->removeEntry(this);
    }
    group->addEntry(this);
    m_group = group;
    m_db = group->database();
    QObject::setParent(group);
}
