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
                        "QLineEdit,QMenuBar,QPushButton,QComboBox,QMenu {background: rgb(47, 48, 51)}"
                        "QPushButton:hover,QComboBox:hover,QMenuBar::item:selected,QMenu::item:selected {background: rgb(73, 75, 80)}"
                        "QSlider {height: 23px}"
                        "QSlider::sub-page:horizontal {background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(215,25,33), stop:1 rgb(160, 19, 26));border-radius: 5px;margin: 0px 2px}"
                        "QSlider::groove:horizontal {border: 1px solid #5c5c5c;background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0C0D0E, stop:1 #303336);height: 12px;margin: -2px 0px;border-radius: 5px}"
                        "QSlider::handle:horizontal {background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #303336, stop:1 #0C0D0E);border: 1px solid #5c5c5c;width: 15px;margin: -3px 0;border-radius: 8px}"
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

    // Test layout
    QHBoxLayout *playerControllsLayout = new QHBoxLayout(); // No parent needed - layout does this for us
    playerControllsLayout->setSpacing(8);
    playerControllsLayout->setContentsMargins(11, 0, 22, 0);
    this->mainLayout->addLayout(playerControllsLayout);
    ui->centralwidget->layout()->setAlignment(playerControllsLayout, Qt::AlignHCenter);

    this->pausePlayButton = new QToolButton();
    this->pausePlayButton->setIcon(this->style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay));
    this->pausePlayButton->setIconSize(QSize(40, 40));
    this->pausePlayButton->setAutoRaise(true);
    playerControllsLayout->addWidget(this->pausePlayButton);

    this->currentTimeLabel = new QLabel("--:--");
    playerControllsLayout->addWidget(this->currentTimeLabel);

    this->seekBar = new QSlider(Qt::Orientation::Horizontal);
    this->seekBar->setSliderPosition(50);
    this->seekBar->setStyle(new SeekbarStyle(this->seekBar->style()));
//    this->seekBar->setEnabled(false);
//    this->seekBar->setMinimumHeight(20);
    playerControllsLayout->addWidget(this->seekBar);

    this->lengthTimeLabel = new QLabel("--:--");
    playerControllsLayout->addWidget(this->lengthTimeLabel);


    QPushButton *button = new QPushButton("Toggle Phone (1) and Phone (2) Glyphs"); // No parent needed - layout does this for us
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

