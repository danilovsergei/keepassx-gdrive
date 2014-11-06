#ifndef DATABASEOPENWIDGETCLOUD_H
#define DATABASEOPENWIDGETCLOUD_H

#include "gui/DatabaseWidget.h"
#include "gui/DialogyWidget.h"
#include "QtCore/QList"
#include "gdrive/GoogleDriveApi.h"
#include "gdrive/GoogleDriveTools.h"
#include "gdrive/helpers/GDriveDbDownloadHelper.h"

#include "../qtdrive/lib/file_info.h"
namespace Ui {
class DatabaseOpenWidgetCloud;
}

class DatabaseOpenWidgetCloud : public DialogyWidget
{
    Q_OBJECT

Q_SIGNALS:
    void editFinished(bool accepted);
    void widgetLoaded();
    void cloudSelected();
    void dbSelected(const QString&);
    void dbRejected();

protected Q_SLOTS:
    void selectCloud(int cloud);
    void cloudDbLoad(GoogleDrive::FileInfoList db_files);
    void downloadDb();
    void reject();
    void downloadDbError(int ErrorType,const QString& description);



public Q_SLOTS:
    void loadSupportedCloudEngines();
    void startWorkInAThread();


public:
    explicit DatabaseOpenWidgetCloud(QWidget *parent = 0);
    ~DatabaseOpenWidgetCloud();

private:
    Ui::DatabaseOpenWidgetCloud *ui;
    QSharedPointer<GoogleDriveApi> googleDrive;
    QSharedPointer<GDriveDbDownloadHelper> downloadHelper;
};

#endif // DATABASEOPENWIDGETCLOUD_H
