#include "GDriveUploadDbHelperTest.h"
#include <QtCore/QFile>
QTEST_GUILESS_MAIN(GDriveUploadDbHelperTest)

void GDriveUploadDbHelperTest::testDbUpload() {
//KeePass2XmlWriter writer;
//QTemporaryFile file;
//QString fileName;
//if (file.open()) {
//        fileName = file.fileName();
//    }
//qDebug() << "filename = "+fileName;
Database* db = GDriveTestUtils().createLocalDatabase("/tmp/testdbgfgfgf.kdbx");
QFile dbfile("/tmp/testdbgfgfgf.kdbx");
dbfile.open(QIODevice::ReadWrite);
QByteArray md5;
md5 = QCryptographicHash::hash(dbfile.readAll(),QCryptographicHash::Md5).toHex();
qDebug() << md5;
dbfile.close();

GDriveTestUtils().saveDatabase(db,"/tmp/testdbgfgfgf.kdbx");
dbfile.open(QIODevice::ReadWrite);
md5 = QCryptographicHash::hash(dbfile.readAll(),QCryptographicHash::Md5).toHex();
qDebug() << md5;
qDebug() << QCryptographicHash::hash(dbfile.readAll(),QCryptographicHash::Md5).toHex();

dbfile.close();



//writer.writeDatabase(fileName,db);
}


void GDriveUploadDbHelperTest::initTestCase() {
      Crypto::init();
}
