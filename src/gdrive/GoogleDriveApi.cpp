#include "GoogleDriveApi.h"
#include "QtCore/QFuture"
#include "QtCore/QtConcurrentRun"
#include "QtCore/QFile"
#include <QtCore/QBuffer>
#include <QtCore/QVariant>
#include <QtGui/QErrorMessage>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
const QString LAST_MODIFIED="LAST_MODIFIED";
GoogleDriveApi* GoogleDriveApi::m_instance(Q_NULLPTR);

GoogleDriveApi::GoogleDriveApi(QObject *parent) :
    QObject(parent)
{

}

void GoogleDriveApi::getDatabases() {
    QFuture<void> future=QtConcurrent::run(this,&GoogleDriveApi::getDatabasesTh);
}

void GoogleDriveApi::getDatabasesTh() {
    CommandFileList cmd(getSession());
    cmd.setFields("items(fileSize,id,title,modifiedDate,description,downloadUrl)");
    cmd.execForFolder("0B8tltL21wts3NGpOZzdNZDcwTXc");
    if (!cmd.waitForFinish(false)) {
        //TODO move error handling to the main thread
            //QErrorMessage error;
            //error.showMessage("Failed to get databases list"+cmd.errorString());
            //error.exec();
            return;
    }


    FileInfoList db_files=cmd.files();
    Q_EMIT DbListLoaded(db_files);
}

void GoogleDriveApi::downloadDatabase(const FileInfo& fi) {
 QFuture<void> future=QtConcurrent::run(this,&GoogleDriveApi::downloadDatabaseTh,fi);
}


void GoogleDriveApi::downloadDatabaseTh(const FileInfo& fi) {
qDebug() << QString("Url::"+fi.downloadUrl().toString());
    qDebug() <<QString("id="+fi.id());

CommandDownloadFile cmd(getSession());
connect(&cmd, SIGNAL(progress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
QString localDb=QDir::toNativeSeparators(config()->getConfigDir()+"/"+fi.title());
qDebug() << QString("Writing to file::"+localDb);
QFile f(localDb);
f.open(QFile::WriteOnly);

qDebug() << QString("Url::"+fi.downloadUrl().path());
Q_FOREACH(const QString& key,fi.exportList().keys()) {
    qDebug()<<fi.exportList().value(key).toString();
}

cmd.exec(fi.downloadUrl(), &f);
if (!cmd.waitForFinish(false))
    return;
Q_EMIT downloadDatabaseFinished(localDb);
}


void GoogleDriveApi::uploadDatabase(const QString& filePath, const QDateTime &lastModified) {
 QFuture<void> future=QtConcurrent::run(this,&GoogleDriveApi::uploadDatabaseTh,filePath,lastModified);
}

void GoogleDriveApi::setLastModificationDate(const FileInfo& fi,const QDateTime &lastModified) {
 QFuture<void> future=QtConcurrent::run(this,&GoogleDriveApi::setLastModificationDateTh,fi,lastModified);
}

QDateTime GoogleDriveApi::getLastModificationDate(const FileInfo& fi) {
 QFuture<QDateTime> lastModificationDate=QtConcurrent::run(this,&GoogleDriveApi::getLastModificationDateTh,fi);
 return lastModificationDate;
}


void GoogleDriveApi::uploadDatabaseTh(const QString& filePath,const QDateTime& lastModified) {

qDebug() << "Start uploading the database "+filePath;
QString dbName=QDir::toNativeSeparators(QFileInfo(filePath).fileName());

//CommandUploadFile cmd(getSession());
CommandUploadFile* cmd = new CommandUploadFile(getSession());
cmd->setAutoDelete(true);
connect(cmd, SIGNAL(finished()), this,SLOT(checkUploadDatabaseStatus()));
connect(cmd, SIGNAL(progress(qint64,qint64)), this,SLOT(uploadDatabaseProgress(qint64,qint64)));
//we will wait until database will be uploaded using local eventloop
QEventLoop qE;
QTimer tT;
tT.setSingleShot(true);
connect(this,SIGNAL(uploadDatabaseFinished()),&qE,SLOT(quit()));
connect(&tT,SIGNAL(timeout()),&qE,SLOT(quit()));


FileInfo fi;
fi.setTitle(dbName);
fi.setParents(QStringList("0B8tltL21wts3NGpOZzdNZDcwTXc"));
qDebug() << "Set file name:"+dbName;



QFile file(filePath);
cmd->exec(fi, &file);

if (!cmd->waitForFinish(false)) {
return;
}


qDebug() << "Start timer!!";

tT.start(10000);
//execute local loop and wait until database will be uploaded
qDebug() << "Before eventlopp";
if (cmd==NULL) {
    qDebug() << "We are done!!!!";
}
qE.exec();
qDebug() << "Qeventlopp stopped";
if (tT.isActive()) {
//database upload finished earlier than timer timeout.Just stop the timer
    tT.stop();
    qDebug() << "Timer stopped";

}
else {
    qDebug() <<"Fail to upload database to the cloud in 10 seconds";
    //TODO raise exception here
}

FileInfo uploadedDb=cmd->resultFileInfo();
delete cmd;
qDebug() << "Uploading function finished";
this->setLastModificationDate(uploadedDb,lastModified);
}



void GoogleDriveApi::setLastModificationDateTh(const FileInfo& fi,const QDateTime &lastModified) {
CommandSetFileProperties* cmd = new CommandSetFileProperties(getSession());
QMap<QString,QString> properties;
properties[LAST_MODIFIED]=lastModified.toString();
cmd->exec(fi,properties);

if (!cmd->waitForFinish(true))
    return;
}

QDateTime GoogleDriveApi::getLastModificationDateTh(const FileInfo& fi){
    CommandGetFileProperties* cmd = new CommandGetFileProperties(getSession());
    cmd->exec(fi);
    if (!cmd->waitForFinish(true))
        return QDateTime();
    QMap<QString,QString> props=cmd->getProperties();
    QDateTime time;
    if (props.contains(LAST_MODIFIED)) {
        time=QDateTime::fromString(props[LAST_MODIFIED]);
    }
    //return null in case if LAST_MODIFIED property does not exist
    return time;
}


void GoogleDriveApi::checkUploadDatabaseStatus()
{
    CommandUploadFile* cmd = qobject_cast<CommandUploadFile*>(sender());
    if (cmd->error() == Command::NoError) {
        qDebug() << "Uploading finished: "+cmd->resultFileInfo().id();
    }

    else {
         qDebug() << cmd->errorString();
    }

    //delete cmd;
    //initialize removed pointer to null for further check that object does not exist anymore
    //cmd=NULL;
    Q_EMIT uploadDatabaseFinished();

}

void GoogleDriveApi::uploadDatabaseProgress(qint64 v, qint64 total)
{
    qDebug() << "Upload progress: "+QString::number(v)+" from "+ QString::number(total);
}


void GoogleDriveApi::downloadProgress(qint64 v, qint64 total)
{
    qDebug() <<QString("Download progress "+QString::number(v)+" from "+QString::number(total));
}


GoogleDriveApi* GoogleDriveApi::instance(){
    if (!m_instance) {
        m_instance = new GoogleDriveApi();
    }
    return m_instance;
}



GoogleDriveApi::~GoogleDriveApi(){

}




