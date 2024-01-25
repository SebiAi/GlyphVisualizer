#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Logging
Q_LOGGING_CATEGORY(mainWindow, "MainWindow")
Q_LOGGING_CATEGORY(mainWindowVerbose, "MainWindow.Verbose")
// TODO: Actually implement proper loggin in MainWindow

// TODO: Set the application icon: https://doc.qt.io/qt-6/appicon.html

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup UI
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

    // Add entrys to MenuBar
    this->fileMenu = ui->menubar->addMenu("&File");
    this->helpMenu = ui->menubar->addMenu("&Help");

    // Add 'Open File' action to the fileMenu
    this->openFileAction = new QAction(this->style()->standardIcon(QStyle::SP_DirOpenIcon), "&Open Composition", this->fileMenu);
    this->openFileAction->setShortcut(QKeySequence("Ctrl+O"));
    this->fileMenu->addAction(this->openFileAction);
    connect(this->openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFileAction_onTriggered(bool)));

    // Add 'Check for Updates...' action to helpMenu
    this->checkForUpdateAction = new QAction("Check for Updates...", this->helpMenu);
    this->helpMenu->addAction(this->checkForUpdateAction);
    connect(this->checkForUpdateAction, SIGNAL(triggered(bool)), this, SLOT(checkForUpdateAction_onTriggered(bool)));
    // Add 'About GlyphVisualizer' action to helpMenu
    this->aboutAction = new QAction("About GlyphVisualizer", this->helpMenu);
    this->helpMenu->addAction(this->aboutAction);
    connect(this->aboutAction, SIGNAL(triggered(bool)), this, SLOT(aboutAction_onTriggered(bool)));

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
    this->seekBarStyle = new SeekBarStyle(this->seekBar->style());
    this->seekBar->setStyle(this->seekBarStyle); // Needed so the bar moves exactly to where we click with the mouse
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

void MainWindow::showEvent(QShowEvent *event)
{
    // Init config
    bool clearConfig = false;
    do
    {
        try
        {
            if (this->config)
                delete this->config;

            this->config = new Config("GlyphVisualizer.ini", this, clearConfig);
            clearConfig = false;
        }
        catch (Config::ConfigVersionTooHighError e)
        {
            qCInfo(mainWindowVerbose) << "ConfigVersionTooHighError occurred";
            QMessageBox mb = QMessageBox(QMessageBox::Icon::Critical,"Config Error",
                                         QString("The configuration file in '").append(e.fileName)
                                             .append("' is intended to be used with a higher version of this software.\n\nThis usually happens when you either downgrade your software or manually modify the config.\n\nDo you want to clear the config (Recommended) or exit the application?")
                                         );
            mb.setParent(this);
            mb.addButton("Exit Software", QMessageBox::ButtonRole::AcceptRole);
            mb.addButton("Clear Config (Recommended)", QMessageBox::ButtonRole::DestructiveRole);
            int answer = mb.exec();

            switch (answer)
            {
            case QDialog::DialogCode::Accepted: // Clear
                qCInfo(mainWindowVerbose) << "ConfigVersionTooHighError - User decided to exit";
                clearConfig = true;
                break;
            case QDialog::DialogCode::Rejected: // Exit
                qCInfo(mainWindowVerbose) << "ConfigVersionTooHighError - User decided to exit";
                this->earlyQuit = true;
                return;
                break;
            default:
                // This should never happen
                throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
            }
        }
    } while (clearConfig);

    // Init update checker
    this->updateChecker = new UpdateChecker(this);
    connect(this->updateChecker, &UpdateChecker::updateAvailable, this, &MainWindow::updateChecker_onUpdateAvailable);
    connect(this->updateChecker, &UpdateChecker::updateCheckFailed, this, &MainWindow::updateChecker_onUpdateCheckFailed);
    connect(this->updateChecker, &UpdateChecker::noUpdateAvailable, this, &MainWindow::updateChecker_noUpdateAvailable);

    // Check for update
    if (this->config->getBool(Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool))
    {
        QDateTime currentTime = QDateTime::currentDateTimeUtc();
        if (currentTime > this->config->getQDateTime(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime).addSecs(6 * 60 * 60))
        {
            // Set the currentTime
            this->config->setValue(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, currentTime);

            // Check for update
            updateChecker->checkForUpdate(APPLICATION_VERSION, true);
        }
        else
        {
            qCInfo(mainWindow) << "Skipping automatic update check, last check was too recent";
        }
    }

    // Execute original function
    QWidget::showEvent(event);
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
        QMessageBox* msg = new QMessageBox(QMessageBox::Critical, "Critical Error", e.what(), QMessageBox::StandardButton::Ok, this->window());
        connect(msg, &QMessageBox::finished, msg, [=](){
            // Reopen the OpenCompositionDialog
            emit this->openFileAction->triggered();
        });
        msg->open();

        return;
    }
    catch (const CompositionManager::InvalidLightDataContentException& e)
    {
        QMessageBox* msg = new QMessageBox(QMessageBox::Critical, "Critical Error", e.what(), QMessageBox::StandardButton::Ok, this->window());
        msg->open();

        return;
    }

    // TODO: Maybe move this into the GlyphWidget class?
    // Set the proper visual depending on the loaded light data
    if (this->glyphWidget->compositionManager->getGlyphMode() == CompositionManager::GlyphMode::Phone2 && this->glyphWidget->getVisualMode() == CompositionManager::PhoneModel::Phone1)
        this->glyphWidget->setVisual(CompositionManager::PhoneModel::Phone2);

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
    case CompositionManager::PhoneModel::Phone1:
        glyphWidget->setVisual(CompositionManager::PhoneModel::Phone2);
        break;
    case CompositionManager::PhoneModel::Phone2:
        // Only switch to Phone (1) visual if the currently loaded light data is compatible with it.
        if (glyphWidget->compositionManager->getGlyphMode() != CompositionManager::GlyphMode::Phone2)
            glyphWidget->setVisual(CompositionManager::PhoneModel::Phone1);
        break;
    case CompositionManager::PhoneModel::None:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] Phone model None in function '").append(__FUNCTION__).append("' - can't be None!"));
    default:
        // This should never happen
        throw std::logic_error(std::string("[Development Error] switch in function '").append(__FUNCTION__).append("' not updated!"));
        return;
    }
}

void MainWindow::openFileAction_onTriggered(bool checked)
{
    // Pause the player
    this->glyphWidget->compositionManager->player->pause();

    // Open the OpenCompositionDialog
    this->openCompositionDialog->open();
}

void MainWindow::checkForUpdateAction_onTriggered(bool checked)
{
    // Set the currentTime
    this->config->setValue(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, QDateTime::currentDateTimeUtc());

    // Check for updates
    this->updateChecker->checkForUpdate(APPLICATION_VERSION);
}

void MainWindow::aboutAction_onTriggered(bool checked)
{
    // Display an about dialog
    QMessageBox* mb = new QMessageBox(QMessageBox::Icon::NoIcon, "About GlyphVisualizer",
                                 QString("# GlyphVisualizer\n**A Glyph composition player written with the Qt6 framework in C++ that plays Glyph compositions from Nothing Phones.**\n***\nVersion: *").append(APPLICATION_VERSION)
                                     .append("*\n\nCommit Hash: ").append(QString("[*%1*](%2)").arg(APPLICATION_GIT_COMMIT_HASH, APPLICATION_GITHUB_COMMIT_URL))
                                     .append("\n***\nCreator: *Sebastian Aigner aka. SebiAi*\n\nGitHub: ").append(QString("[*%1*](%2)").arg(APPLICATION_GITHUB_REPO_URL, APPLICATION_GITHUB_REPO_URL)),
                                 QMessageBox::StandardButton::Ok, this);
    mb->setTextFormat(Qt::TextFormat::MarkdownText);
    mb->open();
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
        // Resume playing the composition
        this->glyphWidget->compositionManager->player->play();
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

void MainWindow::updateChecker_onUpdateAvailable(const QString &newVersion)
{
    // Display info dialog
    QMessageBox* mb = new QMessageBox(QMessageBox::Icon::NoIcon, "New version available!",
                                 QString("## ").append(newVersion).append(" now available!")
                                     .append("\n***\nHeyo, just chipping in to tell you that a new version is available!")
                                     .append("\n\n**You can download it here:** [**Download**]").append(QString("(%1)").arg(APPLICATION_RELEASE_URL.arg(newVersion))),
                                 QMessageBox::StandardButton::Ok, this);
    mb->setTextFormat(Qt::TextFormat::MarkdownText);
    mb->open();
}

void MainWindow::updateChecker_onUpdateCheckFailed(const QString &errorMessage)
{
    // Display error dialog
    QMessageBox* mb = new QMessageBox(QMessageBox::Icon::Warning,
                                 "Error checking for updates",
                                 QString("**An error occurred during the update check:**\n\n*%1*\n\n").arg(errorMessage),
                                 QMessageBox::StandardButton::Ok, this);
    mb->setTextFormat(Qt::TextFormat::MarkdownText);
    mb->open();
}

void MainWindow::updateChecker_noUpdateAvailable()
{
    // Display info dialog
    QMessageBox* mb = new QMessageBox(QMessageBox::Icon::Information,
                                 "Check for Updates",
                                 QString("## No update available\n\n**You are on the latest version of this software!**"),
                                 QMessageBox::StandardButton::Ok, this);
    mb->setTextFormat(Qt::TextFormat::MarkdownText);
    mb->open();
}

/*
 * ==================================
 *           Deconstructor
 * ==================================
 */

MainWindow::~MainWindow()
{
    delete this->ui;

    //delete this->seekBarStyle; // TODO: This delete causes invalid reads??? See valgrind memcheck.
}

