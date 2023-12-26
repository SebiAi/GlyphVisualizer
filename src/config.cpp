#include "config.h"

// Logging
Q_LOGGING_CATEGORY(config, "Config")
Q_LOGGING_CATEGORY(configVerbose, "Config.Verbose")

const QMap<Config::Setting, QString> Config::settingsKeyMap = {
    {Config::Setting::ConfigVersion_Int, "ConfigVersion"},
    {Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool, "UpdateChecker/AutoUpdateCheckEnabled"},
    {Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, "UpdateChecker/LastAutoUpdateCheck"}
};

const QMap<Config::Setting, QVariant> Config::settingsDefaultValues = {
    {Config::Setting::ConfigVersion_Int, (int)1},
    {Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool, true},
    {Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, QDateTime::fromSecsSinceEpoch(0)}
};

Config::Config(const QString &fileName, QObject *parent, bool clearConfig)
    : QObject{parent}
{
    // Get all app config locations
    QStringList appConfigLocations = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);

    // We need at least one location
    if (appConfigLocations.size() == 0)
    {
        qCFatal(config, "No app config location found!!");
    }

    // Pick the first location which also has the highest priority
    QDir appConfigDir = QDir(appConfigLocations.at(0));

    // Create the directory structure if the location does not exist
    createPathIfNeeded(appConfigDir);

    // Log config location
    qCInfo(config) << "Config location:" << appConfigDir.absoluteFilePath(fileName);

    // Init QSettings with the config location
    this->settings = new QSettings(appConfigDir.absoluteFilePath(fileName), QSettings::Format::IniFormat, this);

    // Clear the config if needed
    if (clearConfig)
        this->settings->clear();

    // Write missing keys to the config file
    writeDefaultConfig();

    // Get config version
    int configVersion = getInt(Config::Setting::ConfigVersion_Int);

    // Check if the config version is below minimum
    if (configVersion < minConfigVersion)
    {
        // We don't know what config this is so we set it to the minConfigVersion
        // This should cause an upgrade - see below
        configVersion = minConfigVersion;
        settings->setValue(getKey(Config::Setting::ConfigVersion_Int), configVersion);
    }

    // Check if the config version is higher than the current default
    if (configVersion > getDefaultValue(Config::Setting::ConfigVersion_Int).toInt())
    {
        qCCritical(config) << "The config version" << configVersion << "is higher than the supported config version" << getDefaultValue(Config::Setting::ConfigVersion_Int).toInt();
        throw ConfigVersionTooHighError(this->settings->fileName());
    }

    // Upgrade the config if the config version is lower than the current default value
    if (configVersion < getDefaultValue(Config::Setting::ConfigVersion_Int).toInt())
    {
        upgradeConfig(configVersion, getDefaultValue(Config::Setting::ConfigVersion_Int).toInt());
    }
}

/*
 * ==================================
 *           Getter/Setter
 * ==================================
 */

int Config::getInt(Config::Setting setting) const
{
    bool ok = false;
    int value = settings->value(getKey(setting)).toInt(&ok);
    if (!ok)
    {
        // Get default value
        value = getDefaultValue(setting).toInt();

        // Save back the default value
        settings->setValue(getKey(setting), value);
    }

    return value;
}
bool Config::getBool(Config::Setting setting) const
{
    bool value = settings->value(getKey(setting)).toBool();
    if (!settings->contains(getKey(setting)))
    {
        // Get default value
        value = getDefaultValue(setting).toBool();

        // Save back the default value
        settings->setValue(getKey(setting), value);
    }

    return value;
}
QDateTime Config::getQDateTime(Config::Setting setting) const
{
    QDateTime value = settings->value(getKey(setting)).toDateTime();
    if (!value.isValid())
    {
        // Get default value
        value = getDefaultValue(setting).toDateTime();

        // Save back the default value
        settings->setValue(getKey(setting), value);
    }

    return value;
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void Config::writeDefaultConfig()
{
    // Iterate over all the values
    for (const QString& k : Config::settingsKeyMap)
    {
        if (this->settings->contains(k))
            continue;

        // Write the current key
        this->settings->setValue(k, getDefaultValue(Config::settingsKeyMap.key(k)));
    }
}

void Config::backupConfig() const
{
    QString backupFileName = this->settings->fileName().append(".bak");
    int fileDifferentiator = 1;
    while (QFileInfo::exists(backupFileName))
    {
        backupFileName = this->settings->fileName().append(".bak.").append(QString::number(fileDifferentiator));
        fileDifferentiator++;
    }

    if (!QFile::copy(this->settings->fileName(), backupFileName))
    {
        qCFatal(config) << "Backup of config failed! Copy from" << this->settings->fileName() << "to" << backupFileName;
    }
}

void Config::upgradeConfig(int oldVersion, int newVersion)
{
    // Backup the current config
    backupConfig();

    // Upgrade
//    switch (oldVersion)
//    {
//    case 1:
//        // Update the config from version 1 to 2
//        oldVersion = 2;
//    case 2:
//        // Update the config from version 1 to 2
//        oldVersion = 2;
//    }

    if (oldVersion < newVersion)
        qCWarning(config) << "[Development Error] Config upgrade step(s) from" << oldVersion << "to" << newVersion << "is/are missing!";
}
