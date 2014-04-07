#ifndef SETTINGSWIDGETCLOUD_H
#define SETTINGSWIDGETCLOUD_H

#include <QtGui/QWidget>

namespace Ui {
class SettingsWidgetCloud;
}

class SettingsWidgetCloud : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidgetCloud(QWidget *parent = 0);
    ~SettingsWidgetCloud();


private:
    Ui::SettingsWidgetCloud *ui;


};

#endif // SETTINGSWIDGETCLOUD_H
