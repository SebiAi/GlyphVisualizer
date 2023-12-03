#include "mainwindow.h"
#include "./ui_mainwindow.h"

// TODO: [END] Leak check with Valgrind: 'valgrind --leak-check=full ./GlyphVisualizer'

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Add style sheets
    // TODO: [END] Test style sheet in white and black mode
    this->setStyleSheet("* {background: rgb(30, 32, 34); color: rgb(240, 242, 242)} QLineEdit,QMenuBar,QPushButton,QComboBox {background: rgb(47, 48, 51)} QPushButton:hover,QComboBox:hover,QMenuBar::item:selected,QMenuBar::item::drop-down {background: rgb(73, 75, 80)}");

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
    mainLayout->setSpacing(20);
    ui->centralwidget->setLayout(mainLayout);

    // Add GlyphWidget
    glyphWidget = new GlyphWidget(); // No parent needed - layout does this for us
    ui->centralwidget->layout()->addWidget(glyphWidget);

    // Test layout
    QLabel *label = new QLabel("Hello World!"); // No parent needed - layout does this for us
    ui->centralwidget->layout()->addWidget(label);
    ui->centralwidget->layout()->setAlignment(label, Qt::AlignHCenter);

    QPushButton *button = new QPushButton("Toggle Phone (1) and Phone (2) Glyphs"); // No parent needed - layout does this for us
    connect(button, SIGNAL(clicked(bool)), this, SLOT(button_onClicked(bool)));
    ui->centralwidget->layout()->addWidget(button);
    ui->centralwidget->layout()->setAlignment(button, Qt::AlignHCenter);
}

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

MainWindow::~MainWindow()
{
    delete this->ui;

    delete this->mainLayout;
    delete this->glyphWidget;

    delete this->openFileAction;
    delete this->fileMenu;

    delete this->openCompositionDialog;
}

