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

#include "VERSION.h"
#include "glyph_widget.h"
#include "open_composition_dialog.h"
#include "seekbar_style.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QVBoxLayout *mainLayout;
    GlyphWidget *glyphWidget;

    QMenu *fileMenu;
    QAction *openFileAction;
    QMenu *helpMenu;
    QAction *aboutAction;

    OpenCompositionDialog *openCompositionDialog;

    QShortcut *playPauseShortcut;
    QToolButton *playPauseButton;
    QLabel *currentTimeLabel;
    QSlider *seekBar;
    QLabel *lengthTimeLabel;

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
};
#endif // MAINWINDOW_H
