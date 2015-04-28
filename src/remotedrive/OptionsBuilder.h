#ifndef OPTIONSBUILDER_H
#define OPTIONSBUILDER_H
#include <QtCore/QVariantMap>
/**
 * @brief The OptionsBuilder class used to create options for Command::execute()
 */
class OptionsBuilder {

public:
    /**
     * @brief build provides QVariantMap options which could be used in Command::execute(options)
     */
    OptionsBuilder();
    const QVariantMap build();
    template <typename T>
    /**
     * @brief addOption adds option to builder using fluent technique.
     * ex: builder->addOption()->addOption()
     * @param key  String option name. Use GDriveConstants to get list of available names
     * @param value could be any type. Custom types must be registered with Q_DECLARE_METATYPE()
     * @return instance of option builder to support fluency.
     */
    OptionsBuilder& addOption(const QString& key, const T& value) {
        instance.insert(key, QVariant::fromValue(value));
        return *this;
    };
private:
    QVariantMap instance;
};

#endif // OPTIONSBUILDER_H
