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

#ifndef KEEPASSX_SYMMETRICCIPHER_H
#define KEEPASSX_SYMMETRICCIPHER_H

#include <QtCore/QByteArray>

class SymmetricCipherBackend;

class SymmetricCipher
{
public:
    enum Algorithm
    {
        Aes256,
        Salsa20
    };

    enum Mode
    {
        Cbc,
        Ecb,
        Stream
    };

    enum Direction
    {
        Decrypt,
        Encrypt
    };

    SymmetricCipher(SymmetricCipher::Algorithm algo, SymmetricCipher::Mode mode,
                    SymmetricCipher::Direction direction, const QByteArray& key, const QByteArray& iv);
    ~SymmetricCipher();

    QByteArray process(const QByteArray& data);
    void processInPlace(QByteArray& data);

    void reset();
    int blockSize() const;

private:
    SymmetricCipherBackend* m_backend;

    Q_DISABLE_COPY(SymmetricCipher)
};

#endif // KEEPASSX_SYMMETRICCIPHER_H
