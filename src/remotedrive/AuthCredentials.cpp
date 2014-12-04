#include "AuthCredentials.h"

AuthCredentials::AuthCredentials()
{}

QVariantMap * AuthCredentials::getCredentials() {
  return creds;
}

const QString AuthCredentials::getOption(const QString& option) {
  if (creds->contains(option))
  {
    return creds->value(option)
  }
  else {
    qFatal(QString("Failed to get credential %1 from the credentials cache").arg(
             option));
  }
}
