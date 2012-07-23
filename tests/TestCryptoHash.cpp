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

#include "TestCryptoHash.h"

#include <QtTest/QTest>

#include "tests.h"
#include "crypto/Crypto.h"
#include "crypto/CryptoHash.h"

void TestCryptoHash::initTestCase()
{
    Crypto::init();
}

void TestCryptoHash::test()
{
    // TODO: move somewhere else
    QVERIFY(Crypto::selfTest());

    CryptoHash cryptoHash1(CryptoHash::Sha256);
    QCOMPARE(cryptoHash1.result(),
             QByteArray::fromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));

    QByteArray source2 = QString("KeePassX").toAscii();
    QByteArray result2 = CryptoHash::hash(source2, CryptoHash::Sha256);
    QCOMPARE(result2, QByteArray::fromHex("0b56e5f65263e747af4a833bd7dd7ad26a64d7a4de7c68e52364893dca0766b4"));

    CryptoHash cryptoHash3(CryptoHash::Sha256);
    cryptoHash3.addData(QString("KeePa").toAscii());
    cryptoHash3.addData(QString("ssX").toAscii());
    QCOMPARE(cryptoHash3.result(),
             QByteArray::fromHex("0b56e5f65263e747af4a833bd7dd7ad26a64d7a4de7c68e52364893dca0766b4"));
}

QTEST_GUILESS_MAIN(TestCryptoHash)
