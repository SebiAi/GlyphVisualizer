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

#include "ConfigurationManager.h"

// Logging
Q_LOGGING_CATEGORY(configurationManager, "ConfigurationManager")
Q_LOGGING_CATEGORY(configurationManagerVerbose, "ConfigurationManager.Verbose")

const QColor ConfigurationManager::glyphOnColor = QColor{QStringLiteral("#f0f2f2")}.toHsv();
const QRegularExpression ConfigurationManager::composerExpression(QStringLiteral(R"((?:v(\d+)-)?(\w+) Glyph Composer)"));

ConfigurationManager::ConfigurationManager()
    : QObject{}
{
    QColor offColor = brightnessToGlyphColor(0);
    configurations[Phone1Configuration::staticBuild] = QSharedPointer<Phone1Configuration>::create(offColor);
    configurations[Phone2Configuration::staticBuild] = QSharedPointer<Phone2Configuration>::create(offColor);
    configurations[Phone2aConfiguration::staticBuild] = QSharedPointer<Phone2aConfiguration>::create(offColor);
}

IConfiguration* ConfigurationManager::getConfiguration(DeviceBuild device) {
    if (!this->configurations.contains(device)) {
        qCCritical(configurationManager) << "Requested invalid DeviceBuild:" << device;
        throw std::logic_error("Requested invalid DeviceBuild!");
    }

    return this->configurations.value(device).get();
}

DeviceBuild ConfigurationManager::loadCompositionFromAudio(const QString& audioPath) {
    qCInfo(configurationManager) << "Loading composition from audio...";

    // Make sure the file exists
    QFileInfo fileInfo{audioPath};
    if (!fileInfo.isFile()) {
        throw SourceFileException("The audio file '" + audioPath.toStdString() + "' could not be found!");
    }
    if (fileInfo.suffix() != QStringLiteral("ogg")) {
        throw SourceFileException("Wrong file extension! Only opus files with the .ogg extension are supported!");
    }

    // Read the audio tags with TagLib
    // TagLib also makes sure that the codec is opus
    TagLib::FileRef f{audioPath.toStdString().c_str()};
    if (f.isNull()) {
        throw SourceFileException("Failed to open file '" + audioPath.toStdString() + "'!");
    }
    TagLib::PropertyMap tags;
    if (auto file = dynamic_cast<TagLib::Ogg::Opus::File *>(f.file())) {
        tags = file->properties();
    } else {
        throw SourceFileException("Wrong audio codec! Only opus files with the .ogg extension are supported!");
    }
    if (!tags.contains("AUTHOR") || tags["AUTHOR"].size() != 1)
        throw InvalidLightDataException("Malformed light data! Could not extract the 'AUTHOR' tag.");
    QByteArray author{tags.value("AUTHOR").front().to8Bit().c_str()};
    qCInfo(configurationManagerVerbose) << "Author raw:" << author;
    if (!tags.contains("COMPOSER") || tags["COMPOSER"].size() != 1)
        throw InvalidLightDataException("Malformed light data! Could not extract the 'COMPOSER' tag.");
    QString composer{tags.value("COMPOSER").front().to8Bit().c_str()};
    qCInfo(configurationManagerVerbose) << "Composer:" << composer;

    // Get DeviceBuild from the composer string
    QRegularExpressionMatch match{ConfigurationManager::composerExpression.match(composer)};
    if (!match.hasMatch())
        throw InvalidLightDataException("This is not a valid composition! (invalid composer)");
    // Check the version of the composer if matched
    // No match means that it is the initial composer version that launched with Phone (2)
    if (match.hasCaptured(1)) {
        qCInfo(configurationManagerVerbose) << "Captured composer version:" << match.captured(1);
        if (match.captured(1).toInt() > 1)
            throw InvalidLightDataException("This composition format is not supported yet. Please try updating " + QCoreApplication::applicationName().toStdString() + ".");
    } else {
        qCInfo(configurationManagerVerbose) << "No composer version captured => 0";
    }
    // Get the DeviceBuild from the string
    qCInfo(configurationManagerVerbose) << "Captured device build:" << match.captured(2);
    bool deviceSupported{false};
    DeviceBuild build{QMetaEnum::fromType<DeviceBuild>().keyToValue(match.captured(2).toStdString().c_str(), &deviceSupported)};
    if (!deviceSupported)
        throw InvalidLightDataException("Device '" + match.captured(2).toStdString() + "' is either invalid or not supported yet!");

    // No need to support the Phone (2a) Plus if it is identical to the Phone (2a)
    if (build == DeviceBuild::PacmanPro)
        build = DeviceBuild::Pacman;

    qCInfo(configurationManagerVerbose) << "Parsed device build:" << build;

    // Base64 decode
    QByteArray::FromBase64Result authorBase64Result{QByteArray::fromBase64Encoding(author)};
    if (authorBase64Result.decodingStatus != QByteArray::Base64DecodingStatus::Ok)
        throw InvalidLightDataException("Malformed light data! Could not decode data.");
    author = authorBase64Result.decoded;
    qCInfo(configurationManagerVerbose) << "Author decoded:" << author;

    // Decompress
    // Prepend the 4 byte header indicating the expected size (we use 200k bytes = 0x030D40) and uncompress the data
    // See here: https://doc.qt.io/qt-6/qbytearray.html#qUncompress
    author.prepend(QByteArray::fromHex("00030D40"));
    QString decodedAuthor{qUncompress(author)};
    if (decodedAuthor.trimmed().isEmpty())
        throw InvalidLightDataException("Malformed light data! Could not uncompress data.\nAre you sure that you selected the right entry in the dropdown?");
    qCInfo(configurationManagerVerbose) << "Author csv:" << decodedAuthor;

    // Parse the data
    QList<QList<int>> lightData{parseLightData(decodedAuthor)};
    // We still need to check here bc. a non empty decodedAuthor string can still result
    // in an empty list e.g.: '\n'
    if (lightData.empty())
        throw InvalidLightDataException("Malformed light data! No valid light values (empty).");
    // qCInfo(configurationManagerVerbose) << "Parsed light data:" << lightData;

    // Check if the number of columns are consistent and valid
    IConfiguration* config{getConfiguration(build)};
    qsizetype firstSize{lightData.first().size()};
    if (!config->supportedZones.contains(firstSize))
        throw InvalidLightDataException(
            std::string("The amount of zones does not match the amount of supported zones (")
                .append(QMetaEnum::fromType<DeviceBuild>().valueToKey((int)build))
                .append("). Got: ").append(std::to_string(firstSize))
                .append(", Expected: ").append(listToString(config->supportedZones).toStdString())
        );
    for (const QList<int>& row: lightData) {
        if (row.size() != firstSize)
            throw InvalidLightDataException(
                std::string("At least one line has a different length than the others. Got: ")
                    .append(std::to_string(row.size()))
                    .append(", Expected: ").append(std::to_string(firstSize))
            );
    }

    // Convert the ints to QColors we can use to color in the Glyphs
    QList<QList<QColor>> colors;
    colors.reserve(lightData.size());
    for (const QList<int>& row: lightData) {
        QList<QColor> colorRow;
        colorRow.reserve(row.size());
        std::transform(row.cbegin(), row.cend(), std::back_inserter(colorRow), [](const int& v){
            return ConfigurationManager::brightnessToGlyphColor(v);
        });
        colors.append(colorRow);
    }
    // qCInfo(configurationManagerVerbose) << "Color data:" << colors;

    // Set the colors
    config->parsedColors = colors;

    qCInfo(configurationManager) << "Loaded composition successfully!";

    return build;
}
DeviceBuild ConfigurationManager::loadCompositionFromNglyph(const QString& nglyphPath) {
    // TODO: Implement loadCompositionFromNglyph
    throw SourceFileException("NOT IMPLEMENTED YET!");
}

QList<QList<int>> ConfigurationManager::parseLightData(const QString& lightData) {
    QList<QList<int>> data;

    const QList<QString> lightDataList{QString{lightData}.replace(QString{"\r\n"}, QString{"\n"}).split("\n")};

    for (const QString& line: lightDataList) {
        // Remove whitespace and trailing comma
        QString trimmedLine{line.trimmed()};
        if (trimmedLine.endsWith(','))
            trimmedLine.removeLast();

        // Skip empty lines
        if (trimmedLine.isEmpty())
            continue;

        // Split lines and map to ints
        QList<QString> split{trimmedLine.split(',', Qt::SplitBehaviorFlags::SkipEmptyParts)};
        QList<int> rowData;
        bool ok = true;
        std::transform(split.cbegin(), split.cend(), std::back_inserter(rowData), [&ok](const QString& s){
            bool lOk = false;
            int i{s.toInt(&lOk)};

            // | ok | lOk | result |
            // | -- | --- | ------ |
            // | 0  | 0   | 0      |
            // | 0  | 1   | 0      |
            // | 1  | 0   | 0      |
            // | 1  | 1   | 1      | <= Only if the current conversion and all prior ones were successful we want true as a result
            ok = ok && lOk;

            return i;
        });

        // Catch conversion error
        if (!ok)
            throw InvalidLightDataException("Malformed light data! Could not convert to integers: '" + line.toStdString() + "'");

        data.append(rowData);
    }

    return data;
}

QColor ConfigurationManager::brightnessToGlyphColor(int value) {
    // Clamp the value and calculate percentage (0-1)
    qreal percentage = std::max(0., std::min(1., ((qreal)value) / ConfigurationManager::maxLightValue));

    // Calculate the color
    // We want to only modify the brightness => HSV color space
    // We have a min brightness (off) (ConfigurationManager::glyphOffHSVValue)
    // We scale the brighness linearly to the brightness input - an exponential curve would be more realistic
    return QColor::fromHsvF(ConfigurationManager::glyphOnColor.hueF(),
                            ConfigurationManager::glyphOnColor.saturationF(),
                            percentage * (ConfigurationManager::glyphOnColor.valueF() - ConfigurationManager::glyphOffHSVValue) + ConfigurationManager::glyphOffHSVValue);
}
