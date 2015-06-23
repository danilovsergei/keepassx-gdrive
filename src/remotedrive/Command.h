#ifndef Command_H
#define Command_H
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QtConcurrentRun>
#include <remotedrive/Errors.h>

namespace KeePassxDriveSync {
class Command : public QObject
{
    Q_OBJECT
public:
    Command();
    // returns error description if error happen. empty otherwise
    const QString getErrorString();
    // returns error code from Errors.h , or Errors:NoError if command was successfull
    const int getErrorCode();
    // result of command execution if it requires something other than void. result variable must be initialized in derived class in that case
    const QVariantList getResult();

    /**
     * @brief execute used to execute execute() functions in derived classes with concrete implementation.
     * ex: DownloadCommand execute()
     * For async execution results could be captured via finished() slot and getErrorCode() function
     * @param options map of options which could be constructed by OptionsBuilder.
     */

    void executeAsync(const QVariantMap options);
public Q_SLOTS:
    virtual void execute(const QVariantMap options) = 0;


protected:
    // generates error messages and emits finished()
    void emitError(const int errorCode, const QString &errorString);
    void emitSuccess();
    /**
     * @brief parseOption used to extract option by it's value.
     * @param options map of options to extract values from
     * @param optionName name of the option to look in the options map
     * @param optionalValue what value should be used if option does not exists in the map
     * @return  provided optional value if option does not exists in the map or option value from the map otherwise
     */
    template<typename T>
    T parseOption(const QVariantMap &options, const QString &optionName, const T &optionalValue)
    {
        if (!options.contains(optionName))
            return optionalValue;
        else
            return options.value(optionName).value<T>();
    }

    /**
     * Extracts optionName value from the options map
     * Will fail if optionName does not exists in the map
     * Must be used for required options only
     */
    template<typename T>
    T parseOption(const QVariantMap &options, const QString &optionName)
    {
        // fail since option is required
        Q_ASSERT(options.contains(optionName));
        QVariant value(options.value(optionName));
        Q_ASSERT(!value.isNull());
        return value.value<T>();
    }

    QVariantList result;
private:
    QString errorString;
    // set errorCode to error by default to prevent any mistaken expectations
    int errorCode = Errors::InternalError::EMPTY_ERROR_CODE;

Q_SIGNALS:
    void finished();
    void emitExecuteAsync(const QVariantMap options);
};
}
#endif // Command_H
