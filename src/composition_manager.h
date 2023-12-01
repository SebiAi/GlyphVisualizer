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
#include "helper.h"

// TODO: [Next] Use QTimer to create a signal at 15ms and call GlyphWidget::update() with that
// TODO: To fix the low "refresh" issue: Make per Glyph glyphdata + spawn a thread for each glyph which waits and then changes the glyph/start a timer for each glyph

// TODO: [END] Try libvlc.so instead of the QMediaPlayer. The QMediaPlayer does not trigger the positionChanged signal only triggers every 40-60ms but we need 16ms or lower.

// The maximal value a light can be. Is used to calculate the opacity percent
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
     */
    explicit CompositionManager();
    /**
     * @brief Load a Composition with a seperate ogg and light data file.
     * @param filepathAudio Filepath to the audio ogg file.
     * @param filepathLightData Filepath to the light data file.
     *
     * @throws std::invalid_argument Audio file does not exist.
     * @throws std::invalid_argument Light data file does not exist.
     * @throws std::invalid_argument Can not open the light data file.
     * @throws CompositionManager::InvalidLightDataContentException Invalid light data in light data file.
     */
    explicit CompositionManager(const QString &filepathAudio, const QString &filepathLightData);
    ~CompositionManager();

    /**
     * @brief Get the current Glyph Mode depending on the currently loaded
     * @return See enum GlyphMode.
     */
    const GlyphMode& getGlyphMode() const { return currentGlyphMode; }
    /**
     * @brief Get the calculated opacity values for each of the Phone (1) Glyphs for the position.
     * @param position The audio time in milliseconds.
     * @return A list with numberOfZones[(int)GlyphMode::Compatibility] elements, each corresponding to one Zone on Phone (1).
     */
    const QList<qreal>* const getPhone1OpacityValues(qint64 position) const;
    /**
     * @brief Get the calculated opacity values for each of the Phone (2) Glyphs for the position.
     * @param position The audio time in milliseconds.
     * @return A list with numberOfZones[(int)GlyphMode::Phone2] elements, each corresponding to one Zone on Phone (2).
     */
    const QList<qreal>* const getPhone2OpacityValues(qint64 position) const;


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
     * @brief This function contains the initialisation code. It will be called by all constructors.
     */
    inline void init();

    /**
     * @brief Holds the default off values for Phone (1) - will be used by getPhone1OpacityValues. Has a size of numberOfZones[(int)GlyphMode::Compatibility].
     */
    QList<qreal> *opacityOffValuesPhone1;
    /**
     * @brief Holds the default off values for Phone (2) - will be used by getPhone2OpacityValues. Has a size of numberOfZones[(int)GlyphMode::Phone2].
     */
    QList<qreal> *opacityOffValuesPhone2;

    /**
     * @brief Audio output.
     */
    QAudioOutput *audioOutput;

    /**
     * @brief Holds the current glyph mode depending on the currently loaded light data.
     */
    GlyphMode currentGlyphMode = GlyphMode::None;

    /**
     * @brief Holds the opacity values for each of the Phone (1) Glyphs. Inner list has a length of numberOfZones[(int)GlyphMode::Compatibility] when set.
     */
    QList<QList<qreal>> *opacityValuesPhone1;
    /**
     * @brief Holds the opacity values for each of the Phone (2) Zones. Inner list has a length of numberOfZones[(int)GlyphMode::Phone2] when set.
     */
    QList<QList<qreal>> *opacityValuesPhone2;

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
};

#endif // COMPOSITIONMANAGER_H
