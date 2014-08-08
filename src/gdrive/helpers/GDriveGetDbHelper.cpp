#include "GDriveGetDbHelper.h"
Q_DECLARE_METATYPE(FileInfo)
Q_DECLARE_METATYPE(GoogleDrive::FileInfoList)

GDriveGetDbHelper::GDriveGetDbHelper(const QList<QueryEntry>& queryFilter)

{
    qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
    //connect(googleDriveApi(),SIGNAL(DbListLoaded(GoogleDrive::FileInfoList)),this,SLOT(DbListLoaded(GoogleDrive::FileInfoList)));
    this->dbList=googleDriveApi()->getDatabasesSeq(queryFilter);
    this->loaded=true;
}

void GDriveGetDbHelper::waitForFinish() {
    QEventLoop qE;
    QTimer tT;
    tT.setSingleShot(true);
    connect(googleDriveApi(),SIGNAL(DbListLoaded(GoogleDrive::FileInfoList)),&qE,SLOT(quit()));
    connect(&tT,SIGNAL(timeout()),&qE,SLOT(quit()));

    qDebug() << "Start timer!!";

    tT.start(10000);
    //execute local loop and wait until database will be uploaded
    qDebug() << "Before eventlopp";
    if (this->loaded) {
        qDebug() << "We are done!!!!";
    }
    qE.exec();
    qDebug() << "Qeventlopp stopped";
    if (tT.isActive()) {
    //request finished earlier than timer timeout.Just stop the timer
        tT.stop();
        qDebug() << "Timer stopped";

    }
    else {
        qDebug() <<"Fail to get databases from google drive in 10 seconds";
        //TODO raise exception here
    }
}
const GoogleDrive::FileInfoList GDriveGetDbHelper::getDatabases() {
        //TODO investigate whether my own waitForFinish+QConcurrent will work better than waitForFinish built in GoogleDriveAPI
        //I noticed GUI is frozen while using GoogleDriveAPI waitForFinish
        //waitForFinish();
        return dbList;
}

void GDriveGetDbHelper::DbListLoaded(const FileInfoList dbList) {
 qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
 this->dbList=dbList;
 this->loaded=true;
}
