#include "AuthCredentials.h"

AuthCredentials::AuthCredentials()
{}

QVariantMap * AuthCredentials::getCredentials() {
  return creds;
}
