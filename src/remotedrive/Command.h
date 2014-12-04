#ifndef Command_H
#define Command_H
#include <QtCore/QObject>
#include <gdrive/Errors.h>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QtConcurrentRun>
namespace KeePassxDriveSync {
class Command : public QObject
{
Q_OBJECT
public:
    Command();
    /**
     * @brief execute used to execute execute() functions in derived classes with concrete implementation.
     * ex: DownloadCommand execute()
     * For async execution results could be captured via finished() slot and getErrorCode() function
     * @param options map of options which could be constructed by OptionsBuilder.
     */
    virtual void execute(const QVariantMap& options) = 0;
    void executeAsync(const QVariantMap& options);
    // returns error description if error happen. empty otherwise
    const QString getErrorString();
    // returns error code from Errors.h , or Errors:NoError if command was successfull
    const int getErrorCode();
    // result of command execution if it requires something other than void. result variable must be initialized in derived class in that case
    const QVariantList getResult();
protected:
    // generates error messages and emits finished()
    void emitError(const int errorCode, const QString& errorString);
    void emitSuccess();
    /**
     * @brief parseOption used to extract option by it's value.
     * @param options map of options to extract values from
     * @param optionName name of the option to look in the options map
     * @param optionalValue what value should be used if option does not exists in the map
     * @return  provided optional value if option does not exists in the map or option value from the map otherwise
     */
    template <typename T>
    T parseOption(const QVariantMap& options, const QString& optionName, const T& optionalValue) {
        if (!options.contains(optionName)) {
          return optionalValue;
        }
        else {
          return options.value(optionName).value<T>();
        }
    }
    /**
     * Extracts optionName value from the options map
     * Will fail if optionName does not exists in the map
     * Must be used for required options only
     */
    template <typename T>
    T parseOption(const QVariantMap& options, const QString& optionName) {
        // fail since option is required
        Q_ASSERT(options.contains(optionName));
        QVariant value(options.value(optionName));
        Q_ASSERT(!value.isNull());
        return value.value<T>();
    }
    QVariantList result;
private:
    QString errorString;
    int errorCode = Errors::NO_ERROR;



Q_SIGNALS:
    void finished();
};
/**
 * @brief The OptionsBuilder class used to create options for Command::execute()
 */
class OptionsBuilder {

public:
    /**
     * @brief newInstance creates new instance of OptionsBuilder
     */
    static OptionsBuilder* newInstance();
    template <typename T>
    /**
     * @brief addOption adds option to builder using fluent technique.
     * ex: builder->addOption()->addOption()
     * @param key  String option name. Use GDriveConstants to get list of available names
     * @param value could be any type. Custom types must be registered with Q_DECLARE_METATYPE()
     * @return instance of option builder to support fluency.
     */
    OptionsBuilder* addOption(const QString& key, const T& value) {
        instance.insert(key, QVariant::fromValue(value));
        return this;
    }
    /**
     * @brief build provides QVariantMap options which could be used in Command::execute(options)
     */
    const QVariantMap build();
private:
    QVariantMap instance;
};
}
#endif // Command_H
