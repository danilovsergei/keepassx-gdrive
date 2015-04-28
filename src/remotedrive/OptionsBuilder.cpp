#include "OptionsBuilder.h"

OptionsBuilder::OptionsBuilder()
{
}

const QVariantMap OptionsBuilder::build() {
  // return by value since QVariantMap is relatively cheap object
  return instance;
}

//template <typename T>
///**
// * @brief addOption adds option to builder using fluent technique.
// * ex: builder->addOption()->addOption()
// * @param key  String option name. Use GDriveConstants to get list of available names
// * @param value could be any type. Custom types must be registered with Q_DECLARE_METATYPE()
// * @return instance of option builder to support fluency.
// */
//OptionsBuilder& OptionsBuilder::addOption(const QString& key, const T& value) {

//}
