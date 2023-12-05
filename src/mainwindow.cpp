#include "mainwindow.h"
#include "./ui_mainwindow.h"

// TODO: [END] Leak check with Valgrind: 'valgrind --leak-check=full ./GlyphVisualizer'
// TODO: Set the application icon: https://doc.qt.io/qt-6/appicon.html

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add style sheets (see here: https://doc.qt.io/qt-6/stylesheet-reference.html)
    // TODO: [END] Test style sheet in white and black mode
    this->setStyleSheet("* {background: rgb(30, 32, 34); color: rgb(240, 242, 242)}"
                        "QLineEdit,QMenuBar,QPushButton,QComboBox,QMenu,QSlider::groove:horizontal {background: rgb(47, 48, 51)}"
                        "QPushButton:hover,QComboBox:hover,QMenuBar::item:selected,QMenu::item:selected {background: rgb(73, 75, 80)}"
                        "QSlider {height: 23px}"
                        "QSlider::sub-page:horizontal {background: rgb(215,25,33);border-radius: 5px;margin: 0px 2px}"
                        "QSlider::groove:horizontal {border: 1px solid rgb(73, 75, 80);height: 12px;margin: -2px 0px;border-radius: 5px}"
                        "QSlider::handle:horizontal {background: transparent}"
                        );

    // Add 'File' entry to MenuBar
    this->fileMenu = ui->menubar->addMenu("&File");

    // Add 'Open File' action to the fileMenu
    this->openFileAction = new QAction(this->style()->standardIcon(QStyle::SP_DirOpenIcon), "&Open Composition", fileMenu);
    openFileAction->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(openFileAction);
    connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFileAction_onTriggered(bool)));

    // Create OpenCompositionDialog
    this->openCompositionDialog = new OpenCompositionDialog(this);
    connect(this->openCompositionDialog, SIGNAL(finished(int)), this, SLOT(openCompositionDialog_onFinished(int)));

    // Set window title
    this->setWindowTitle("GlyphVisualizer");

    // Set vertical layout
    mainLayout = new QVBoxLayout(ui->centralwidget);
    mainLayout->setSpacing(4);
    ui->centralwidget->setLayout(mainLayout);

    // Add GlyphWidget
    glyphWidget = new GlyphWidget(); // No parent needed - layout does this for us
    ui->centralwidget->layout()->addWidget(glyphWidget);
    connect(this->glyphWidget->compositionManager->player, SIGNAL(positionChanged(qint64)), this, SLOT(glyphWidget_onPositionChanged(qint64)));
    connect(this->glyphWidget->compositionManager->player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)), this, SLOT(glyphWidget_onPlaybackStateChanged(QMediaPlayer::PlaybackState)));

    // Add layout for the player controls
    QHBoxLayout *playerControlsLayout = new QHBoxLayout();
    playerControlsLayout->setSpacing(8);
    playerControlsLayout->setContentsMargins(11, 0, 22, 0);
    this->mainLayout->addLayout(playerControlsLayout);
    ui->centralwidget->layout()->setAlignment(playerControlsLayout, Qt::AlignHCenter);

    // Play button
    this->playPauseButton = new QToolButton();
    this->playPauseButton->setIcon(this->style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay));
    this->playPauseButton->setIconSize(QSize(40, 40));
    this->playPauseButton->setAutoRaise(true);
    playerControlsLayout->addWidget(this->playPauseButton);
    connect(this->playPauseButton, SIGNAL(clicked(bool)), this, SLOT(pausePlayButton_onClicked(bool)));
    // Create a shortcut for the play/pause button
    playPauseShortcut = new QShortcut(QKeySequence(" "), this);
    connect(this->playPauseShortcut, SIGNAL(activated()), this->playPauseButton, SLOT(click()));

    // Current time label
    this->currentTimeLabel = new QLabel("--:--");
    playerControlsLayout->addWidget(this->currentTimeLabel);

    // Seek bar
    this->seekBar = new QSlider(Qt::Orientation::Horizontal);
    this->seekBar->setStyle(new SeekBarStyle(this->seekBar->style())); // Needed so the bar moves exactly to where we click with the mouse
    this->seekBar->setEnabled(false);
    this->seekBar->setMinimum(0);
    this->seekBar->setSingleStep(1000);
    this->seekBar->setPageStep(5000);
    // TODO: Set transparent QSlider::groove:horizontal {background: transparent} until the value is big enough to not clip so weird. Or set the border-radius to something smaller.
    playerControlsLayout->addWidget(this->seekBar);
    connect(this->seekBar, SIGNAL(valueChanged(int)), this, SLOT(seekBar_onValueChanged(int)));
    connect(this->seekBar, SIGNAL(sliderReleased()), this, SLOT(seekBar_onSliderReleased()));

    // Length time label
    this->lengthTimeLabel = new QLabel("--:--");
    playerControlsLayout->addWidget(this->lengthTimeLabel);
    connect(this->glyphWidget->compositionManager->player, SIGNAL(durationChanged(qint64)), this, SLOT(glyphWidget_onDurationChanged(qint64)));

    // Toggle button
    QPushButton *button = new QPushButton("Toggle Phone (1) and Phone (2) Glyphs");
    connect(button, SIGNAL(clicked(bool)), this, SLOT(button_onClicked(bool)));
    ui->centralwidget->layout()->addWidget(button);
    ui->centralwidget->layout()->setAlignment(button, Qt::AlignHCenter);
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void MainWindow::processOpenCompositionDialogAccepted(const OpenCompositionDialog::CompositionOpenModeResult& openMode, const QString& str0, const QString& str1)
{
    // Load the composition
    try
    {
        // Load the composition with the right function
        switch (openMode)
        {
        case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_ONLY:
            throw std::logic_error(std::string("[Development Error] Audio only not implemented yet: '").append(__FUNCTION__).append("'!"));
            break;
        case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_LIGHT_DATA:
            this->glyphWidget->compositionManager->loadComposition(str0, str1);
            break;
        case OpenCompositionDialog::CompositionOpenModeResult::AUDIO_AUDACITY_LABELS:
            throw std::logic_error(std::string("[Development Error] Audio only not implemented yet: '").append(__FUNCTION__).append("'!"));
            break;
        case OpenCompositionDialog::CompositionOpenModeResult::None:
            throw std::logic_error(std::string("[Development Error] Ups... This should not happen. Please report this error to the developer. '").append(__FUNCTION__).append("'!"));
            break;
        default:
            // This should never happen
            throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
            return;
        }
    }
    catch (const std::invalid_argument& e)
    {
        // Display the error
        QMessageBox msg(QMessageBox::Critical, "Critical Error", e.what(), QMessageBox::StandardButton::Ok, this->window());
        msg.exec();

        // Reopen the OpenCompositionDialog
        emit this->openFileAction->triggered();
        return;
    }
    catch (const CompositionManager::InvalidLightDataContentException& e)
    {
        QMessageBox msg(QMessageBox::Critical, "Critical Error", e.what(), QMessageBox::StandardButton::Ok, this->window());
        msg.exec();

        return;
    }

    // TODO: Maybe move this into the GlyphWidget class?
    // Set the proper visual depending on the loaded light data
    if (this->glyphWidget->compositionManager->getGlyphMode() == CompositionManager::GlyphMode::Phone2 && this->glyphWidget->getVisualMode() == GlyphWidget::Visual::Phone1)
        this->glyphWidget->setVisual(GlyphWidget::Visual::Phone2);

    // Play the composition
    this->glyphWidget->compositionManager->player->setPosition(0);
    this->glyphWidget->compositionManager->player->play();
}

QString MainWindow::millisecondsToTimeRepresentation(qint64 milliseconds) const
{
    quint64 minutes = (quint64)(milliseconds / 1000 / 60);
    quint64 seconds = (quint64)(milliseconds / 1000 - minutes * 60);
    QString result;
    QTextStream textStream(&result);
    textStream.setPadChar('0');
    textStream << qSetFieldWidth(2) << minutes << qSetFieldWidth(0) << ':'<< qSetFieldWidth(2) << seconds;
    return result;
}

/*
 * ==================================
 *               Slots
 * ==================================
 */

void MainWindow::button_onClicked(bool checked)
{
    switch (glyphWidget->getVisualMode())
    {
    case GlyphWidget::Visual::Phone1:
        glyphWidget->setVisual(GlyphWidget::Visual::Phone2);
        break;
    case GlyphWidget::Visual::Phone2:
        // Only switch to Phone (1) visual if the currently loaded light data is compatible with it.
        if (glyphWidget->compositionManager->getGlyphMode() != CompositionManager::GlyphMode::Phone2)
            glyphWidget->setVisual(GlyphWidget::Visual::Phone1);
        break;
    }
}

void MainWindow::openFileAction_onTriggered(bool checked)
{
    // Pause the player
    this->glyphWidget->compositionManager->player->pause();

    // Open the OpenCompositionDialog
    this->openCompositionDialog->open();
}

void MainWindow::openCompositionDialog_onFinished(int result)
{
    // Respond to result code
    switch (result)
    {
    case QDialog::DialogCode::Accepted: // == 0
        processOpenCompositionDialogAccepted(this->openCompositionDialog->openModeResult, this->openCompositionDialog->row0Result, this->openCompositionDialog->row1Result);
        break;
    case QDialog::DialogCode::Rejected: // == 1
        // Do nothing
        break;
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch case not accounted for in function '").append(__FUNCTION__).append("'!"));
        break;
    }
}

void MainWindow::glyphWidget_onDurationChanged(qint64 duration)
{
    // Set the lengthTimeLabel
    this->lengthTimeLabel->setText(millisecondsToTimeRepresentation(duration));

    // Set seekBar
    this->seekBar->setEnabled(true);
    this->seekBar->setMaximum(duration);
}

void MainWindow::seekBar_onValueChanged(int value)
{
    // Update currentTimeLabel
    this->currentTimeLabel->setText(millisecondsToTimeRepresentation(value));

    // Set player position if we have a position missmatch (the user moved the slider)
    if (value != this->glyphWidget->compositionManager->player->position())
        this->glyphWidget->compositionManager->player->setPosition(this->seekBar->value());
}

void MainWindow::glyphWidget_onPositionChanged(qint64 position)
{
    // Change the seekBar position when the slider is not beeing held down
    if (!this->seekBar->isSliderDown())
        this->seekBar->setValue(position);
}

void MainWindow::seekBar_onSliderReleased()
{
    // Set the player position after the slider is released
    this->glyphWidget->compositionManager->player->setPosition(this->seekBar->value());
}

void MainWindow::glyphWidget_onPlaybackStateChanged(QMediaPlayer::PlaybackState newState)
{
    // Set the icon depending on the playback state
    switch (newState)
    {
    case QMediaPlayer::PlaybackState::PlayingState:
        this->playPauseButton->setIcon(this->style()->standardIcon(QStyle::StandardPixmap::SP_MediaPause));
        break;
    case QMediaPlayer::PlaybackState::StoppedState:
        // Stop the player to reset the position to 0
        this->glyphWidget->compositionManager->player->stop();
    case QMediaPlayer::PlaybackState::PausedState:
        this->playPauseButton->setIcon(this->style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay));
        break;
    }
}

void MainWindow::pausePlayButton_onClicked(bool checked)
{
    // Change playback state
    switch (this->glyphWidget->compositionManager->player->playbackState())
    {
    case QMediaPlayer::PlaybackState::PlayingState:
        this->glyphWidget->compositionManager->player->pause();
        break;
    case QMediaPlayer::PlaybackState::StoppedState:
    case QMediaPlayer::PlaybackState::PausedState:
        this->glyphWidget->compositionManager->player->play();
        break;
    }
}

/*
 * ==================================
 *           Deconstructor
 * ==================================
 */

MainWindow::~MainWindow()
{
    delete this->ui;

    delete this->mainLayout;
    delete this->glyphWidget;

    delete this->openFileAction;
    delete this->fileMenu;

    delete this->openCompositionDialog;
}

