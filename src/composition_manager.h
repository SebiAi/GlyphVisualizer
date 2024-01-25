#ifndef COMPOSITIONMANAGER_H
#define COMPOSITIONMANAGER_H

#include <QObject>
#include <QString>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QFileInfo>
#include <QFile>
#include <QElapsedTimer>
#include <QTimer>
#include <QColor>
#include <QList>
#include "helper.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(compositionManager)
Q_DECLARE_LOGGING_CATEGORY(compositionManagerVerbose)

// TODO: [Next] Use QTimer to create a signal at 15ms and call GlyphWidget::update() with that
// TODO: To fix the low "refresh" issue: Make per Glyph glyphdata + spawn a thread for each glyph which waits and then changes the glyph/start a timer for each glyph

// TODO: [END] Try libvlc.so instead of the QMediaPlayer. The QMediaPlayer does not trigger the positionChanged signal only triggers every 40-60ms but we need 16ms or lower.

// The maximal value a light can be.
#define COMPOSITION_MANAGER_MAX_LIGHT_VALUE 4080
// How many milliseconds each line in the light data file is.
#define COMPOSITION_MANAGER_MS_PER_LINE 16

class CompositionManager : private QObject
{
    Q_OBJECT
public:
    /**
     * @brief Enum definition for the glyph modes. MUST be kept in sync with the numberOfZones array!
     */
    enum class GlyphMode {
        None = -1,           // No light data loaded
        Compatibility = 0,  // Compatibility mode => Phone (1) and Phone (2) compatible
        Phone2 = 1          // Phone2 mode => ONLY Phone (2) compatible
    };
    Q_ENUM(GlyphMode)
    /**
     * @brief Enum definition for the phones. Must be kept in sync with the phoneModelStrings QList.
     */
    enum class PhoneModel { None = -1, Phone1 = 0, Phone2 = 1 };
    Q_ENUM(PhoneModel)

    /**
     * @brief Exception for when the light data is illformed.
     */
    class InvalidLightDataContentException : public std::exception {
    public:
        const std::string message;

        InvalidLightDataContentException(const std::string& message) : message(message) {}

        inline const char* what() const noexcept override
        {
            return message.c_str();
        }
    };

    /**
     * @brief Empty constructor for the CompositionManager class. Use the CompositionManager::loadComposition() function to load a Composition.
     * @param glyphOnColor The color of the Glyphs if they are 100% on.
     * @param glyphOffValue Value of the HSV glyphOnColor where the Glyphs are 0% on. Range 0 to 1.
     * @param parent The parent object.
     */
    explicit CompositionManager(const QColor& glyphOnColor, const qreal& glyphOffValue, QObject *parent = nullptr);
    /**
     * @brief Load a Composition with a seperate ogg and light data file.
     * @param glyphOnColor The color of the Glyphs if they are 100% on.
     * @param glyphOffValue Value of the HSV glyphOnColor where the Glyphs are 0% on. Range 0 to 1.
     * @param filepathAudio Filepath to the audio ogg file.
     * @param filepathLightData Filepath to the light data file.
     * @param parent The parent object.
     *
     * @throws std::invalid_argument Audio file does not exist.
     * @throws std::invalid_argument Light data file does not exist.
     * @throws std::invalid_argument Can not open the light data file.
     * @throws CompositionManager::InvalidLightDataContentException Invalid light data in light data file.
     */
    explicit CompositionManager(const QColor& glyphOnColor, const qreal& glyphOffValue, const QString &filepathAudio, const QString &filepathLightData, QObject *parent = nullptr);
    ~CompositionManager();

    /**
     * @brief Get the current Glyph Mode depending on the currently loaded
     * @return See enum GlyphMode.
     */
    const GlyphMode& getGlyphMode() const { return currentGlyphMode; }
    /**
     * @brief Get the calculated color values for each of the Phone Glyphs for the position.
     * @param position The audio time in milliseconds.
     * @param phone For which phone.
     * @return A list with numberOfZones[(int)GlyphMode::Compatibility] elements, each corresponding to one Zone of the requested phone.
     */
    const QList<QColor>* const getPhoneColorValues(const qint64& position, const CompositionManager::PhoneModel& phone) const;
    /**
     * @brief Get the string representation of the phone.
     * @param phone For which phone
     * @return A string representation of the PhoneModel enum.
     */
    static QString getPhoneModelString(const CompositionManager::PhoneModel& phone);


    /**
     * @brief Load a composition with a seperate ogg and light data file.
     * @param filepathAudio Filepath to the audio ogg file.
     * @param filepathLightData Filepath to the light data file.
     *
     * @throws std::invalid_argument Audio file does not exist.
     * @throws std::invalid_argument Light data file does not exist.
     * @throws std::invalid_argument Can not open the light data file.
     * @throws CompositionManager::InvalidLightDataContentException Invalid light data in light data file.
     */
    void loadComposition(const QString &filepathAudio, const QString &filepathLightData);
    /**
     * @brief Skip to the provided percentage in the composition if one is loaded.
     * @param percentage The percentage. Range 0 to 1.
     */
    void skipToPercentage(const qreal& percentage);


    /**
     * @brief Media player which plays the audio.
     */
    QMediaPlayer *player;

    inline qint64 getFinerPlayerPosition() const
    {
        QMediaPlayer::MediaStatus state = this->player->mediaStatus();
        return this->finerPlayerPosition + (state == QMediaPlayer::BufferingMedia || state == QMediaPlayer::BufferedMedia ? this->elapsedTimer->elapsed() : 0);
    }

    QElapsedTimer *elapsedTimer;
    qint64 finerPlayerPosition = 0;

public slots:
    /**
     * @brief Will be called by the void QMediaPlayer::positionChanged(qint64 position) signal.
     * @param position Position expressed in milliseconds.
     */
    void player_onPositionChanged(qint64 position);
    /**
     * @brief Will be called by the void QMediaPlayer::playingChanged(bool playing) signal.
     * @param playing If the media is playing.
     */
    void player_onPlayingChanged(bool playing);
    void player_onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    /**
     * @brief Holds the string representation of the PhoneModel enum. Must be kept in sync with the enum.
     */
    static const QList<QString> phoneModelStrings;

    /**
     * @brief Number of Zones for the Phone (1) and Phone (2). MUST be kept in sync with the GlyphMode enum!
     */
    static constexpr quint8 numberOfZones[2] = {
        5,  // Phone (1)
        33  // Phone (2)
    };
    /**
     * @brief Defines a lookup table to convert a Phone (1) Composition (GlyphMode::Compatibility) to a Phone (2) Composition (GlyphMode::Phone2). Usage: phone2[x] = phone1[zone5To33LookupTable[x]]
     */
    static constexpr quint8 zone5To33LookupTable[numberOfZones[(int)GlyphMode::Phone2]] = {// Zone3 => Zone33
        0, 0,                                           // GLYPH_CAMERA => GLYPH_CAMERA_TOP, GLYPH_CAMERA_BOTTOM
        1,                                              // GLYPH_DIAGONAL => GLYPH_DIAGONAL
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // GLYPH_BATTERY => GLYPH_BATTERY_TOP_RIGHT(16)
        2, 2, 2, 2, 2,                                  // GLYPH_BATTERY => GLYPH_BATTERY_TOP_LEFT, GLYPH_BATTERY_TOP_VERTICAL_LEFT, GLYPH_BATTERY_BOTTOM_LEFT, GLYPH_BATTERY_BOTTOM_RIGHT, GLYPH_BATTERY_BOTTOM_VERTICAL_RIGHT
        4,                                              // GLYPH_USB_DOT => GLYPH_USB_DOT
        3, 3, 3, 3, 3, 3, 3, 3                          // GLYPH_USB_LINE => GLYPH_USB_LINE(8)
    };

    /**
     * @brief The color of the Glyphs if they are 100% on.
     */
    const QColor glyphOnColor;
    /**
     * @brief Value of the HSV glyphOnColor where the Glyphs are 0% on.
     */
    const qreal glyphOffValue;

    /**
     * @brief This function contains the initialisation code. It will be called by all constructors.
     */
    inline void init();

    /**
     * @brief Holds the default off values for Phone (1). Has a size of numberOfZones[(int)GlyphMode::Compatibility].
     */
    QList<QColor> *colorOffValuesPhone1;
    /**
     * @brief Holds the default off values for Phone (2). Has a size of numberOfZones[(int)GlyphMode::Phone2].
     */
    QList<QColor> *colorOffValuesPhone2;

    /**
     * @brief Audio output.
     */
    QAudioOutput *audioOutput;

    /**
     * @brief Holds the current glyph mode depending on the currently loaded light data.
     */
    GlyphMode currentGlyphMode = GlyphMode::None;

    /**
     * @brief Holds the color values for each of the Phone (1) Glyphs. Inner list has a length of numberOfZones[(int)GlyphMode::Compatibility] when set.
     */
    QList<QList<QColor>> *colorValuesPhone1;
    /**
     * @brief Holds the color values for each of the Phone (2) Zones. Inner list has a length of numberOfZones[(int)GlyphMode::Phone2] when set.
     */
    QList<QList<QColor>> *colorValuesPhone2;

    /**
     * @brief Initialize the media player. Does NOT check file type, compatibility or extension - YOLO!
     * @param filepathAudio The path to the audio file.
     */
    void playerInit(const QString &filepathAudio);
    /**
     * @brief Read the light data from the light data file and parse it.
     * @param filepathLightData The path to the audio file.
     *
     * @throws std::invalid_argument Can not open the light data file.
     * @throws CompositionManager::InvalidLightDataContentException Invalid light data in light data file.
     */
    void parseLightData(const QString &filepathLightData);

    /**
     * @brief Get the color where the Glyph is on for a certain percent.
     * @param percent How much percent the Glyph is on. Range: 0 - 1
     * @return The color which reflects the Glyph on state depending on the percent.
     */
    QColor getGlyphColor(const qreal& percent);
};

#endif // COMPOSITIONMANAGER_H
