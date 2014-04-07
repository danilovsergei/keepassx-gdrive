#ifndef OPTIONS_DIALOG_H
#define OPTIONS_DIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

    bool event(QEvent*);

public Q_SLOTS:
    void accept();

private:
    void updateDialog();

    Ui::OptionsDialog *ui;
};

#endif // OPTIONS_DIALOG_H
