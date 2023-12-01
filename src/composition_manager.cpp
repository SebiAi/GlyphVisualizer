#include "composition_manager.h"

inline void CompositionManager::init()
{
    // Init off values
    opacityOffValuesPhone1 = new QList<qreal>();
    for (quint8 i = 0; i < numberOfZones[(int)GlyphMode::Compatibility]; i++)
        opacityOffValuesPhone1->append(MINIMUM_GLYPH_OPACITY_VALUE);
    opacityOffValuesPhone2 = new QList<qreal>();
    for (quint8 i = 0; i < numberOfZones[(int)GlyphMode::Phone2]; i++)
        opacityOffValuesPhone2->append(MINIMUM_GLYPH_OPACITY_VALUE);

    // Init lists
    opacityValuesPhone1 = new QList<QList<qreal>>();
    opacityValuesPhone2 = new QList<QList<qreal>>();

    // Create player
    this->player = new QMediaPlayer();
    this->audioOutput = new QAudioOutput();

    // Create elapsed timer
    this->elapsedTimer = new QElapsedTimer();
}

CompositionManager::CompositionManager()
    : QObject{}
{
    this->init();
}

CompositionManager::CompositionManager(const QString &filepathAudio, const QString &filepathLightData)
    : QObject{}
{
    this->init();

    this->loadComposition(filepathAudio, filepathLightData);
}

/*
 * ==================================
 *           Getter/Setter
 * ==================================
 */
qsizetype lastIndex = 0;
const QList<qreal>* const CompositionManager::getPhone1OpacityValues(qint64 position) const
{
    qsizetype index =  position / COMPOSITION_MANAGER_MS_PER_LINE;


    // TODO: REMOVE ME!
    qsizetype indexDelta = index - lastIndex - 1;
    if (indexDelta > 0)
        qDebug().nospace() << "[WARNING] Skipped " << indexDelta << " indexes!";
    lastIndex = index;


    if (index >= opacityValuesPhone1->length())
    {
        // Overrun => just return off state
        return opacityOffValuesPhone1;
    }

    return &opacityValuesPhone1->at(index);
}
const QList<qreal>* const CompositionManager::getPhone2OpacityValues(qint64 position) const
{
    qsizetype index =  position / COMPOSITION_MANAGER_MS_PER_LINE;


    // TODO: REMOVE ME!
    qsizetype indexDelta = index - lastIndex - 1;
    if (indexDelta > 0)
        qDebug().nospace() << "[WARNING] Skipped " << indexDelta << " indexes!";
    lastIndex = index;


    if (index >= opacityValuesPhone2->length())
    {
        // Overrun => just return off state
        return opacityOffValuesPhone2;
    }

    return &opacityValuesPhone2->at(index);
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void CompositionManager::loadComposition(const QString &filepathAudio, const QString &filepathLightData)
{
    // Check if the files exist
    QFileInfo audioFileInfo = QFileInfo(filepathAudio);
    QFileInfo lightDataFileInfo = QFileInfo(filepathLightData);
    if (!audioFileInfo.exists() || !audioFileInfo.isFile())
        throw std::invalid_argument(std::string("The audio file '").append(filepathAudio.toStdString()).append("' could not be found!"));
    if (!lightDataFileInfo.exists() || !lightDataFileInfo.isFile())
        throw std::invalid_argument(std::string("The light data file '").append(filepathLightData.toStdString()).append("' could not be found!"));

    // Parse light data
    parseLightData(filepathLightData);

    // Set up player
    this->playerInit(filepathAudio);
}

void CompositionManager::playerInit(const QString &filepathAudio)
{
//    connect(this->player, SIGNAL(metaDataChanged()), this, SLOT(player_onMetaDataChanged()), Qt::UniqueConnection);
    connect(this->player, SIGNAL(positionChanged(qint64)), this, SLOT(player_onPositionChanged(qint64)), Qt::UniqueConnection);
    connect(this->player, SIGNAL(playingChanged(bool)), this, SLOT(player_onPlayingChanged(bool)), Qt::UniqueConnection);
    connect(this->player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(player_onMediaStatusChanged(QMediaPlayer::MediaStatus)), Qt::UniqueConnection);

    if (this->player->isPlaying())
        this->player->stop();

    this->player->setAudioOutput(this->audioOutput);
    this->player->setSource(QUrl::fromLocalFile(filepathAudio));
}

void CompositionManager::parseLightData(const QString &filepathLightData)
{
    QFile file(filepathLightData);
    if (!file.open(QIODevice::ReadOnly))
    {
        // Error opening file => throw error
        throw std::invalid_argument(std::string("Could not open light data file '").append(filepathLightData.toStdString()).append("':\n").append(file.errorString().toStdString()));
    }

    // Clear current opacity value list
    opacityValuesPhone1->clear();
    opacityValuesPhone2->clear();

    bool firstLine = true;
    qsizetype lineN = 1;
    QList<QList<qreal>> *globalOpacityValues;
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

        // Trim line and remove prepend ','
        QString line = QString(buf).trimmed();
        if (line.endsWith(','))
            line = line.removeLast();

        // Skip lines that are empty
        if (line.isEmpty())
            continue;

        // Detect the mode depending on the first line
        if (firstLine)
        {
            currentGlyphMode = (line.split(',').length() == numberOfZones[(int)GlyphMode::Compatibility]) ? GlyphMode::Compatibility :
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
                globalOpacityValues = opacityValuesPhone1;
                break;
            case GlyphMode::Phone2:
                globalOpacityValues = opacityValuesPhone2;
                break;
            default:
                // This should never happen
                throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
                return;
            }

            firstLine = false;
        }

        QList<qreal> opacityValues;
        bool conversionSuccess;
        for (const QString& s: line.split(','))
        {
            // Calculate opacity
            opacityValues.append((1.0 - MINIMUM_GLYPH_OPACITY_VALUE) * std::min(s.toUInt(&conversionSuccess), (uint)COMPOSITION_MANAGER_MAX_LIGHT_VALUE) / (qreal)COMPOSITION_MANAGER_MAX_LIGHT_VALUE + MINIMUM_GLYPH_OPACITY_VALUE);
            // Check if the conversion was successfull
            if (!conversionSuccess)
                throw InvalidLightDataContentException(std::string("Malformed light data! Could not convert '").append(s.toStdString()).append("' to an unsigned integer in line ").append(std::to_string(lineN)).append("."));
        }
        // Check the number of columns in the light data
        if (opacityValues.length() != numberOfZones[(int)currentGlyphMode])
            throw InvalidLightDataContentException(std::string("Malformed light data! Line ").append(std::to_string(lineN)).append(" does not have ").append(std::to_string(numberOfZones[(int)currentGlyphMode])).append(" columns."));

        // Add current line to the data
        globalOpacityValues->append(opacityValues);

        if (currentGlyphMode == GlyphMode::Compatibility)
        {
            // Convert the Phone (1) light data to Phone (2) light data
            QList<qreal> tmp(numberOfZones[(int)GlyphMode::Phone2]);
            for (qsizetype i = 0; i < numberOfZones[(int)GlyphMode::Phone2]; i++)
                tmp[i] = opacityValues.at(zone5To33LookupTable[i]);

            // Add current line to the data
            opacityValuesPhone2->append(tmp);
        }

        lineN++;
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
    qDebug().nospace() << "State: " << status;
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
    delete player;
    delete audioOutput;

    delete opacityValuesPhone1;
    delete opacityOffValuesPhone1;
    delete opacityValuesPhone2;
    delete opacityOffValuesPhone2;

    delete elapsedTimer;
}
