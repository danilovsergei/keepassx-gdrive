  #include "databaseopenwidgetcloud.h"
#include "ui_databaseopenwidgetcloud.h"

#include <QtCore/QSettings>
#include "../qtdrive/lib/session.h"
#include "QtNetwork/QNetworkAccessManager"
#include "../qtdrive/lib/command_file_list.h"
#include "../qtdrive/test/options.h"

#include "../gdrive/Singleton.h"
#include "../gdrive/GoogleDriveSession.h"

#include <iostream>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include "../gdrive/GoogleDriveApi.h"

#include <QtCore/QMetaType>



using namespace GoogleDrive;
Q_DECLARE_METATYPE(GoogleDrive::FileInfo)
Q_DECLARE_METATYPE(GoogleDrive::FileInfoList)


DatabaseOpenWidgetCloud::DatabaseOpenWidgetCloud(QWidget *parent) :
    DialogyWidget(parent),
    ui(new Ui::DatabaseOpenWidgetCloud)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(rejected()), SIGNAL(dbRejected()));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(downloadDb()));

}

/**
 * @brief DatabaseOpenWidgetCloud::cloudDbLoad - shows the latest version of each available database with unique name
 * @param db_files - list all databases available including all revisions
 */
void DatabaseOpenWidgetCloud::cloudDbLoad(GoogleDrive::FileInfoList db_files)
{
     QMap<QString,FileInfo> files;
     Q_FOREACH (const FileInfo& fi, db_files) {
         if (!files.contains(fi.title()) || (files.contains(fi.title()) && fi.modifiedDate().toMSecsSinceEpoch()>=files[fi.title()].modifiedDate().toMSecsSinceEpoch())) {
            files[fi.title()]=fi;
         }
       }
        Q_FOREACH (const QString& db_name,files.keys()) {
            QDateTime last_date=googleDriveApi()->getLastModificationDate(files[db_name]);
            QString lastDateStr="Empty";
            if (!last_date.isNull()&& last_date.isValid()) {
               lastDateStr=last_date.toString();
            }
            ui->comboCloudDbLatest->addItem(lastDateStr+"----"+db_name,QVariant::fromValue(files[db_name]));
        }
}

void DatabaseOpenWidgetCloud::reject()
{
    Q_EMIT editFinished(false);
}

void DatabaseOpenWidgetCloud::startWorkInAThread()
{

qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
connect(googleDriveApi(),SIGNAL(DbListLoaded(GoogleDrive::FileInfoList)),this,SLOT(cloudDbLoad(GoogleDrive::FileInfoList)));
googleDriveApi()->getDatabasesPar();
}

void DatabaseOpenWidgetCloud::selectCloud(int cloud) {
//if (cloud==0) {
Q_EMIT cloudSelected();
//}
}

void DatabaseOpenWidgetCloud::loadSupportedCloudEngines(){
//TODO load list of supported engines from config file
ui->comboCloudEngine->addItem("Google Drive");
ui->comboCloudEngine->setCurrentIndex(0);
startWorkInAThread();
}

DatabaseOpenWidgetCloud::~DatabaseOpenWidgetCloud()
{
}


void DatabaseOpenWidgetCloud::downloadDb() {
    FileInfo fi=ui->comboCloudDbLatest->itemData(ui->comboCloudDbLatest->currentIndex(),Qt::UserRole).value<FileInfo>();
    connect(googleDriveApi(),SIGNAL(downloadDatabaseFinished(const QString&)),this,SIGNAL(dbSelected(const QString&)));
    googleDriveApi()->downloadDatabase(fi);
    Q_EMIT editFinished(true);
}






