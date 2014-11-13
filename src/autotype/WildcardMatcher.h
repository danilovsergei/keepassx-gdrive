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

#ifndef KEEPASSX_WILDCARDMATCHER_H
#define KEEPASSX_WILDCARDMATCHER_H

#include <QtCore/QObject>

#include "core/Global.h"

class WildcardMatcher : public QObject
{
    Q_OBJECT

public:
    explicit WildcardMatcher(QString text, QObject* parent = Q_NULLPTR);

    static const QString Wildcard;

    bool match(QString pattern);

private:
    static const Qt::CaseSensitivity Sensitivity;

    QString m_text;
    QString m_pattern;
    bool patternEqualsText();
    bool patternContainsWildcard();
    bool matchWithWildcards();
    bool startOrEndDoesNotMatch(QStringList parts);
    bool partsMatch(QStringList parts);
    int getMatchIndex(QString part, int startIndex);
    bool noMatchFound(int index);
    int calculateNewIndex(int matchIndex, int partLength);
};

#endif // KEEPASSX_WILDCARDMATCHER_H
