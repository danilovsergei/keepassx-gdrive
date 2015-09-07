#include "DatabaseOpenWidgetCloud.h"
#include "ui_DatabaseOpenWidgetCloud.h"

#include <QSettings>
#include <iostream>
#include <QMessageBox>
#include <QDebug>

#include <QMetaType>

using namespace GoogleDrive;
Q_DECLARE_METATYPE(RemoteFile)
Q_DECLARE_METATYPE(RemoteFileList)
bool firstTimeDownload = false;
QString fullDbName;

DatabaseOpenWidgetCloud::DatabaseOpenWidgetCloud(QWidget *parent) :
  DialogyWidget(parent),
  m_ui(new Ui::DatabaseOpenWidgetCloud)
{
  m_ui->setupUi(this);
  connect(m_ui->buttonBox, SIGNAL(rejected()), SIGNAL(dbRejected()));
  connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(downloadDb()));
  AuthCredentials *creds = new GoogleDriveCredentials(this);
  CommandsFactory *commandsFactory = new CommandsFactoryImpl(this, creds);
  remoteDrive = new RemoteDriveApi(this, commandsFactory);
}

/**
 * @brief DatabaseOpenWidgetCloud::cloudDbLoad - populates databases combobox with the list of
 * available databases.
 * Chooses only one database with latest modification time if there are two databases with the same name.
 * Combobox display format is modification_time: database_name
 *
 * @param db_files - list all databases available including all revisions
 */
void DatabaseOpenWidgetCloud::cloudDbLoad()
{
  Q_ASSERT(listCommand->getErrorCode() == static_cast<int>(Errors::NO_ERROR));
  qRegisterMetaType<RemoteFileList>("RemoteFileList");
  const RemoteFileList db_files = listCommand->getResult().at(0).value<RemoteFileList>();
  QMap<QString, RemoteFile> files;
  Q_FOREACH(const RemoteFile &fi, db_files) {
    if (!files.contains(fi.getTitle()))
      files[fi.getTitle()] = fi;

    if (fi.getModifiedDate() > files[fi.getTitle()].getModifiedDate())
         files[fi.getTitle()] = fi;
  }

  QList<RemoteFile> sortedFiles = files.values();
  qSort(sortedFiles.begin(), sortedFiles.end(), compareByDateTime);

  Q_FOREACH(RemoteFile fi, sortedFiles) {
    const QString formattedDate = formatDate(fi.getModifiedDate());
    m_ui->comboCloudDbLatest->addItem(formattedDate+ " : " + fi.getTitle(),
                                    QVariant::fromValue(fi));
  }
}

bool DatabaseOpenWidgetCloud::compareByDateTime(const RemoteFile &rf1,
                                                const RemoteFile &rf2)
{
  return rf1.getModifiedDate() > rf2.getModifiedDate();
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

void DatabaseOpenWidgetCloud::selectCloud(int cloud)
{
  // if (cloud==0) {
  Q_EMIT cloudSelected();

  // }
}

void DatabaseOpenWidgetCloud::loadSupportedCloudEngines()
{
  // TODO load list of supported engines from config file
  m_ui->comboCloudEngine->addItem("Google Drive");
  m_ui->comboCloudEngine->setCurrentIndex(0);
  listCommand = remoteDrive->list();
  connect(listCommand.data(), SIGNAL(finished()), this, SLOT(cloudDbLoad()));
  listCommand->executeAsync(OptionsBuilder().build());
}

DatabaseOpenWidgetCloud::~DatabaseOpenWidgetCloud()
{
}

/**
 * @brief DatabaseOpenWidgetCloud::downloadDb downloads selected database from
 *cloud and emits dbSelected signal
 */
void DatabaseOpenWidgetCloud::downloadDb()
{
  RemoteFile fi = m_ui->comboCloudDbLatest->itemData(
    m_ui->comboCloudDbLatest->currentIndex(),
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
    connect(downloadCommand.data(), SIGNAL(finished()), this,
            SLOT(downloadDbFinished()));
    downloadCommand->executeAsync(OptionsBuilder().addOption(OPTION_FI, fi).build());
  }
  Q_EMIT editFinished(true);
}

void DatabaseOpenWidgetCloud::downloadDbFinished()
{
  Q_ASSERT(downloadCommand->getErrorCode() == static_cast<int>(Errors::NO_ERROR));
  QString dbPath = downloadCommand->getResult().at(0).toString();
  Q_ASSERT(!dbPath.isNull() && !dbPath.isEmpty());
  Q_EMIT dbSelected(dbPath);
}

/**
 * @brief DatabaseOpenWidgetCloud::formatDate formats datetime to the compact user readable format
 * to fit combobox
 * @param dateTime input date time
 * @return formatted time
 */
const QString DatabaseOpenWidgetCloud::formatDate(const QDateTime &dateTime)
{
  return dateTime.toString(DATETIME_FORMAT);
}

QLabel* DatabaseOpenWidgetCloud::headlineLabel()
{
    return m_ui->headlineLabel;
}
