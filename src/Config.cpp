/*
This file is part of the GlyphVisualizer project, a Glyph composition
player that plays Glyph compositions from Nothing phones.
Copyright (C) 2025  Sebastian Aigner (aka. SebiAi)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Config.h"

// Logging
Q_LOGGING_CATEGORY(config, "Config")
Q_LOGGING_CATEGORY(configVerbose, "Config.Verbose")

const QMap<Config::Setting, QString> Config::settingsKeyMap = {
	{Config::Setting::ConfigVersion_Int, QStringLiteral("ConfigVersion")},
	{Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool, QStringLiteral("UpdateChecker/AutoUpdateCheckEnabled")},
	{Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, QStringLiteral("UpdateChecker/LastAutoUpdateCheck")},
	{Config::Setting::DonationDialog_DoNotShowAgain_Bool, QStringLiteral("Donation/DoNotShowAgain")},
	{Config::Setting::DonationDialog_LastShown_QDateTime, QStringLiteral("Donation/LastShown")},
	{Config::Setting::FirstStart_Bool, QStringLiteral("FirstStart")}
};

const QMap<Config::Setting, QVariant> Config::settingsDefaultValues = {
	{Config::Setting::ConfigVersion_Int, (int)1},
	{Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool, true},
	{Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, QDateTime::fromSecsSinceEpoch(0)},
	{Config::Setting::DonationDialog_DoNotShowAgain_Bool, false},
	{Config::Setting::DonationDialog_LastShown_QDateTime, QDateTime::fromSecsSinceEpoch(0)},
	{Config::Setting::FirstStart_Bool, true}
};

Config::Config(QObject *parent)
	: QObject{parent}
{

}

void Config::load(const QString& configPath, bool resetConfig) {
	qCInfo(config).nospace() << "Loading config from " << configPath << ", (reset=" << resetConfig << ")";

	// We trust that configPath is an absolute existing path to a file.
	this->settings = new QSettings{configPath, QSettings::Format::IniFormat, this};

	// Reset the config if needed
	if (resetConfig)
		this->resetConfig();
	else
		writeDefaultConfig(); // Write missing keys to the config file

	// Get config version
	int configVersion{getInt(Config::Setting::ConfigVersion_Int)};
	qCInfo(configVerbose) << "Config version:" << configVersion;
	if (configVersion < Config::minConfigVersion) {
		// We don't know what config this is so we set it to the minConfigVersion
		// This should cause an upgrade - see below
		qCWarning(config) << "Invalid config version, force setting to" << Config::minConfigVersion;
		configVersion = Config::minConfigVersion;
		settings->setValue(settingToKeySafe(Config::Setting::ConfigVersion_Int), configVersion);
	}

	// Check if the config version is higher than the current default
	int defaultConfigVersion{getDefaultValueSafe(Config::Setting::ConfigVersion_Int).toInt()};
	if (configVersion > defaultConfigVersion)
	{
		throw Config::ConfigVersionTooHighError("Configuration file version not supported! Try upgrading the software.", this->settings->fileName().toStdString());
	}

	// Upgrade the config if the config version is lower than the current default value
	if (configVersion < defaultConfigVersion)
	{
		qCInfo(config).nospace() << "Config version below default (" << defaultConfigVersion << "), upgrading...";
		upgradeConfig(configVersion, defaultConfigVersion);
	}
}

void Config::setValue(Config::Setting setting, const QVariant& value) {
	if (value.metaType() != getDefaultValueSafe(setting).metaType())
		throw std::logic_error(
			std::string("Setting ")
				.append(QMetaEnum::fromType<Config::Setting>().valueToKey((int)setting))
				.append(" is not compatible with the default value type.")
		);

	this->settings->setValue(settingToKeySafe(setting), value);
}
void Config::resetConfig() {
	qCInfo(configVerbose) << "Resetting config";

	this->settings->clear();
	writeDefaultConfig();
};

QString Config::settingToKeySafe(Config::Setting setting) {
	if (!Config::settingsKeyMap.contains(setting))
		throw std::logic_error("settingsKeyMap not updated for setting " + std::to_string((int)setting));

	return Config::settingsKeyMap[setting];
}
QVariant Config::getDefaultValueSafe(Config::Setting setting) {
	if (!Config::settingsDefaultValues.contains(setting))
		throw std::logic_error("settingsDefaultValues not updated for setting " + std::to_string((int)setting));

	return Config::settingsDefaultValues[setting];
}

void Config::writeDefaultConfig()
{
	// Iterate over all the values
	for (const QString& k : Config::settingsKeyMap)
	{
		if (this->settings->contains(k))
			continue;

		// Write the current key
		qCInfo(configVerbose) << "Key" << k << "missing in config, writing default";
		this->settings->setValue(k, getDefaultValueSafe(Config::settingsKeyMap.key(k)));
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

	qCInfo(config) << "Making backup of config" << this->settings->fileName() << "to" << backupFileName;
	if (!QFile::copy(this->settings->fileName(), backupFileName))
	{
		std::runtime_error("Backup of config failed! Copy from" + this->settings->fileName().toStdString() + "to" + backupFileName.toStdString());
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
		throw std::logic_error("Config upgrade step(s) from " + std::to_string(oldVersion) + "to" + std::to_string(newVersion) + "is/are missing!");
}
