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

#ifndef GV_CONFIG_H
#define GV_CONFIG_H

#include <QDateTime>
#include <QFileInfo>
#include <QMetaEnum>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringLiteral>
#include <QVariant>

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(config)
Q_DECLARE_LOGGING_CATEGORY(configVerbose)

class Config : private QObject
{
    Q_OBJECT
public:
    class ConfigVersionTooHighError: public std::runtime_error {
    public:
        const std::string filePath;
        explicit ConfigVersionTooHighError(const std::string& msg, const std::string& filePath): std::runtime_error{msg}, filePath{filePath} {}
    };

    enum class Setting
    {
        ConfigVersion_Int = 0,
        UpdateChecker_AutoUpdateCheckEnabled_Bool = 1,
        UpdateChecker_LastAutoUpdateCheck_QDateTime = 2,
        DonationDialog_DoNotShowAgain_Bool = 3,
        DonationDialog_LastShown_QDateTime = 4,
        FirstStart_Bool = 5
    };
    Q_ENUM(Setting)

    explicit Config(QObject *parent = nullptr);
    void load(const QString& configPath, bool resetConfig = false);

    int getInt(Config::Setting setting) const { return getValue<int>(setting); }
    bool getBool(Config::Setting setting) const { return getValue<bool>(setting); }
    QDateTime getQDateTime(Config::Setting setting) const { return getValue<QDateTime>(setting); }

    void setValue(Config::Setting setting, const QVariant& value);
    void resetConfig();

signals:

private:
    static const QMap<Config::Setting, QString> settingsKeyMap;
    static const QMap<Config::Setting, QVariant> settingsDefaultValues;
    static constexpr int minConfigVersion{1};

    QSettings* settings;

    template <typename T>
    T getValue(Config::Setting setting) const {
        QVariant defaultValue{getDefaultValueSafe(setting)};

        // Check type compatibility between type of defaultValue and T
        if (defaultValue.metaType() != QMetaType::fromType<T>())
            throw std::logic_error(
                std::string("Setting ")
                    .append(QMetaEnum::fromType<Config::Setting>().valueToKey((int)setting))
                    .append(" is not compatible with type ")
                    .append(typeid(T).name())
            );

        // Get the setting
        QVariant variant{this->settings->value(settingToKeySafe(setting))};

        // Check if conversion is possible
        // canConvert returns true for values like "123" if T is type bool
        if (!variant.canConvert<T>()) {
            // Save back the default value
            this->settings->setValue(settingToKeySafe(setting), defaultValue);
            return defaultValue.value<T>();
        }

        return variant.value<T>();
    }
    static QString settingToKeySafe(Config::Setting setting);
    static QVariant getDefaultValueSafe(Config::Setting setting);

    void writeDefaultConfig();

    void backupConfig() const;
    void upgradeConfig(int oldVersion, int newVersion);
};

#endif // GV_CONFIG_H
