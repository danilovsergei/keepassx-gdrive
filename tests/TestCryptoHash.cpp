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

#include <QtTest/QTest>

#include "crypto/Crypto.h"
#include "crypto/CryptoHash.h"

class TestCryptoHash : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void test();
};

void TestCryptoHash::initTestCase()
{
    Crypto::init();
}

void TestCryptoHash::test()
{
    // TODO move somewhere else
    QVERIFY(Crypto::selfTest());

    CryptoHash cryptoHash1(CryptoHash::Sha256);
    QCOMPARE(QString(cryptoHash1.result().toHex()),
             QString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));

    QByteArray source2 = QString("KeePassX").toAscii();
    QString result2 = CryptoHash::hash(source2, CryptoHash::Sha256).toHex();
    QCOMPARE(result2, QString("0b56e5f65263e747af4a833bd7dd7ad26a64d7a4de7c68e52364893dca0766b4"));

    CryptoHash cryptoHash3(CryptoHash::Sha256);
    cryptoHash3.addData(QString("KeePa").toAscii());
    cryptoHash3.addData(QString("ssX").toAscii());
    QCOMPARE(QString(cryptoHash3.result().toHex()),
             QString("0b56e5f65263e747af4a833bd7dd7ad26a64d7a4de7c68e52364893dca0766b4"));
}

QTEST_MAIN(TestCryptoHash);

#include "TestCryptoHash.moc"
