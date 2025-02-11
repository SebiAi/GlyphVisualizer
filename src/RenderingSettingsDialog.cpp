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

#include "RenderingSettingsDialog.h"

// Logging
Q_LOGGING_CATEGORY(renderingSettingsDialog, "RenderingSettingsDialog")
Q_LOGGING_CATEGORY(renderingSettingsDialogVerbose, "RenderingSettingsDialog.Verbose")

RenderingSettingsDialog::RenderingSettingsDialog(QWidget* parent)
    : QDialog{parent},
    renderer{new CompositionRenderer{this}}, progressDialog{nullptr}
{
    initUi();

    // Blocking terminate when exiting this dialog
    connect(this, &QDialog::finished, this->renderer, &CompositionRenderer::terminate);

    connect(this->renderer, &CompositionRenderer::renderingAborted, this, &RenderingSettingsDialog::onRendererAborted);
    connect(this->renderer, &CompositionRenderer::renderingFinished, this, &RenderingSettingsDialog::onRendererFinished);
    connect(this->renderer, &CompositionRenderer::ffmpegErrorOccurred, this, &RenderingSettingsDialog::onRendererFFmpegError);
    connect(this->renderer, &CompositionRenderer::unexpectedErrorOccurred, this, &RenderingSettingsDialog::onRendererUnexpectedError);
}
RenderingSettingsDialog::~RenderingSettingsDialog() {
    delete this->progressDialog;
}

void RenderingSettingsDialog::open(const QString& audioPath, IConfiguration* config) {
    // Save values
    this->audioPath = audioPath;
    this->config = config;

    // Determine the output file name from the file name of the audioPath
    QString outputFilePath{QFileInfo{this->audioPath}.baseName().append(QStringLiteral(".mp4"))};

    // Determine output path
    QStringList movieLocations{QStandardPaths::standardLocations(QStandardPaths::StandardLocation::MoviesLocation)};
    if (movieLocations.size()) {
        outputFilePath = QDir{movieLocations.at(0)}.absoluteFilePath(outputFilePath);
    }

    this->filePathLineEdit->setText(outputFilePath);
    qCInfo(renderingSettingsDialogVerbose) << "Determined file path to be" << outputFilePath;

    // Call original open method
    QDialog::open();
}

void RenderingSettingsDialog::showEvent(QShowEvent* event) {
    Q_UNUSED(event);

    if (this->ffmpegPathLineEdit->text().trimmed().isEmpty()) {
        // If FFmpeg can not be found we get an empty string - just like this was never called.
        this->ffmpegPathLineEdit->setText(getExecutableFromPath(QStringLiteral("ffmpeg")));
        qCInfo(renderingSettingsDialog) << "Auto detected ffmpeg path on show:" << this->ffmpegPathLineEdit->text();
    }
}

void RenderingSettingsDialog::initUi() {
    // Window title and size
    setWindowTitle(QStringLiteral("Render Composition"));

    // Layout
    this->layout = new QVBoxLayout{this};
    this->layout->setAlignment(Qt::AlignmentFlag::AlignTop);
    this->layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
    setLayout(this->layout);

    // Info label
    QLabel* infoLabel{new QLabel{QStringLiteral(
        "Configure the settings and press the \"Render\" button at the bottom to render "
        "the currently loaded composition to a video file."
    )}};
    // infoLabel->setWordWrap(true);
    this->layout->addWidget(infoLabel);

    // Create form layout for all the rows
    QFormLayout* formLayout{new QFormLayout{}};
    this->layout->addLayout(formLayout);

    // Add file path row
    QGridLayout* filePathRow{new QGridLayout{}};
    this->filePathLineEdit = new QLineEdit{QStringLiteral("composition.mp4")};
    this->filePathBrowseButton = new QPushButton{QStringLiteral("Browse...")};
    filePathRow->addWidget(this->filePathLineEdit, 0, 0);
    filePathRow->addWidget(this->filePathBrowseButton, 0, 1);
    formLayout->addRow(QStringLiteral("File path:"), filePathRow);
    connect(this->filePathBrowseButton, &QPushButton::clicked, this, &RenderingSettingsDialog::onFilePathBrowseButtonClicked);

    // Add resolution dropdown
    this->resolutionDropdown = new QComboBox{};
    this->resolutionDropdown->addItem(QStringLiteral("UHD 4k/2160p (2160x3840)"), QSize{2160, 3840});
    this->resolutionDropdown->addItem(QStringLiteral("WQHD 1440p (1440x2560)"), QSize{1440, 2560});
    this->resolutionDropdown->addItem(QStringLiteral("FHD 1080p (1080x1920)"), QSize{1080, 1920});
    this->resolutionDropdown->addItem(QStringLiteral("HD 720p (720x1280)"), QSize{720, 1280});
    this->resolutionDropdown->addItem(QStringLiteral("NTSC 480p (480x720)"), QSize{480, 720});
    this->resolutionDropdown->setCurrentIndex(2); // Select 1080p per default
    formLayout->addRow(QStringLiteral("Resolution:"), this->resolutionDropdown);

    // Add background color picker
    QGridLayout* backgroundColorRow{new QGridLayout{}};
    this->backgroundColorButton = new QToolButton{};
    this->backgroundColorLabel = new QLabel{};
    updatebackgroundColorUI(Qt::GlobalColor::black);
    backgroundColorRow->addWidget(this->backgroundColorButton, 0, 0);
    backgroundColorRow->addWidget(this->backgroundColorLabel, 0, 1);
    formLayout->addRow(QStringLiteral("Background color:"), backgroundColorRow);
    connect(this->backgroundColorButton, &QToolButton::clicked, this, &RenderingSettingsDialog::onBackgroundColorButtonClicked);

    // Add FFmpeg path row
    QGridLayout* ffmpegPathRow{new QGridLayout{}};
    this->ffmpegPathLineEdit = new QLineEdit{};
    this->ffmpegPathAutoDetectButton = new QPushButton{QStringLiteral("Auto Detect")};
    this->ffmpegPathBrowseButton = new QPushButton{QStringLiteral("Browse...")};
    ffmpegPathRow->addWidget(this->ffmpegPathLineEdit, 0, 0);
    ffmpegPathRow->addWidget(this->ffmpegPathAutoDetectButton, 0, 1);
    ffmpegPathRow->addWidget(this->ffmpegPathBrowseButton, 0, 2);
    formLayout->addRow(QStringLiteral("FFmpeg path:"), ffmpegPathRow);
    connect(this->ffmpegPathAutoDetectButton, &QPushButton::clicked, this, &RenderingSettingsDialog::onFFmpegPathAutoDetectButton);
    connect(this->ffmpegPathBrowseButton, &QPushButton::clicked, this, &RenderingSettingsDialog::onFFmpegPathBrowseButtonClicked);

    // Add spacing before the dialog buttons
    this->layout->addSpacing(5);
    // Dialog buttons
    this->buttonBox = new QDialogButtonBox{QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Close, Qt::Orientation::Horizontal};
    this->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(QStringLiteral("&Render"));
    this->buttonBox->button(QDialogButtonBox::StandardButton::Close)->setText(QStringLiteral("&Close"));
    this->layout->addWidget(this->buttonBox);
    connect(this->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(this->buttonBox, &QDialogButtonBox::accepted, this, &RenderingSettingsDialog::onButtonBoxAccepted);
}

void RenderingSettingsDialog::updatebackgroundColorUI(const QColor& color) {
    if (!color.isValid())
        return;

    qCInfo(renderingSettingsDialog) << "Updating background color to" << color;

    // Render pixmap with color as the new icon for the button
    QPixmap colorIcon{16, 16};
    colorIcon.fill(color);
    this->backgroundColorButton->setIcon(colorIcon); // Gets converted to an QIcon

    // Set the label
    this->backgroundColorLabel->setText(color.name());
}

void RenderingSettingsDialog::afterRenderCleanup() {
    // This function must only be called after the rendering is finished/crashed/etc.
    // because only then is progressDialog guaranteed to be valid.
    this->progressDialog->reset();
    this->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);

    qApp->alert(this->parentWidget()); // Alert the user that the rendering is done
}

void RenderingSettingsDialog::onFilePathBrowseButtonClicked() {
    QString currentPath{getValidPath(this->filePathLineEdit->text().trimmed())};
    QString filePath{QFileDialog::getSaveFileName(this, QStringLiteral("Select File Path"), currentPath, QStringLiteral("Videos (*.mp4)"), nullptr).trimmed()};

    if (filePath.isEmpty())
        return; // User canceled file picking

    // Make sure we have the extension
    if (!filePath.endsWith(QStringLiteral(".mp4")))
        filePath.append(".mp4");

    qCInfo(renderingSettingsDialog) << "Selected output file path" << filePath;

    this->filePathLineEdit->setText(filePath);
}
void RenderingSettingsDialog::onBackgroundColorButtonClicked() {
    // Open the color picker dialog
    updatebackgroundColorUI(QColorDialog::getColor(QColor{this->backgroundColorLabel->text()}, this, QStringLiteral("Select Background Color")));
}
void RenderingSettingsDialog::onFFmpegPathAutoDetectButton() {
    qCInfo(renderingSettingsDialogVerbose) << "Auto Detect Button clicked...";

    QString ffmpegPath{getExecutableFromPath(QStringLiteral("ffmpeg"))};
    if (ffmpegPath.isEmpty()) {
        // Did not find ffmpeg
        qCInfo(renderingSettingsDialog) << "Did not find FFmpeg";
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("Missing FFmpeg"), QStringLiteral("Could not automatically find FFmpeg in PATH.\nPlease download and install ffmpeg or enter the path to ffmpeg manually."), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
    } else {
        // Found ffmpeg
        qCInfo(renderingSettingsDialog) << "Found ffmpeg:" << ffmpegPath;
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Information, QStringLiteral("Found FFmpeg"), QStringLiteral("Using FFmpeg from '%1'").arg(ffmpegPath), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
        this->ffmpegPathLineEdit->setText(ffmpegPath);
    }
}
void RenderingSettingsDialog::onFFmpegPathBrowseButtonClicked() {
    QString currentPath{getValidPath(this->ffmpegPathLineEdit->text().trimmed())};
    QString filePath{QFileDialog::getOpenFileName(this, QStringLiteral("Select FFmpeg executable"), currentPath, QStringLiteral("Executable (*)"), nullptr, QFileDialog::ReadOnly).trimmed()};

    if (filePath.isEmpty())
        return; // User canceled file picking

    // We check if the ffmpeg executable is valid after the user clicks on the render button

    qCInfo(renderingSettingsDialog) << "User selected ffmpeg" << filePath;

    this->ffmpegPathLineEdit->setText(filePath);
}
void RenderingSettingsDialog::onButtonBoxAccepted() {
    // Get all values
    QString filePath{this->filePathLineEdit->text().trimmed()};
    QSize resolution{this->resolutionDropdown->currentData().toSize()};
    QColor backgroundColor{this->backgroundColorLabel->text()};
    QString ffmpegPath{this->ffmpegPathLineEdit->text().trimmed()};

    // Do checks on the values
    if (filePath.isEmpty() || ffmpegPath.isEmpty()) {
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("Invalid values"), QStringLiteral("There is at least one field empty.\nPlease fill out all the fields."), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
        return;
    }
    if (!filePath.endsWith(QStringLiteral(".mp4"))) {
        filePath.append(".mp4");
        this->filePathLineEdit->setText(filePath);
    }
    QFileInfo fileFileInfo{filePath};
    if (!fileFileInfo.absoluteDir().exists()) {
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("File Path Invalid"), QStringLiteral("The provided file path directory structure does not exist!\nPlease choose another save location."), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
        return;
    }
    QFileInfo ffmpegFileInfo{ffmpegPath};
    if (!ffmpegFileInfo.isFile() || !ffmpegFileInfo.isExecutable()) {
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("FFmpeg Executable Missing"), QStringLiteral("The provided path for FFmpeg is not a file or not executable!"), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
        return;
    }

    // Disable render button after all checks passed
    this->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

    // Set up progressDialog
    if (this->progressDialog) delete this->progressDialog;
    this->progressDialog = new QProgressDialog{QStringLiteral("Rendering Composition..."), QStringLiteral("Cancel"), 0, 100, this};
    connect(progressDialog, &QProgressDialog::canceled, this->renderer, &CompositionRenderer::abort);
    connect(this->renderer, &CompositionRenderer::progressChanged, progressDialog, &QProgressDialog::setValue);

    progressDialog->setMinimumDuration(0);
    progressDialog->setAutoReset(false);
    progressDialog->setLabelText(QStringLiteral("Rendering..."));
    progressDialog->setValue(0); // Causes the dialog to open because of setMinimumDuration(0)

    try {
        // Start render - no need to catch anything (except for missing audio file) because we confirmed the validity above
        this->renderer->render(this->audioPath, this->config, filePath, resolution, backgroundColor, ffmpegPath);
    } catch (const SourceFileException& e) {
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("Starting Render Failed"), QStringLiteral("Starting the renderer failed for the following reason:\n%1\n\nTry reopening the composition and try again.").arg(e.what()), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();

        this->progressDialog->reset();
        this->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);
        return;
    }
}

void RenderingSettingsDialog::onRendererAborted() {
    afterRenderCleanup();

    QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Information, QStringLiteral("Rendering Aborted"), QStringLiteral("Rendering aborted successfully!"), QMessageBox::StandardButton::Ok, this}};
    connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
    msg->open();
}
void RenderingSettingsDialog::onRendererFinished() {
    afterRenderCleanup();

    QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Information, QStringLiteral("Rendering Finished"), QStringLiteral("Rendering finished successfully!"), QMessageBox::StandardButton::Ok, this}};
    connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
    msg->open();
}
void RenderingSettingsDialog::onRendererFFmpegError(const QString& error) {
    afterRenderCleanup();

    QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Warning, QStringLiteral("Rendering Error"), QStringLiteral("An error with FFmpeg during the rendering process occurred:\n%1\n\nThe rendering could not be finished.").arg(error), QMessageBox::StandardButton::Ok, this}};
    connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
    msg->open();
}
void RenderingSettingsDialog::onRendererUnexpectedError(const QString& error) {
    afterRenderCleanup();

    QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Critical, QStringLiteral("Unknown Rendering Error"), QStringLiteral("An unknown error during the rendering process occurred:\n%1\n\nThe rendering could not be finished.").arg(error), QMessageBox::StandardButton::Ok, this}};
    connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
    msg->open();
}
