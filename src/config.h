#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QException>

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(config)
Q_DECLARE_LOGGING_CATEGORY(configVerbose)

class Config : public QObject
{
    Q_OBJECT
public:
    class ConfigVersionTooHighError : public QException
    {
    public:
        const QString fileName;
        explicit ConfigVersionTooHighError(const QString &fileName) : fileName{fileName} {}

        void raise() const override { throw *this; }
        ConfigVersionTooHighError *clone() const override { return new ConfigVersionTooHighError(*this); }
    };

    /**
     * @brief Determines the config location and loads the config.
     * @param fileName The name of the config file. Will be appended to the config location.
     * @param parent The parent.
     * @param clearConfig If true will clear the whole config file and then initialize it.
     * @throws ConfigVersionTooHighError When the config version is higher as the currently supported config version.
     */
    explicit Config(const QString &fileName, QObject *parent = nullptr, bool clearConfig = false);

    enum class Setting
    {
        ConfigVersion_Int = 0,
        UpdateChecker_AutoUpdateCheckEnabled_Bool = 1,
        UpdateChecker_LastAutoUpdateCheck_QDateTime = 2
    };
    Q_ENUM(Setting)

    /**
     * @brief Get the settings key for the setting.
     * @param setting Which setting.
     * @return The settings key.
     */
    inline QString getKey(Config::Setting setting) const {
        if (!settingsKeyMap.contains(setting))
            throw std::logic_error(std::string("[Development Error] settingsKeyMap not updated for setting ").append(std::to_string((int)setting)));

        return settingsKeyMap[setting];
    };
    /**
     * @brief Get the default value for the setting.
     * @param setting Which setting.
     * @return The default value.
     */
    inline QVariant getDefaultValue(Config::Setting setting) const {
        if (!settingsDefaultValues.contains(setting))
            throw std::logic_error(std::string("[Development Error] settingsDefaultValues not updated for setting ").append(std::to_string((int)setting)));

        return settingsDefaultValues[setting];
    };
    /**
     * @brief Returns a int value for the given setting. If no value is saved the default will be written and then returned.
     * @param setting For which setting.
     * @return The int value in the config file or the default if faulty or missing.
     */
    int getInt(Config::Setting setting) const;
    /**
     * @brief Returns a bool value for the given setting. If no value is saved the default will be written and then returned.
     * @param setting For which setting.
     * @return The bool value in the config file or the default if faulty or missing.
     */
    bool getBool(Config::Setting setting) const;
    /**
     * @brief Returns a QDateTime value for the given setting. If no value is saved the default will be written and then returned.
     * @param setting For which setting.
     * @return The QDateTime value in the config file or the default if faulty or missing.
     */
    QDateTime getQDateTime(Config::Setting setting) const;
    /**
     * @brief Sets the value the setting to value.
     * @param setting
     * @param value
     */
    inline void setValue(Config::Setting setting, const QVariant &value) { this->settings->setValue(getKey(setting), value); }


private:
    /**
     * @brief Maps the Setting enum to the key string.
     */
    static const QMap<Config::Setting, QString> settingsKeyMap;
    /**
     * @brief Maps the Setting enum to the default value.
     */
    static const QMap<Config::Setting, QVariant> settingsDefaultValues;

    /**
     * @brief The minimum config version. This will never change.
     */
    static const int minConfigVersion = 1;

    /**
     * @brief The actual settings object.
     */
    QSettings *settings;

    /**
     * @brief Creates all directories in the given path if they do not already exist.
     * @param dir The directory.
     */
    inline void createPathIfNeeded(const QDir &dir) const
    {
        if (!dir.exists() && !dir.mkpath(dir.absolutePath()))
            qCFatal(config) << "Could not create config directory:" << dir.absolutePath();
    }

    /**
     * @brief Writes the config with all keys in settingsKeyMap and its corresponding settingsDefaultValues. Skips keys which are already set.
     */
    void writeDefaultConfig();

    /**
     * @brief Make a backup of the config.
     */
    void backupConfig() const;

    /**
     * @brief Backup and upgrade the config to the current version.
     * @param oldVersion The config version we are upgrading from.
     * @param newVersion The config version we upgrade to (only used for logging).
     */
    void upgradeConfig(int oldVersion, int newVersion);
};

#endif // CONFIG_H
