#include "DatabaseOpenWidgetCloud.h"
#include "ui_DatabaseOpenWidgetCloud.h"

#include <QtCore/QSettings>
#include <iostream>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>

#include <QtCore/QMetaType>


using namespace GoogleDrive;
Q_DECLARE_METATYPE(RemoteFile)
Q_DECLARE_METATYPE(RemoteFileList)
bool firstTimeDownload = false;
QString fullDbName;

DatabaseOpenWidgetCloud::DatabaseOpenWidgetCloud(QWidget *parent) :
  DialogyWidget(parent),
  ui(new Ui::DatabaseOpenWidgetCloud)
{
  ui->setupUi(this);
  connect(ui->buttonBox, SIGNAL(rejected()), SIGNAL(dbRejected()));
  connect(ui->buttonBox, SIGNAL(accepted()), SLOT(downloadDb()));
  AuthCredentials* creds = new GoogleDriveCredentials(this);
  CommandsFactory* commandsFactory = new CommandsFactoryImpl(this, creds);
  // remote drive will be used to call all remote drive functions like sync , upload, download
  remoteDrive = new RemoteDriveApi(this, commandsFactory);
}

/**
 * @brief DatabaseOpenWidgetCloud::cloudDbLoad - shows the latest version of
 *each available database with unique name
 * @param db_files - list all databases available including all revisions
 */
void DatabaseOpenWidgetCloud::cloudDbLoad()
{
  Q_ASSERT(listCommand->getErrorCode()==static_cast<int>(Errors::NO_ERROR));
  qRegisterMetaType<RemoteFileList>("RemoteFileList");
  const RemoteFileList db_files = listCommand->getResult().at(0).value<RemoteFileList>();
  QMap<QString, RemoteFile> files;
  Q_FOREACH(const RemoteFile& fi, db_files) {
    if (!files.contains(fi.getTitle()) ||
        (files.contains(fi.getTitle()) &&
         (fi.getModifiedDate().toMSecsSinceEpoch() >=
      files[fi.getTitle()].getModifiedDate().toMSecsSinceEpoch()))) {
      files[fi.getTitle()] = fi;
    }
  }
  Q_FOREACH(const QString db_name, files.keys()) {
    ui->comboCloudDbLatest->addItem(files.value(
                                      db_name).getModifiedDate().toString() + "----" + db_name,
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


void DatabaseOpenWidgetCloud::selectCloud(int cloud) {
  // if (cloud==0) {
  Q_EMIT cloudSelected();

  // }
}

void DatabaseOpenWidgetCloud::loadSupportedCloudEngines() {
  // TODO load list of supported engines from config file
  ui->comboCloudEngine->addItem("Google Drive");
  ui->comboCloudEngine->setCurrentIndex(0);
  listCommand = remoteDrive->list();
  connect(listCommand.data(), SIGNAL(finished()),this, SLOT(cloudDbLoad()));
  listCommand->executeAsync(OptionsBuilder().build());
}

DatabaseOpenWidgetCloud::~DatabaseOpenWidgetCloud()
{}

/**
 * @brief DatabaseOpenWidgetCloud::downloadDb downloads selected database from
 *cloud and emits dbSelected signal
 */
void DatabaseOpenWidgetCloud::downloadDb() {
  RemoteFile fi = ui->comboCloudDbLatest->itemData(
    ui->comboCloudDbLatest->currentIndex(),
    Qt::UserRole).value<RemoteFile>();

  // Avoid unnecessary downloading of cloud db if it exists locally.Instead call
  // sync further
  if (GoogleDriveTools::hasLocalDatabase(fi.getTitle())) {
    qDebug() << QString(
      "Database %1 already exists locally.Open it and sync with cloud revision of ").arg(
      fi.getTitle());
    Q_EMIT dbSelected(GoogleDriveTools::getLocalDatabasePath(fi.getTitle()));
  } else {
    // download cloud db to the database folder
    qDebug() << QString("Database %1 downloaded locally first time.").arg(
      fi.getTitle());
    downloadCommand = remoteDrive->download();
    connect(downloadCommand.data(), SIGNAL(finished()),         this,
          SLOT(downloadDbFinished()) );
    downloadCommand->executeAsync(OptionsBuilder().addOption(OPTION_FI, fi).build());
  }
  Q_EMIT editFinished(true);
}

void DatabaseOpenWidgetCloud::downloadDbFinished() {
    Q_ASSERT(downloadCommand->getErrorCode()==static_cast<int>(Errors::NO_ERROR));
    QString dbPath = downloadCommand->getResult().at(0).toString();
    Q_ASSERT (!dbPath.isNull() && !dbPath.isEmpty());
    Q_EMIT dbSelected(dbPath);
}
