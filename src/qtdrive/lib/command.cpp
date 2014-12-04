#include "command.h"

#include <QtDebug>
#include <QNetworkReply>
#include <QEventLoop>
#include <qjson/parser.h>

#include "session.h"
#include "command_private.h"
#include <QtCore/QThread>

namespace GoogleDrive {
Command::Command(Session *session)
  : QObject(QThread::currentThread() == session->thread() ? session : 0)
    , d_ptr(new CommandPrivate)
{
  Q_D(Command);
  d->session = session;

  if (QThread::currentThread() != session->thread()) {
      qWarning() <<
        "Session object was created in different thread \n."
                 <<
        "Google api user is responsible to destroy initiated command manually "
                 << "or via deleteLater by setting setAutoDelete(true)";
  }
}

Command::~Command()
{
  delete d_ptr;
  qDebug() << "destoy command.h";
}

bool Command::autoDelete() const
{
  Q_D(const Command);
  return d->autoDelete;
}

void Command::setAutoDelete(bool v)
{
  Q_D(Command);
  d->autoDelete = v;
}

Command::Error Command::error() const
{
  Q_D(const Command);
  return d->error;
}

QString Command::errorString() const
{
  Q_D(const Command);
  return d->errorString;
}

bool Command::waitForFinish(bool processUserInputEvents)
{
  Q_D(Command);
  QEventLoop loop;
  d->loop = &loop;
  loop.exec(
    processUserInputEvents ? QEventLoop::AllEvents : QEventLoop::ExcludeUserInputEvents);
  d->loop = 0;
  return error() == NoError;
}

void Command::emitError(Error code, const QString& msg)
{
  Q_D(Command);
  d->error       = code;
  d->errorString = msg;

  qDebug() << metaObject()->className() << "error:" << msg;
  Q_EMIT error(code);
  Q_EMIT session()->error(this);
  Q_EMIT session()->finished(this);
  Q_EMIT finished();

  if (d->loop) d->loop->quit();
}
/**
 * @brief Command::tryAutoDelete tries to auto delete current object.
 * Usually called when emit request finished.
 * Could affect access to error() and errorString  functions since object already destoyed.
 */
void Command::tryAutoDelete()
{
  if (autoDelete()) deleteLater();
}

bool Command::checkJsonReplyError(const QVariantMap& map)
{
  const QString cError("error");

  if (!map.contains(cError)) return false;

  const QVariantMap& errorMap = map[cError].toMap();
  const QString    & code     = errorMap["code"].toString();
  const QString    & msg      = errorMap["message"].toString();

    emitError(UnknownError, tr("(%1) %2").arg(code).arg(msg));
  return true;
}

bool Command::parseJsonReply(QNetworkReply *reply, QVariantMap& map)
{
  bool ok         = false;
  QByteArray data = reply->readAll();
  QVariant   res  = QJson::Parser().parse(data, &ok);

  if (!ok)
  {
    emitError(UnknownError, tr("Json reply parse error"));
    return false;
  }
  map = res.toMap();
  return true;
}

bool Command::checkInvalidReply(QNetworkReply *reply)
{
  if (reply->error() == QNetworkReply::NoError) return false;

  QVariantMap map;

  if (parseJsonReply(reply, map))
  {
    if (checkJsonReplyError(map)) return true;
  }
    emitError(UnknownError, reply->errorString());
  return true;
}

Command::Command(CommandPrivate *d, Session *session)
  : QObject(QThread::currentThread() == session->thread() ? session : 0)
    , d_ptr(d)
{
  if (QThread::currentThread() != session->thread()) {
    qWarning() <<
      "Session object was created in different thread \n."
               <<
      "Google api user is responsible to destroy initiated command manually "
               << "or via deleteLater by setting setAutoDelete(true)";
  }

  d->session = session;
}

void Command::emitSuccess()
{
  Q_D(Command);
  d->error       = NoError;
  d->errorString = QString();
  Q_EMIT finished();
  Q_EMIT session()->finished(this);

  if (d->loop) d->loop->quit();
}

void Command::emitStarted()
{
  Q_EMIT session()->started(this);
}

Session * Command::session() const
{
  Q_D(const Command);
  return d->session;
}
}
