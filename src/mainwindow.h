#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QSlider>
#include <QTextStream>
#include <QShortcut>
#include <QSettings>
#include <QDateTime>
#include <QShowEvent>

#include "APPLICATION_INFO.h"
#include "glyph_widget.h"
#include "open_composition_dialog.h"
#include "seekbar_style.h"
#include "config.h"
#include "update_checker.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(mainWindow)
Q_DECLARE_LOGGING_CATEGORY(mainWindowVerbose)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showEvent(QShowEvent *event) override;

    /**
     * @brief Indicates that we do not want to enter the Main Event Loop and quit before that.
     */
    bool earlyQuit = false;

private:
    Ui::MainWindow *ui;

    QVBoxLayout *mainLayout;
    GlyphWidget *glyphWidget;

    QMenu *fileMenu;
    QAction *openFileAction;
    QMenu *helpMenu;
    QAction *aboutAction;
    QAction *checkForUpdateAction;

    OpenCompositionDialog *openCompositionDialog;

    QShortcut *playPauseShortcut;
    QToolButton *playPauseButton;
    QLabel *currentTimeLabel;
    QSlider *seekBar;
    QLabel *lengthTimeLabel;

    Config *config = nullptr;

    UpdateChecker *updateChecker;

    /**
     * @brief Loads the composition with the glyphWidget. Will be executed after the OpenCompositionDialog closes with the Accept return value.
     * @param openMode Contains the needed information to call the right loadComposition function.
     * @param str0 The first QString (most likely the path of the Audio file).
     * @param str1 The second QString (most likely light data in some form).
     */
    void processOpenCompositionDialogAccepted(const OpenCompositionDialog::CompositionOpenModeResult& openMode, const QString& str0, const QString& str1);

    /**
     * @brief Converts the milliseconds to a minutes and seconds string.
     * @param milliseconds The milliseconds to convert.
     * @return A string with the form %M:%S (at least two digits for %M and %S).
     */
    QString millisecondsToTimeRepresentation(qint64 milliseconds) const;

public slots:
    /**
     * @brief Will be called by the void QPushButton::clicked(bool checked) signal.
     * @param checked If the action is checked.
     */
    void button_onClicked(bool checked);
    /**
     * @brief Will be called by the void QAction::triggered(bool checked) signal.
     * @param checked If the action is checked.
     */
    void openFileAction_onTriggered(bool checked);
    /**
     * @brief Will be called by the void QAction::triggered(bool checked) signal.
     * @param checked If the action is checked.
     */
    void checkForUpdateAction_onTriggered(bool checked);
    /**
     * @brief Will be called by the void QAction::triggered(bool checked) signal.
     * @param checked If the action is checked.
     */
    void aboutAction_onTriggered(bool checked);
    /**
     * @brief Will be called by the void OpenCompositionDialog::finished(int result) signal.
     * @param result The result code.
     */
    void openCompositionDialog_onFinished(int result);
    /**
     * @brief Will be called by the void QMediaPlayer::durationChanged(qint64 duration) signal.
     * @param duration The duration of the audio in milliseconds.
     */
    void glyphWidget_onDurationChanged(qint64 duration);
    /**
     * @brief Will be called by the void QSlider::valueChanged(int value) signal.
     * @param value The value of the slider.
     */
    void seekBar_onValueChanged(int value);
    /**
     * @brief Will be called by the void QMediaPlayer::positionChanged(qint64 position) signal.
     * @param position The position in milliseconds.
     */
    void glyphWidget_onPositionChanged(qint64 position);
    /**
     * @brief Will be called by the void QSlider::sliderReleased() signal.
     */
    void seekBar_onSliderReleased();
    /**
     * @brief Will be called by the void QMediaPlayer::playbackStateChanged(QMediaPlayer::PlaybackState newState) signal.
     * @param playing If the media is playing.
     */
    void glyphWidget_onPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
    /**
     * @brief Will be called by the void QPushButton::clicked(bool checked) signal.
     * @param checked If the action is checked.
     */
    void pausePlayButton_onClicked(bool checked);
    /**
     * @brief Will be called by the void UpdateChecker::updateAvailable(const QString &newVersion) signal.
     * @param newVersion The new version.
     */
    void updateChecker_onUpdateAvailable(const QString &newVersion);
    /**
     * @brief Will be called by the void UpdateChecker::updateCheckFailed(const QString &errorMessage) signal.
     * @param errorMessage The error message.
     */
    void updateChecker_onUpdateCheckFailed(const QString &errorMessage);
    /**
     * @brief Will be called by the void UpdateChecker::noUpdateAvailable() signal.
     */
    void updateChecker_noUpdateAvailable();
};
#endif // MAINWINDOW_H
