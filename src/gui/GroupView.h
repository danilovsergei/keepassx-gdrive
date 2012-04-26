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

#ifndef KEEPASSX_GROUPVIEW_H
#define KEEPASSX_GROUPVIEW_H

#include <QtGui/QTreeView>

class Database;
class Group;
class GroupModel;

class GroupView : public QTreeView
{
    Q_OBJECT

public:
    explicit GroupView(Database* db, QWidget* parent = 0);
    void setModel(QAbstractItemModel* model);
    Group* currentGroup();

Q_SIGNALS:
    void groupChanged(Group* group);

private Q_SLOTS:
    void expandedChanged(const QModelIndex& index);
    void emitGroupChanged(const QModelIndex& index);
    void emitGroupChanged();

protected:
    void dragMoveEvent(QDragMoveEvent* event);

private:
    void recInitExpanded(Group* group);

    GroupModel* const m_model;
};

#endif // KEEPASSX_GROUPVIEW_H
