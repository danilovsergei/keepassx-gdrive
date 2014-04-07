#include "SettingsWidgetCloud.h"
#include "ui_SettingsWidgetCloud.h"
#include "QtGui/QFileDialog"

SettingsWidgetCloud::SettingsWidgetCloud(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidgetCloud)
{
    ui->setupUi(this);
    ui->dbDirectory->setText("Test");

}






SettingsWidgetCloud::~SettingsWidgetCloud()
{
    delete ui;
}
