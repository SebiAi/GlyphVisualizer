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

#include "MainWindow.h"

// Logging
Q_LOGGING_CATEGORY(mainWindow, "MainWindow")
Q_LOGGING_CATEGORY(mainWindowVerbose, "MainWindow.Verbose")

MainWindow::MainWindow(Config* config, QWidget *parent)
	: QMainWindow(parent), config{config}, updateChecker{new UpdateChecker{this}}, firstShow{true}, configurationManager{}, compositonManager{this}
{
    initUi();

	// Init update checker
	connect(this->updateChecker, &UpdateChecker::updateAvailable, this, &MainWindow::onUpdateCheckerUpdateAvailable);
	connect(this->updateChecker, &UpdateChecker::updateCheckFailed, this, &MainWindow::onUpdateCheckerUpdateCheckFailed);
	connect(this->updateChecker, &UpdateChecker::noUpdateAvailable, this, &MainWindow::onUpdateCheckerNoUpdateAvailable);

    // Init composition manager
	connect(&this->compositonManager, &CompositionManager::compositionTick, this, &MainWindow::onCompositionManagerTick);
	connect(&this->compositonManager, &CompositionManager::mediaStatusChanged, this, &MainWindow::onCompositionManagerMediaStatusChanged);
}

MainWindow::~MainWindow() {
	qCInfo(mainWindow) << "Bye, bye!";
}

void MainWindow::showEvent(QShowEvent *event) {
	if (!this->firstShow)
		return;
	this->firstShow = false;

	// Automatic update check
	if (this->config->getBool(Config::Setting::UpdateChecker_AutoUpdateCheckEnabled_Bool)) {
		// Auto update check is enabled in config
		QDateTime currentTime{QDateTime::currentDateTimeUtc()};
		if (currentTime > this->config->getQDateTime(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime).addSecs(6 * 60 * 60)) {
			// Update config
			this->config->setValue(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, currentTime);

			// Check for update
			this->updateChecker->checkForUpdate(QStringLiteral(BUILDINFO_VERSION), true);
		} else {
			qCInfo(mainWindow) << "Skipping automatic update check, last check was to recent";
		}
	}

	// Call the original showEvent
	QMainWindow::showEvent(event);

	if (!this->config->getBool(Config::Setting::FirstStart_Bool)) {
		bool doNotShowAgain{this->config->getBool(Config::Setting::DonationDialog_DoNotShowAgain_Bool)};
		QDateTime lastShown{this->config->getQDateTime(Config::Setting::DonationDialog_LastShown_QDateTime)};
		// Display if not already shown in the threshold (from 15th of November onwards)
		QDateTime winterShowLowerThreshold{QDate{QDate::currentDate().year(), 11, 15}, QTime{0, 0}};
		bool showWinter{lastShown < winterShowLowerThreshold && QDateTime::currentDateTimeUtc() >= winterShowLowerThreshold};
		if (!doNotShowAgain || showWinter) {
			// We want the dialog to pop up just after the main window was shown
			// => single shot timer with 150 to trigger on next event loop
			// We need 150 because other wise the dialog is not centered on the parent (this)
			QTimer::singleShot(150, this, [=](){
				if (showWinter)
					qCInfo(mainWindow) << "Showing Donation dialog (winter)";
				else
					qCInfo(mainWindow) << "Showing Donation dialog";

				DonationDialog* donationDialog{new DonationDialog{this, this->config, !(showWinter && doNotShowAgain)}};
				connect(donationDialog, &QDialog::finished, donationDialog, &QObject::deleteLater);
				donationDialog->open();
			});
		} else {
			qCInfo(mainWindow) << "Not showing Donation dialog (DoNotShowAgain)";
		}
	} else {
		qCInfo(mainWindow) << "Showing Donation dialog on next start";
	}

}

void MainWindow::initUi() {
    // Window
    setObjectName(QStringLiteral("MainWindow"));
    resize(800, 600);
    setWindowTitle(QCoreApplication::applicationName());
	// See here: https://doc.qt.io/qt-6/stylesheet-reference.html
	setStyleSheet(
		QStringLiteral(
			"* { background: #1e2022; color: #f0f2f2 }" // Background and off white acent color
			"QLineEdit,QMenuBar,QPushButton,QComboBox,QMenu,QSlider::groove:horizontal,QStatusBar { background: #2f3033 }" // Lighter color on background
			"QPushButton:hover,QComboBox:hover,QMenuBar::item:selected,QMenu::item:selected { background: #494b50 }" // Even lighter color for hover effects
		)
	);

    // QMenuBar
    this->menuBar = new QMenuBar(this);
    this->menuBar->setObjectName(QStringLiteral("MenuBar"));
	this->fileMenu = this->menuBar->addMenu("&File");
	this->helpMenu = this->menuBar->addMenu("&Help");
    setMenuBar(this->menuBar);

	// Add "Open File" action to the file menu
	this->openFileAction = new QAction(style()->standardIcon(QStyle::StandardPixmap::SP_DirOpenIcon), QStringLiteral("&Open File"), this->fileMenu);
	this->openFileAction->setShortcut(QKeySequence("Ctrl+O"));
	this->fileMenu->addAction(this->openFileAction);
	connect(this->openFileAction, &QAction::triggered, this, &MainWindow::onOpenFileActionTriggered);
    this->openCompositionDialog = new OpenCompositionDialog{this};
    connect(this->openCompositionDialog, &OpenCompositionDialog::finished, this, &MainWindow::onOpenCompositionDialogFinished);

	// Add "Check for Update..." action to the help menu
	this->checkForUpdateAction = new QAction(QStringLiteral("Check for &Update..."), this->helpMenu);
	this->helpMenu->addAction(this->checkForUpdateAction);
	connect(this->checkForUpdateAction, &QAction::triggered, this, &MainWindow::onCheckForUpdateActionTriggered);
	// Add "Donate" action to the help menu
	this->donateAction = new QAction{QStringLiteral("&Donate"), this->helpMenu};
	this->helpMenu->addAction(this->donateAction);
	connect(this->donateAction, &QAction::triggered, this, &MainWindow::onDonateActionTriggered);
	// Add "About" action to the help menu
	this->aboutAction = new QAction(QStringLiteral("&About %1").arg(QCoreApplication::applicationName()), this->helpMenu);
	this->helpMenu->addAction(this->aboutAction);
	connect(this->aboutAction, &QAction::triggered, this, &MainWindow::onAboutActionTriggered);

    // QStatusBar
    this->statusBar = new QStatusBar(this);
    this->statusBar->setObjectName(QStringLiteral("StatusBar"));
    this->statusBar->setSizeGripEnabled(false);
    setStatusBar(this->statusBar);

    // Central QWidget with vertical layout
    this->centralWidget = new QWidget(this);
    this->centralWidget->setObjectName(QStringLiteral("CentralWidget"));
    this->centralLayout = new QVBoxLayout(this->centralWidget);
    this->centralLayout->setContentsMargins(10, 20, 10, 15);
    this->centralLayout->setSpacing(20);
    this->centralWidget->setLayout(this->centralLayout);
    setCentralWidget(this->centralWidget);

    // GlyphWidget
	this->glyphWidget = new GlyphWidget(this->configurationManager.getConfiguration(DeviceBuild::Spacewar));
    this->centralLayout->addWidget(this->glyphWidget);

	// Player controls
	this->compositonManagerControls = new CompositionManagerControls(&this->compositonManager);
	this->compositonManagerControls->setEnabled(false);
	this->centralLayout->addWidget(this->compositonManagerControls);
}

void MainWindow::openCompositionDialogAccepted() {
    // Reset the UI before loading the new composition - if the loading fails we are still in a valid state
    onCompositionManagerMediaStatusChanged(QMediaPlayer::MediaStatus::NoMedia);

    const std::pair<OpenCompositionMode, QList<QString>>& resultData{this->openCompositionDialog->getResultData()};
	qCInfo(mainWindowVerbose).nospace() << "OpenCompositionDialog accepted with openMode: " << resultData.first
										<< ", " << resultData.second;
    try {
        DeviceBuild build;
        switch (resultData.first) {
        case OpenCompositionMode::AUDIO_ONLY:
            // Load the composition
            build = configurationManager.loadCompositionFromAudio(resultData.second.at(0));
            this->glyphWidget->setConfiguration(configurationManager.getConfiguration(build));
            this->compositonManager.loadAudio(resultData.second.at(0));

            // Play
            this->compositonManager.play();
            break;
        case OpenCompositionMode::AUDIO_AND_NGLYPH:
            // Load the composition
            build = configurationManager.loadCompositionFromNglyph(resultData.second.at(1));
            this->glyphWidget->setConfiguration(configurationManager.getConfiguration(build));
            this->compositonManager.loadAudio(resultData.second.at(0));

            // Play
            this->compositonManager.play();
            break;
        }
    } catch (const SourceFileException& e) {
		qCWarning(mainWindow) << "SourceFileException:" << e.what();

        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Critical, QStringLiteral("Composition file error"), QStringLiteral("Error loading the composition: %1").arg(e.what()), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, [&](){ this->openFileAction->trigger(); }); // Reopen the dialog
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
    } catch (const InvalidLightDataException& e) {
		qCWarning(mainWindow) << "InvalidLightDataException:" << e.what();

        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Critical, QStringLiteral("Composition light data error"), QStringLiteral("Error loading the composition: %1").arg(e.what()), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, [&](){ this->openFileAction->trigger(); }); // Reopen the dialog
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
    }
}

void MainWindow::onUpdateCheckerUpdateAvailable(const QString& newVersion) {
	QString newVersionMessage{
		QStringLiteral(
			"## %1 now available!\n"
			"***\n"
			"Heyo, just chipping in to tell you that a new version is available!\n"
			"\n"
			"**You can download it here:** [**Download**](%2)"
		)
		.arg(newVersion)
		.arg(BUILDINFO_RELEASE_URL.arg(newVersion))
	};

	// Display info dialog
	QMessageBox* msg{new QMessageBox(QMessageBox::Icon::NoIcon, QStringLiteral("New version available!"), newVersionMessage, QMessageBox::StandardButton::Ok, this)};
	msg->setTextFormat(Qt::TextFormat::MarkdownText);
	connect(msg, &QDialog::finished, msg, &QWidget::deleteLater); // Delete the dialog after it finished
	msg->open();
}
void MainWindow::onUpdateCheckerUpdateCheckFailed(const QString& errorMessage) {
	QString errorDialogMessage{
		QStringLiteral(
			"**An error occurred during the update check:**\n"
			"\n"
			"*%1*\n"
			"\n"
		)
		.arg(errorMessage)
	};

	// Display error dialog
	QMessageBox* msg{new QMessageBox(QMessageBox::Icon::Warning, QStringLiteral("Error checking for updates"), errorDialogMessage, QMessageBox::StandardButton::Ok, this)};
	msg->setTextFormat(Qt::TextFormat::MarkdownText);
	connect(msg, &QDialog::finished, msg, &QWidget::deleteLater); // Delete the dialog after it finished
	msg->open();
}
void MainWindow::onUpdateCheckerNoUpdateAvailable() {
	QString noUpdateMessage{
		QStringLiteral(
			"## No update available\n"
			"\n"
			"**You are on the latest version of this software!**"
		)
	};

	// Display info dialog
	QMessageBox* msg{new QMessageBox(QMessageBox::Icon::Information, QStringLiteral("Check for Updates"), noUpdateMessage, QMessageBox::StandardButton::Ok, this)};
	msg->setTextFormat(Qt::TextFormat::MarkdownText);
	connect(msg, &QDialog::finished, msg, &QWidget::deleteLater); // Delete the dialog after it finished
	msg->open();
}

void MainWindow::onCompositionManagerTick(qint64 position) {
    qint64 index{(qint64)(position / CompositionManager::tickIntervalMS)};
    this->glyphWidget->render((qsizetype)index);
}

void MainWindow::onCompositionManagerMediaStatusChanged(QMediaPlayer::MediaStatus status) {
	switch (status) {
	case QMediaPlayer::MediaStatus::EndOfMedia:
		// Set position to 0
		this->compositonManager.stop();
		// Render start of the light sequence
		this->glyphWidget->render(0);
		break;
	case QMediaPlayer::MediaStatus::NoMedia:
	case QMediaPlayer::MediaStatus::InvalidMedia:
		// Disable controls
		this->compositonManagerControls->setEnabled(false);
        // Reset the controls
        this->compositonManagerControls->resetControls();
        // Render an invalid index to force render the fallback color
        this->glyphWidget->render(-1);
		break;
	case QMediaPlayer::MediaStatus::LoadedMedia:
		// Enable controls
		this->compositonManagerControls->setEnabled(true);
		break;
	default:
		break;
	}
}

void MainWindow::onOpenFileActionTriggered() {
	qCInfo(mainWindowVerbose) << "Opening the OpenCompositionDialog";

    // Save the player state so we can restore it after closing the dialog
    this->compositionWasPlaying = this->compositonManager.isPlaying();
    this->compositonManager.pause();

    this->openCompositionDialog->open();
}
void MainWindow::onCheckForUpdateActionTriggered() {
	qCInfo(mainWindowVerbose) << "Manual update check triggered";

	// Update config
	this->config->setValue(Config::Setting::UpdateChecker_LastAutoUpdateCheck_QDateTime, QDateTime::currentDateTimeUtc());

	// Check for update
	this->updateChecker->checkForUpdate(QStringLiteral(BUILDINFO_VERSION));
}
void MainWindow::onDonateActionTriggered() {
	qCInfo(mainWindowVerbose) << "Donate action triggered";

	DonationDialog* donationDialog{new DonationDialog{this, this->config, false}};
	connect(donationDialog, &QDialog::finished, donationDialog, &QObject::deleteLater);
	donationDialog->open();
}
void MainWindow::onAboutActionTriggered() {
	qCInfo(mainWindowVerbose) << "Opening the about dialog";

    QString aboutMessage{
        QStringLiteral(
            "# %1\n"
            "A Glyph composition player written with the Qt6 framework in C++ that plays Glyph compositions from Nothing phones.\n"
            "***\n"
            "Version: *%2*\n"
            "\n"
            "Commit Hash: [*%3*](%4)\n"
            "***\n"
            "Creator: *Sebastian Aigner aka. SebiAi*\n"
            "\n"
            "GitHub: [*%5*](%6)\n"
            "***\n"
            "Libraries used:\n"
            "- *Qt6* (%7)\n"
            "- *TagLib* (%8.%9.%10)"
        )
        .arg(QCoreApplication::applicationName())
        .arg(QCoreApplication::applicationVersion())
		.arg(BUILDINFO_GIT_COMMIT_HASH_SHORT)
        .arg(BUILDINFO_GITHUB_COMMIT_URL)
        .arg(BUILDINFO_GITHUB_REPO_URL)
        .arg(BUILDINFO_GITHUB_REPO_URL)
        .arg(QT_VERSION_STR)
        .arg(TAGLIB_MAJOR_VERSION).arg(TAGLIB_MINOR_VERSION).arg(TAGLIB_PATCH_VERSION)
    };

    QMessageBox* msg{new QMessageBox{QMessageBox::Icon::NoIcon, QStringLiteral("About %1").arg(QCoreApplication::applicationName()), aboutMessage, QMessageBox::StandardButton::Ok, this}};
    msg->setTextFormat(Qt::TextFormat::MarkdownText);
    connect(msg, &QDialog::finished, msg, &QObject::deleteLater);
    msg->open();
}

void MainWindow::onOpenCompositionDialogFinished(int result) {
	qCInfo(mainWindowVerbose) << "OpenCompositionDialog finished with result code:" << result;

    switch (result) {
    case QDialog::DialogCode::Accepted:
        openCompositionDialogAccepted();
        break;
    case QDialog::DialogCode::Rejected:
        // Restore the players state before opening the dialog
        if (this->compositionWasPlaying)
            this->compositonManager.play();
        break;
    }
}
