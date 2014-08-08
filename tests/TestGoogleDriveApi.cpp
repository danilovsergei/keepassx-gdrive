#include "TestGoogleDriveApi.h"
#include <QtCore/QDebug>
#include "crypto/Crypto.h"
#include <QtTest/QTest>
#include "tests.h"
#include <QtTest/QSignalSpy>
#include <QtCore/QMetaType>
#include "qtdrive/lib/command_upload_file.h"
#include "qtdrive/lib/command_update.h"
#include "qtdrive/lib/command_delete.h"
#include "core/Database.h"
#include "core/Entry.h"
#include "core/Group.h"
#include <core/qsavefile.h>
#include <format/KeePass2Writer.h>
#include "core/Metadata.h"
#include <QtCore/QDir>
#include "keys/PasswordKey.h"
#include <QtCore/QFile>
#include "core/Uuid.h"
#include "gdrive/GoogleDriveSession.h"
#include "gdrive/helpers/GDriveGetDbHelper.h"
#include "gdrive/GoogleDriveTools.h"
#include "core/Tools.h"

Q_DECLARE_METATYPE(FileInfo)
Q_DECLARE_METATYPE(GoogleDrive::FileInfoList)


using namespace GoogleDrive;
void TestGoogleDriveApi::initTestCase() {
    Crypto::init();
    gdrive=googleDriveApi();
    parentDir="0B8tltL21wts3alc0eS00UVRLNnc";
    dbName="test";
}

void TestGoogleDriveApi::cleanupTestCase() {
 delete gdrive;
}

/**
 * @brief TestGoogleDriveApi::testGetDatabasesHHelper - tests helper class method  getDatabases() properly returns fake database using specified filter
 */
void TestGoogleDriveApi::testGetDatabasesHHelper() {


    const QString fileName=createLocalDatabase();
    uploadDb(this->dbName,fileName);

    //List all databases with name test.kdbx at google drive
    qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName+".kdbx"));
    filter.append(filterEntry);
    GoogleDrive::FileInfoList dbList=GDriveGetDbHelper(filter).getDatabases();
    qDebug() << "Get Db List";
    QVERIFY2(dbList.size()==1,"Only one test database expected by test design");
    QCOMPARE(dbList.first().title(),QString(dbName+".kdbx"));

    deleteDb(dbList.first());

}






const QString TestGoogleDriveApi::createLocalDatabase() {
    QString dbName("test");
    Database* db1= new Database();
    CompositeKey key;
    key.addKey(PasswordKey("test"));
    db1->setKey(key);
    db1->metadata()->setName(dbName);

    Entry* entry = new Entry();
    entry->setGroup(db1->resolveGroup(db1->rootGroup()->uuid()));
    entry->setTitle("NewEntry");
    entry->setPassword("TestPassword");
    entry->setUuid(Uuid::random());

    //save fake database locally
    KeePass2Writer m_writer;
    QString fileName(QDir::tempPath()+QDir::separator()+dbName+".kdbx");
    qDebug() << "saving to "+fileName;
    QFile qFile(fileName);
    if (qFile.exists()) {
    qFile.remove();
    }
    Q_ASSERT(qFile.exists()==false);
    QSaveFile saveFile(fileName);
    bool result = false;
    if (saveFile.open(QIODevice::WriteOnly)) {
        m_writer.writeDatabase(&saveFile, db1);
        result = saveFile.commit();
    }
    //check database saved successfully
    Q_ASSERT(result==true);
    Q_ASSERT(qFile.exists()==true);

    delete entry;
    delete db1;
    return fileName;
}

void TestGoogleDriveApi::uploadDb(const QString& dbName,const QString& dbPath){
    //List all databases with name test.kdbx at google drive
    qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName+".kdbx"));
    filter.append(filterEntry);
    QVERIFY2(GDriveGetDbHelper(filter).getDatabases().size()==0,"Test db exists in google drive before test, but it should not");

    //upload new database or update existing one with revision if it exists
    QFileInfo file(dbPath);
    gdrive->uploadDatabase(dbPath,file.lastModified(),true,parentDir);

    //verify db was loaded successfully
    GoogleDrive::FileInfoList dbList1= GDriveGetDbHelper(filter).getDatabases();
    QVERIFY2(dbList1.size()==1,"Db was not loaded to Google Drive");
}

/**
 * @brief TestGoogleDriveApi::deleteDb - Internal test function used to delete database from cloud. Not needed yet by Keepassx application
 * @param db
 */
void TestGoogleDriveApi::deleteDb(FileInfo& db) {
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(db.title()));
    filter.append(filterEntry);

    CommandDelete* deleteCmd= new CommandDelete(getSession());
    deleteCmd->exec(db.id());

    if (!deleteCmd->waitForFinish(false)) {
    return;
    }

    QVERIFY2(GDriveGetDbHelper(filter).getDatabases().size()==0,"Db still exists in Google Drive");

    delete deleteCmd;
}

//TODO make test of uploading db with parent="" and make sure it uploads to root

/**
 * @brief TestGoogleDriveApi::testUpdateDb - uploads new test database to the cloud only if it does not exists before
 */
void TestGoogleDriveApi::testUploadNewDb() {
//create the fake database
Database* db1= new Database();
CompositeKey key;
key.addKey(PasswordKey("test"));
db1->setKey(key);
db1->metadata()->setName(dbName);

Entry* entry = new Entry();
entry->setGroup(db1->resolveGroup(db1->rootGroup()->uuid()));
entry->setTitle("NewEntry");
entry->setPassword("TestPassword");
entry->setUuid(Uuid::random());

//save fake database locally
KeePass2Writer m_writer;
QString fileName(QDir::tempPath()+QDir::separator()+dbName+".kdbx");
qDebug() << "saving to "+fileName;
QFile qFile(fileName);
if (qFile.exists()) {
qFile.remove();
}
QCOMPARE(qFile.exists(),false);
QSaveFile saveFile(fileName);
bool result = false;
if (saveFile.open(QIODevice::WriteOnly)) {
    m_writer.writeDatabase(&saveFile, db1);
    result = saveFile.commit();
}
//check database saved successfully
QCOMPARE(result,true);
QCOMPARE(qFile.exists(),true);

//List all databases with name test.kdbx at google drive
qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
QList<QueryEntry> filter;
QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName+".kdbx"));
filter.append(filterEntry);
QVERIFY2(GDriveGetDbHelper(filter).getDatabases().size()==0,"Test db exists in google drive before test, but it should't");

//upload new database or update existing one with revision if it exists
FileInfo fi;
fi.setTitle(dbName+".kdbx");
fi.setParents(QStringList(parentDir));

Tools::sleep(1000);

QSignalSpy spy(gdrive,SIGNAL(uploadDatabaseFinished()));
gdrive->uploadDatabase(fileName,QFileInfo(fileName).lastModified(),true);
int waitTime=0;
while (spy.length() == 0 && waitTime<10000) {
  qDebug()<< "spy count="+spy.length();
  QTest::qWait(200);
  waitTime+=200;
}


qDebug() << "verify db was loaded successfully";
//verify db was loaded successfully
GoogleDrive::FileInfoList dbList1= GDriveGetDbHelper(filter).getDatabases();
QCOMPARE(dbList1.size(),1);

//make sure database last modified time was update with our local file last modified date
QCOMPARE(dbList1.first().modifiedDate().toLocalTime(),QFileInfo(fileName).lastModified().toLocalTime());

qDebug()<<"Delete database";
deleteDb(dbList1.first());
QVERIFY2(GDriveGetDbHelper(filter).getDatabases().size()==0,"Db still exists in Google Drive");

}


/**
 * @brief TestGoogleDriveApi::testUpdateDb - updates database revision for the cloud database
 */
void TestGoogleDriveApi::testUpdateExistingDb() {
    //create the fake database
    Database* db1= new Database();
    CompositeKey key;
    key.addKey(PasswordKey("test"));
    db1->setKey(key);
    db1->metadata()->setName(dbName);

    Entry* entry = new Entry();
    entry->setGroup(db1->resolveGroup(db1->rootGroup()->uuid()));
    entry->setTitle("NewEntry");
    entry->setPassword("TestPassword");
    entry->setUuid(Uuid::random());

    //save fake database locally
    KeePass2Writer m_writer;
    QString fileName(QDir::tempPath()+QDir::separator()+dbName+".kdbx");
    qDebug() << "saving to "+fileName;
    QFile qFile(fileName);
    if (qFile.exists()) {
    qFile.remove();
    }
    QCOMPARE(qFile.exists(),false);
    QSaveFile saveFile(fileName);
    bool result = false;
    if (saveFile.open(QIODevice::WriteOnly)) {
        m_writer.writeDatabase(&saveFile, db1);
        result = saveFile.commit();
    }
    //check database saved successfully
    QCOMPARE(result,true);
    QCOMPARE(qFile.exists(),true);

    //List all databases with name test.kdbx at google drive
    qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
    QList<QueryEntry> filter;
    QueryEntry filterEntry(QueryEntry::QueryFilter::DBNAME,QStringList(dbName+".kdbx"));
    filter.append(filterEntry);
    QVERIFY2(GDriveGetDbHelper(filter).getDatabases().size()==0,"Test db exists in google drive before test, but it should't");

    //upload new db
    FileInfo fi;
    fi.setTitle(dbName+".kdbx");
    fi.setParents(QStringList(parentDir));

    QSignalSpy spy(gdrive,SIGNAL(uploadDatabaseFinished()));
    gdrive->uploadDatabase(fileName,db1->metadata()->lastModifiedDate(),true);
    int waitTime=0;
    while (spy.count() == 0 && waitTime<10000) {
      QTest::qWait(200);
      waitTime+=200;
    }

    //verify db was loaded successfully
    GoogleDrive::FileInfoList dbList1= GDriveGetDbHelper(filter).getDatabases();
    QVERIFY2(dbList1.size()==1,"Db was not loaded to Google Drive");

    //sleep before uploading database to make sure local and remote database timestamps could be different
    Tools::sleep(1000);

    //update database revision
    FileInfo newRev=dbList1.first();
    qDebug() << "Update database revision for "+newRev.id();
    gdrive->updateDatabase(fileName,newRev,QFileInfo(fileName).lastModified(),true);
    waitTime=0;
    while (spy.count() == 0 && waitTime<10000) {
      QTest::qWait(200);
      waitTime+=200;
    }

    //get updated database from gdrive
    GoogleDrive::FileInfoList dbList2= GDriveGetDbHelper(filter).getDatabases();

    //TODO add actual check revision was created by implementing https://developers.google.com/drive/v2/reference/revisions/list


    //make sure database last modified time was update with our local file last modified date
    QCOMPARE(dbList2.first().modifiedDate().toLocalTime(),QFileInfo(fileName).lastModified().toLocalTime());

    qDebug()<<"Delete database";
    deleteDb(newRev);
}



QTEST_GUILESS_MAIN(TestGoogleDriveApi)
