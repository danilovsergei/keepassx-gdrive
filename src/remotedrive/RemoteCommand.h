#ifndef RemoteCommand_H
#define RemoteCommand_H
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QtConcurrentRun>
#include <remotedrive/Errors.h>
#include <QtCore/QEventLoop>
#include <core/Global.h>
#include <QtCore/QUuid>
#include <QtCore/QSharedPointer>

namespace KeePassxDriveSync {
class RemoteCommand;
typedef QSharedPointer<RemoteCommand> Command;
class RemoteCommand : public QObject
{
  Q_OBJECT
public:
  enum Status {
    NotStarted = 0,
    InProgress = 1,
  };
  RemoteCommand();
  ~RemoteCommand();
  // returns error description if error happen. empty otherwise
  const QString getErrorString();
  // returns error code from Errors.h , or Errors:NoError if command was successfull
  const int getErrorCode();

  /**
   * @brief checkForError checks whether passed command was executed with error and call emitError()
   * in current command instance. Used to to check for errors when one command executed under another
   *
   * @param cmd command to check for error
   * @return True if command emits error and false othewise
   */
  const bool checkForError(Command cmd);

  /**
   * @brief getResult result of command execution if it exists
   * result variable must be initialized in derived class in that case
   * @return QVariantList with possibly more than one variable
   */
  const QVariantList getResult();
  // allows to wait until async execution will finish
  void waitForFinish();
  /**
   * @brief setResult sets result of command execution
   * @param result QVariantList with possibly more than one variable
   */
  void setResult(QVariantList result);

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

private:
  QVariantList result;
  QUuid ID = QUuid::createUuid();
  QEventLoop *loop = Q_NULLPTR;
  QString errorString;
  // set errorCode to error by default to prevent any mistaken expectations
  int errorCode = Errors::InternalError::EMPTY_ERROR_CODE;
  Status status = Status::NotStarted;

private Q_SLOTS:
  void finishEventLoop();

Q_SIGNALS:
  void finished();
  void emitExecuteAsync(const QVariantMap options);
};
class ResultBuilder
{
public:
  /**
   * @brief build provides fluent interface to build QVariantList result
   *   which returned by command execution
   */
  ResultBuilder()
  {
  }

  const QVariantList build()
  {
    // return by value since QVariantMap is relatively cheap object
    return instance;
  }

  template<typename T>
  /**
   * @brief addOption adds option to builder using fluent technique.
   * ex: builder->addOption()->addOption()
   * @param key  String option name. Use GDriveConstants to get list of available names
   * @param value could be any type. Custom types must be registered with Q_DECLARE_METATYPE()
   * @return instance of option builder to support fluency.
   */
  ResultBuilder &addValue(const T &value)
  {
    instance.append(QVariant::fromValue(value));
    return *this;
  }

private:
  QVariantList instance;
};
}
#endif // RemoteCommand_H
