#include "options_dialog.h"

#include <QEvent>
#include <QSettings>

#include "ui_options_dialog.h"
#include "options.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

bool OptionsDialog::event(QEvent* e)
{
    if (e->type() == QEvent::Polish)
        updateDialog();
    return QDialog::event(e);
}

void OptionsDialog::accept()
{
    QSettings s;
    s.setValue(cClientId, ui->clientIdEdit->text());
    s.setValue(cClientSecret, ui->clientSecretEdit->text());
    s.setValue(cRefreshToken, ui->refreshTokenEdit->text());
    QDialog::accept();
}

void OptionsDialog::updateDialog()
{
    QSettings s;
    ui->clientIdEdit->setText(s.value(cClientId).toString());
    ui->clientSecretEdit->setText(s.value(cClientSecret).toString());
    ui->refreshTokenEdit->setText(s.value(cRefreshToken).toString());
}
