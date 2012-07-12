/*
 *  Copyright (C) 2012 Tobias Tangemann
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

#ifndef KEEPASSX_APPLICATION_H
#define KEEPASSX_APPLICATION_H

#include <QtGui/QApplication>

#include "core/Global.h"

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int& argc, char** argv);

    bool event(QEvent* event) Q_DECL_OVERRIDE;
#ifdef Q_WS_X11
    bool x11EventFilter(XEvent* event) Q_DECL_OVERRIDE;
#endif

Q_SIGNALS:
    void openFile(const QString& filename);
};

#endif // KEEPASSX_APPLICATION_H
