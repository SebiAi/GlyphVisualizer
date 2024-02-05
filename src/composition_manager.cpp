#include "composition_manager.h"

// Logging
Q_LOGGING_CATEGORY(compositionManager, "CompositionManager")
Q_LOGGING_CATEGORY(compositionManagerVerbose, "CompositionManager.Verbose")

const QList<QString> CompositionManager::phoneModelStrings = {
    "Phone1",
    "Phone2"
};

inline void CompositionManager::init()
{
    // Check glyphOffValue
    Q_ASSERT(this->glyphOffValue >= 0 && this->glyphOffValue <= 1.0);

    // Init off values
    colorOffValuesPhone1 = new QList<QColor>();
    colorOffValuesPhone1->reserve((int)GlyphMode::Compatibility);
    for (quint8 i = 0; i < numberOfZones[(int)GlyphMode::Compatibility]; i++)
        colorOffValuesPhone1->append(this->getGlyphColor(0));
    colorOffValuesPhone2 = new QList<QColor>();
    colorOffValuesPhone2->reserve(numberOfZones[(int)GlyphMode::Phone2]);
    for (quint8 i = 0; i < numberOfZones[(int)GlyphMode::Phone2]; i++)
        colorOffValuesPhone2->append(this->getGlyphColor(0));

    // Init lists
    colorValuesPhone1 = new QList<QList<QColor>>();
    colorValuesPhone2 = new QList<QList<QColor>>();

    // Create player
    this->player = new QMediaPlayer(this);
    this->audioOutput = new QAudioOutput(this->player);

    // Create elapsed timer
    this->elapsedTimer = new QElapsedTimer();
}

CompositionManager::CompositionManager(const QColor& glyphOnColor, const qreal& glyphOffValue, QObject *parent)
    : QObject{parent}, glyphOnColor{glyphOnColor}, glyphOffValue{glyphOffValue}
{
    this->init();
}

CompositionManager::CompositionManager(const QColor& glyphOnColor, const qreal& glyphOffValue, const QString &filepathAudio, const QString &filepathLightData, const OpenCompositionDialog::CompositionOpenModeResult& mode, QObject *parent)
    : QObject{parent}, glyphOnColor{glyphOnColor}, glyphOffValue{glyphOffValue}
{
    this->init();

    this->loadComposition(filepathAudio, filepathLightData, mode);
}

/*
 * ==================================
 *           Getter/Setter
 * ==================================
 */

/**
 * @brief Calcualtes the index from the current audio possition.
 * @param position Audio position in ms.
 * @return The index for the value lists.
 */
const qsizetype getIndexFromPosition(const qint64& position) { return position / COMPOSITION_MANAGER_MS_PER_LINE; }
qsizetype lastIndex = 0;
const QList<QColor>* const CompositionManager::getPhoneColorValues(const qint64& position, const CompositionManager::PhoneModel& phone) const
{
    const qsizetype index = getIndexFromPosition(position);


    // TODO: REMOVE ME!
    qsizetype indexDelta = index - lastIndex - 1;
    if (indexDelta != 0)
        // qDebug().nospace() << "[WARNING] Skipped " << indexDelta << " indexes!";
        lastIndex = index;

    QList<QList<QColor>>* colorValues = nullptr;
    QList<QColor>* colorOffValues = nullptr;
    switch (phone)
    {
    case CompositionManager::PhoneModel::Phone1:
        colorValues = this->colorValuesPhone1;
        colorOffValues = this->colorOffValuesPhone1;
        break;
    case CompositionManager::PhoneModel::Phone2:
        colorValues = this->colorValuesPhone2;
        colorOffValues = this->colorOffValuesPhone2;
        break;
    case CompositionManager::PhoneModel::None:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] Phone model None in function '").append(__FUNCTION__).append("' - can't be None!"));
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        //return;
    }

    if (index >= colorValues->length())
    {
        // Overrun => just return off state
        return colorOffValues;
    }

    return &colorValues->at(index);
}

QString CompositionManager::getPhoneModelString(const CompositionManager::PhoneModel& phone)
{
    switch (phone)
    {
    case CompositionManager::PhoneModel::Phone1:
    case CompositionManager::PhoneModel::Phone2:
        return CompositionManager::phoneModelStrings.at((qsizetype)phone);
        break;
    case CompositionManager::PhoneModel::None:
        return "INVALID";
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        //return;
    }
}

QColor CompositionManager::getGlyphColor(const qreal& percent)
{
    Q_ASSERT(percent >= 0 && percent <= 1);
    return QColor::fromHsvF(this->glyphOnColor.hueF(),
                            this->glyphOnColor.saturationF(),
                            percent * (this->glyphOnColor.valueF() - this->glyphOffValue) + this->glyphOffValue
                            );
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void CompositionManager::loadComposition(const QString &filepathAudio, const QString &filepathLightData, const OpenCompositionDialog::CompositionOpenModeResult& mode)
{
    // Check if the files exist
    QFileInfo audioFileInfo = QFileInfo(filepathAudio);
    QFileInfo lightDataFileInfo = QFileInfo(filepathLightData);
    if (!audioFileInfo.exists() || !audioFileInfo.isFile())
    {
        qCWarning(compositionManager) << "The file" << filepathAudio << "could not be found";
        throw std::invalid_argument(std::string("The file '").append(filepathAudio.toStdString()).append("' could not be found!"));
    }
    if (!filepathLightData.isEmpty() && (!lightDataFileInfo.exists() || !lightDataFileInfo.isFile()))
    {
        qCWarning(compositionManager) << "The file" << lightDataFileInfo << "could not be found";
        throw std::invalid_argument(std::string("The light data file '").append(filepathLightData.toStdString()).append("' could not be found!"));
    }

    // Parse light data
    parseLightData(filepathAudio, filepathLightData, mode);

    // Set up player
    this->playerInit(filepathAudio);
}

void CompositionManager::skipToPercentage(const qreal& percentage)
{
    // Check if the percentage is out of range
    if (percentage > 1.0 || percentage < 0.0)
        return;

    // Check if the player has a media loaded and is seekable
    if (!this->player->isSeekable() || !this->player->hasAudio())
        return;

    this->player->setPosition(this->player->duration() * percentage);
}

void CompositionManager::playerInit(const QString &filepathAudio)
{
    qCInfo(compositionManagerVerbose) << "Setting up the player";

//    connect(this->player, SIGNAL(metaDataChanged()), this, SLOT(player_onMetaDataChanged()), Qt::UniqueConnection);
    connect(this->player, SIGNAL(positionChanged(qint64)), this, SLOT(player_onPositionChanged(qint64)), Qt::UniqueConnection);
    connect(this->player, SIGNAL(playingChanged(bool)), this, SLOT(player_onPlayingChanged(bool)), Qt::UniqueConnection);
    connect(this->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(player_onMediaStatusChanged(QMediaPlayer::MediaStatus)), Qt::UniqueConnection);

    if (this->player->isPlaying())
        this->player->stop();

    this->player->setAudioOutput(this->audioOutput);
    this->player->setSource(QUrl::fromLocalFile(filepathAudio));
}

void CompositionManager::parseRawLightDataLine(const QString &rawLightDataLine, bool &firstLine, qsizetype &lineN, QList<QList<QColor>> *&globalColorValues)
{
    // Trim line and remove prepend ','
    QString line = rawLightDataLine.trimmed();
    if (line.endsWith(','))
        line = line.removeLast();

    // Skip lines that are empty
    if (line.isEmpty())
        return;

    // Detect the mode depending on the first line
    if (firstLine)
    {
        this->currentGlyphMode = (line.split(',').length() == numberOfZones[(int)GlyphMode::Compatibility]) ? GlyphMode::Compatibility :
                               ((line.split(',').length() == numberOfZones[(int)GlyphMode::Phone2]) ? GlyphMode::Phone2 : GlyphMode::None);

        std::string columnExceptionString = "";
        switch (currentGlyphMode)
        {
        case GlyphMode::None:
            for (qsizetype i = 0; i < ARRAYSIZE(numberOfZones); i++)
            {
                columnExceptionString.append(i?"/":"").append(std::to_string(numberOfZones[i]));
            }

            throw InvalidLightDataContentException(std::string("Malformed light data! Does not have ").append(columnExceptionString).append(" columns."));
            return;
        case GlyphMode::Compatibility:
            globalColorValues = colorValuesPhone1;
            break;
        case GlyphMode::Phone2:
            globalColorValues = colorValuesPhone2;
            break;
        default:
            // This should never happen
            throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
            return;
        }

        qCInfo(compositionManagerVerbose) << "Detected" << currentGlyphMode << "mode";

        firstLine = false;
    }

    QList<QColor> colorValues;
    bool conversionSuccess;
    for (const QString& s: line.split(','))
    {
        // Convert number to string
        uint number = s.toUInt(&conversionSuccess);
        // Check if the conversion was successfull
        if (!conversionSuccess)
            throw InvalidLightDataContentException(std::string("Malformed light data! Could not convert '").append(s.toStdString()).append("' to an unsigned integer in line ").append(std::to_string(lineN)).append("."));

        // Calculate color
        colorValues.append(this->getGlyphColor(std::min(number, (uint)COMPOSITION_MANAGER_MAX_LIGHT_VALUE) / (qreal)COMPOSITION_MANAGER_MAX_LIGHT_VALUE));
    }
    // Check the number of columns in the light data
    if (colorValues.length() != numberOfZones[(int)currentGlyphMode])
        throw InvalidLightDataContentException(std::string("Malformed light data! Line ").append(std::to_string(lineN)).append(" does not have ").append(std::to_string(numberOfZones[(int)currentGlyphMode])).append(" columns."));

    // Add current line to the data
    globalColorValues->append(colorValues);

    if (currentGlyphMode == GlyphMode::Compatibility)
    {
        // Convert the Phone (1) light data to Phone (2) light data
        QList<QColor> tmpColor(numberOfZones[(int)GlyphMode::Phone2]);
        for (qsizetype i = 0; i < numberOfZones[(int)GlyphMode::Phone2]; i++)
        {
            tmpColor[i] = colorValues.at(zone5To33LookupTable[i]);
        }

        // Add current line to the data
        colorValuesPhone2->append(tmpColor);
    }

    lineN++;
}
void CompositionManager::parseLightData(const QString &filepathAudio, const QString &filepathLightData, const OpenCompositionDialog::CompositionOpenModeResult& mode)
{
    // Used by all
    bool firstLine = true;
    qsizetype lineN = 1;
    QList<QList<QColor>> *globalColorValues = nullptr;
    // Audio only
    TagLib::FileRef tagLibFile;
    TagLib::PropertyMap tags;
    QByteArray author;
    QByteArray::FromBase64Result authorBase64Result;
    QString decodedAuthor;
    // Audio light data
    QFile file(filepathLightData);
    switch (mode)
    {
    case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_ONLY:
        qCInfo(compositionManager) << "Parsing composition (Audio Only)";

        tagLibFile = TagLib::FileRef(filepathAudio.toStdString().c_str());
        tags = tagLibFile.tag()->properties();
        // Check data
        if (!tags.contains("AUTHOR") || tags["AUTHOR"].size() != 1)
            throw InvalidLightDataContentException(std::string("Malformed light data! Could not extract the 'AUTHOR' tag.\nAre you sure that you selected the right entry in the dropdown?"));

        // Get AUTHOR tag
        author = QByteArray(tags["AUTHOR"].front().to8Bit().c_str());
        qCInfo(compositionManagerVerbose) << "AUTHOR metadata:" << author;

        // Decode Base64
        authorBase64Result = QByteArray::fromBase64Encoding(author);
        if (authorBase64Result.decodingStatus != QByteArray::Base64DecodingStatus::Ok)
            throw InvalidLightDataContentException(std::string("Malformed light data! Could not decode data.\nAre you sure that you selected the right entry in the dropdown?"));
        author = authorBase64Result.decoded;

        // Prepend the 4 byte header indicating the expected size (we use 200k bytes = 0x030D40) and uncompress the data
        // See here: https://doc.qt.io/qt-6/qbytearray.html#qUncompress-1
        author.prepend(QByteArray::fromHex("00030D40"));
        decodedAuthor = QString(qUncompress(author));
        qCInfo(compositionManagerVerbose) << "AUTHOR decompressed:" << decodedAuthor;

        // Check decompressed data
        if (decodedAuthor.trimmed().isEmpty())
            throw InvalidLightDataContentException(std::string("Malformed light data! Could not uncompress data.\nAre you sure that you selected the right entry in the dropdown?"));

        // TODO: If this function errors out after this point the playback will work, but no light data is present (always off). Write to a tmp var and then replace the current values with the tmp values.
        // Clear current color value list
        colorValuesPhone1->clear();
        colorValuesPhone2->clear();

        // Parse the data
        for (const QString &line : decodedAuthor.split("\n"))
        {
            this->parseRawLightDataLine(line, firstLine, lineN, globalColorValues);
        }

        break;
    case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_LIGHT_DATA:
        qCInfo(compositionManager) << "Parsing composition (Audio + Light data)";

        if (!file.open(QIODevice::ReadOnly))
        {
            // Error opening file => throw error
            throw std::invalid_argument(std::string("Could not open light data file '").append(filepathLightData.toStdString()).append("':\n").append(file.errorString().toStdString()));
        }

        // TODO: If this function errors out after this point the playback will work, but no light data is present (always off). Write to a tmp var and then replace the current values with the tmp values.
        // Clear current color value list
        colorValuesPhone1->clear();
        colorValuesPhone2->clear();

        // Read all lines
        while (!file.atEnd())
        {
            // Read the line
            char buf[1024];
            qint64 readResult = file.readLine(buf, ARRAYSIZE(buf));

            // Check result
            if (readResult == -1)
            {
                throw std::invalid_argument(std::string("Could not fully read the light data file!"));
            }

            // Parse the line
            this->parseRawLightDataLine(QString(buf), firstLine, lineN, globalColorValues);
        }
        break;
    case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_AUDACITY_LABELS:
        qCInfo(compositionManager) << "Parsing composition (Audio + Audacity Labels)";
        throw std::logic_error(std::string("[Development Error] Audio only not implemented yet: '").append(__FUNCTION__).append("'!"));
        break;
    case OpenCompositionDialog::CompositionOpenModeResult::None:
        throw std::logic_error(std::string("[Development Error] Ups... This should not happen. Please report this error to the developer. '").append(__FUNCTION__).append("'!"));
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
        break;
    }
}

/*
 * ==================================
 *               Slots
 * ==================================
 */

void CompositionManager::player_onPositionChanged(qint64 position)
{
//    qDebug().nospace() << "player->position(): " << position << ", " << getFinerPlayerPosition();
}

void CompositionManager::player_onPlayingChanged(bool playing)
{
    if (!playing)
    {
        this->finerPlayerPosition = this->player->position();
        return;
    }

    // Enable timer
    this->elapsedTimer->start();
}

void CompositionManager::player_onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::StalledMedia:
        this->finerPlayerPosition = this->player->position();
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        this->elapsedTimer->start();
        break;
    default:
        this->finerPlayerPosition = 0;
    }
}

/*
 * ==================================
 *           Deconstructor
 * ==================================
 */

CompositionManager::~CompositionManager()
{
    delete colorValuesPhone1;
    delete colorOffValuesPhone1;
    delete colorValuesPhone2;
    delete colorOffValuesPhone2;

    delete elapsedTimer;
}
