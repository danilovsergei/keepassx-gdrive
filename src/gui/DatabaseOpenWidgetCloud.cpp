#include "DatabaseOpenWidgetCloud.h"
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
bool firstTimeDownload = false;
QString fullDbName;

DatabaseOpenWidgetCloud::DatabaseOpenWidgetCloud(QWidget *parent) :
  DialogyWidget(parent),
  ui(new Ui::DatabaseOpenWidgetCloud)
{
  ui->setupUi(this);
  connect(ui->buttonBox, SIGNAL(rejected()), SIGNAL(dbRejected()));
  connect(ui->buttonBox, SIGNAL(accepted()), SLOT(downloadDb()));
  googleDrive    = GoogleDriveApi::newInstance();
  downloadHelper = GDriveDbDownloadHelper::newInstance();
}

/**
 * @brief DatabaseOpenWidgetCloud::cloudDbLoad - shows the latest version of
 *each available database with unique name
 * @param db_files - list all databases available including all revisions
 */
void DatabaseOpenWidgetCloud::cloudDbLoad(GoogleDrive::FileInfoList db_files)
{
  QMap<QString, FileInfo> files;
  Q_FOREACH(const FileInfo &fi, db_files) {
    if (!files.contains(fi.title()) ||
        (files.contains(fi.title()) &&
         (fi.modifiedDate().toMSecsSinceEpoch() >=
      files[fi.title()].modifiedDate().toMSecsSinceEpoch()))) {
      files[fi.title()] = fi;
    }
  }
  Q_FOREACH(const QString &db_name, files.keys()) {
    ui->comboCloudDbLatest->addItem(files.value(
                                      db_name).modifiedDate().toString() + "----" + db_name,
                                    QVariant::fromValue(files.value(db_name)));
  }
}

void DatabaseOpenWidgetCloud::reject()
{
  Q_EMIT editFinished(false);

  // TODO pass this remove to DatabaseOpenWidget
  // remove locally downloaded database if user cancel cloud db opening.
  // Do it only if database was downloaded first time
  // if
  // (fullDbName.endsWith("kdbx")&&GoogleDriveTools::hasLocalDatabase(fullDbName))
  // QFile(fullDbName).remove();
}

void DatabaseOpenWidgetCloud::startWorkInAThread()
{
  qRegisterMetaType<GoogleDrive::FileInfoList>("GoogleDrive::FileInfoList");
  connect(googleDrive.data(), SIGNAL(dbListLoaded(
                                       GoogleDrive::FileInfoList)), this,
          SLOT(cloudDbLoad(
                 GoogleDrive
                 ::FileInfoList)));
  googleDrive->getDatabasesPar();
}

void DatabaseOpenWidgetCloud::selectCloud(int cloud) {
  // if (cloud==0) {
  Q_EMIT cloudSelected();

  // }
}

void DatabaseOpenWidgetCloud::loadSupportedCloudEngines() {
  // TODO load list of supported engines from config file
  ui->comboCloudEngine->addItem("Google Drive");
  ui->comboCloudEngine->setCurrentIndex(0);
  startWorkInAThread();
}

DatabaseOpenWidgetCloud::~DatabaseOpenWidgetCloud()
{}

/**
 * @brief DatabaseOpenWidgetCloud::downloadDb downloads selected database from
 *cloud and emits dbSelected signal
 */
void DatabaseOpenWidgetCloud::downloadDb() {
  FileInfo fi = ui->comboCloudDbLatest->itemData(
    ui->comboCloudDbLatest->currentIndex(),
    Qt::UserRole).value<FileInfo>();

  // Avoid unnecessary downloading of cloud db if it exists locally.Instead call
  // sync further
  if (GoogleDriveTools::hasLocalDatabase(fi.title())) {
    qDebug() << QString(
      "Database %1 already exists locally.Open it and sync with cloud revision of ").arg(
      fi.title());
    Q_EMIT dbSelected(GoogleDriveTools::getLocalDatabasePath(fi.title()));
  } else {
    // download cloud db to the database folder
    qDebug() << QString("Database %1 downloaded locally first time.").arg(
      fi.title());
    connect(downloadHelper.data(), SIGNAL(downloadDatabaseDone(
                                            const QString &)),         this,
            SIGNAL(dbSelected(const QString &)));
    connect(downloadHelper.data(),
            SIGNAL(downloadDatabaseError(int,const QString&)), this,
            SIGNAL(downloadDbError(int, const QString&)));
    downloadHelper->downloadDatabaseParallel(fi);
  }
  Q_EMIT editFinished(true);
}

void DatabaseOpenWidgetCloud::downloadDbError(int            ErrorType,
                                              const QString& description) {
  QMessageBox::warning(this, tr("Error"),
                       tr("Unable to download cloud database.\n%1:%2")
                       .arg(QString::number(ErrorType), description));
}
