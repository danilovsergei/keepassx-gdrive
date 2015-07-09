#include "OptionsBuilder.h"

OptionsBuilder::OptionsBuilder()
{
}

const QVariantMap OptionsBuilder::build() {
  // return by value since QVariantMap is relatively cheap object
  return instance;
}
