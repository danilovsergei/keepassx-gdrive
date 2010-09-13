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

#ifndef KEEPASSX_ENDIAN_H
#define KEEPASSX_ENDIAN_H

#include <QtCore/QByteArray>
#include <QtCore/QSysInfo>

class QIODevice;

namespace Endian
{
    qint16 bytesToInt16(const QByteArray& ba, QSysInfo::Endian byteOrder);
    quint16 bytesToUInt16(const QByteArray& ba, QSysInfo::Endian byteOrder);
    qint32 bytesToInt32(const QByteArray& ba, QSysInfo::Endian byteOrder);
    quint32 bytesToUInt32(const QByteArray& ba, QSysInfo::Endian byteOrder);
    qint64 bytesToInt64(const QByteArray& ba, QSysInfo::Endian byteOrder);
    quint64 bytesToUInt64(const QByteArray& ba, QSysInfo::Endian byteOrder);

    qint16 readInt16(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
    quint16 readUInt16(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
    qint32 readInt32(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
    quint32 readUInt32(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
    qint64 readInt64(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
    quint64 readUInt64(QIODevice* device, QSysInfo::Endian byteOrder, bool* ok);
};

#endif // KEEPASSX_ENDIAN_H
