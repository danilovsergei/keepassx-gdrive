#ifndef Command_H
#define Command_H
#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <gdrive/Errors.h>
#include <QtCore/QDebug>
namespace KeePassxDriveSync {
class Command : public QObject
{
Q_OBJECT
public:
    Command();
    virtual void execute(const QVariantMap& options) = 0;
    QString getErrorString();
    int getErrorCode();
    QVariantList getResult();
protected:
    void emitError(const int errorCode, const QString& errorString);
    void emitSuccess();
    QVariantList result;
private:
    QString errorString;
    int errorCode = Errors::NO_ERROR;



Q_SIGNALS:
    void finished();
};
}
#endif // Command_H
