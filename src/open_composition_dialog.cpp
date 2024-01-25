#include "open_composition_dialog.h"

// Logging
Q_LOGGING_CATEGORY(openCompositionDialog, "OpenCompositionDialog")
Q_LOGGING_CATEGORY(openCompositionDialogVerbose, "OpenCompositionDialog.Verbose")

const QList<OpenCompositionDialog::CompositionOpenMode> OpenCompositionDialog::openModes = {
    CompositionOpenMode("Audio file (ogg)", "Audio file:", QString(), "Select Audio (ogg)", "", "*.ogg", "", false, false),
    CompositionOpenMode("Audio file (ogg) + Light data file (glypha)", "Audio file:", "Light data file:", "Select Audio (ogg)", "Select Light data (glypha)", "*.ogg", "*.glypha", true, true),
    CompositionOpenMode("Audio file (ogg) + Audacity Labels file (txt)", "Audio file:", "Audacity Labels file:", "Select Audio (ogg)", "Select Audacity Labels (txt)", "*.ogg", "*.txt", true, false)
};

OpenCompositionDialog::OpenCompositionDialog(QWidget *parent)
    : QDialog{parent, Qt::Dialog}
{
    // Configure dialog
    this->resize(800, 200);
    this->setWindowTitle("Open Composition");

    // Configure layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);

    // Add label
    QLabel *headingLabel = new QLabel("Select the type of composition with the dropdown and then select the required files.");
    headingLabel->setWordWrap(true);
    layout->addWidget(headingLabel);

    // Configure ComboBox stage 1 (Add to layout so we have the right order + register currentIndexChanged signal)
    this->comboBox_openMode = new QComboBox();
    layout->addWidget(this->comboBox_openMode);
    connect(this->comboBox_openMode, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_openMode_onCurrentIndexChanged(int)));

    // Create GridLayouts
    QGridLayout *row0GridLayout = new QGridLayout();
    this->row0Container[1] = new QLineEdit();
    row0GridLayout->addWidget(this->row0Container[1], 0, 0);
    this->row0Container[2] = new QPushButton("Browse...");
    row0GridLayout->addWidget(this->row0Container[2], 0, 1);
    connect(this->row0Container[2], SIGNAL(clicked(bool)), this, SLOT(browseRow0FileButton_onClicked(bool)));

    QGridLayout *row1GridLayout = new QGridLayout();
    this->row1Container[1] = new QLineEdit();
    row1GridLayout->addWidget(this->row1Container[1], 0, 0);
    this->row1Container[2] = new QPushButton("Browse...");
    row1GridLayout->addWidget(this->row1Container[2], 0, 1);
    connect(this->row1Container[2], SIGNAL(clicked(bool)), this, SLOT(browseRow1FileButton_onClicked(bool)));

    // Add form layout
    QFormLayout *formLayout = new QFormLayout();
    this->row0Container[0] = new QLabel();
    formLayout->addRow(this->row0Container[0], row0GridLayout);
    this->row1Container[0] = new QLabel();
    formLayout->addRow(this->row1Container[0], row1GridLayout);
    layout->addLayout(formLayout);

    // Configure buttonBox
    this->buttonBox = new QDialogButtonBox(QDialogButtonBox::Open | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    this->buttonBox->button(QDialogButtonBox::Open)->setAutoDefault(true);
    this->buttonBox->button(QDialogButtonBox::Open)->setDefault(true);
    this->buttonBox->button(QDialogButtonBox::Open)->setFocus();
    this->buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
    this->buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);
    layout->addWidget(this->buttonBox);
    connect(this->buttonBox, SIGNAL(accepted()), this, SLOT(buttonBox_onAccepted()));
    connect(this->buttonBox, SIGNAL(rejected()), this, SLOT(buttonBox_onRejected()));

    // Configure ComboBox stage 2 (We need to first initialize the form so the triggered currentIndexChanged function does not have nullptr objects)
    this->comboBox_openMode->addItems(CompositionOpenMode::getTexts(this->openModes)); // Add the items => triggers currentIndexChanged
    bool foundDefault = false;
    for (int i = 0; i < this->openModes.size(); i++) // Disable items if the configuration says so
    {
        if (!this->openModes.at(i).isEnabled)
        {
            // Disable item
            setComboBoxItemEnabled(this->comboBox_openMode, i, false);
        }
        else if (!foundDefault)
        {
            // Found one item that is not disabled => set as selected => triggers currentIndexChanged
            this->comboBox_openMode->setCurrentIndex(i);
            foundDefault = true;
        }
    }
    Q_ASSERT(foundDefault); // There must be at least one option which is enabled
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void OpenCompositionDialog::setComboBoxItemEnabled(const QComboBox* comboBox, int index, bool enabled)
{
    // Cast to the QStandardItemModel (see https://doc.qt.io/qt-6/qcombobox.html#model-view-framework)
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->comboBox_openMode->model());
    Q_ASSERT(model);

    // Get the item
    QStandardItem *item = model->item(index);
    Q_ASSERT(item);

    // Set the flag
    item->setEnabled(enabled);
}

/**
 * @brief [Helper Function] Enable or disable a widget row in the QFormLayout.
 * @param enabled true: Enable the widgets; false: Disable the widgets.
 * @param widgets An widget array with size 3
 */
void setRowEnabled(bool enabled, QWidget* widgets[3])
{
    for (int i = 0; i < 3; i++)
    {
        widgets[i]->setEnabled(enabled);
    }
}
void OpenCompositionDialog::setRow0Enabled(bool enabled)
{
    setRowEnabled(enabled, this->row0Container);
}
void OpenCompositionDialog::setRow1Enabled(bool enabled)
{
    setRowEnabled(enabled, this->row1Container);
}

/*
 * ==================================
 *               Slots
 * ==================================
 */

void OpenCompositionDialog::buttonBox_onAccepted()
{
    qCInfo(openCompositionDialogVerbose) << "Pressed the accept button";

    // Get the QLineEdits
    QLineEdit *row0LineEdit = qobject_cast<QLineEdit*>(this->row0Container[1]);
    Q_ASSERT(row0LineEdit);
    QLineEdit *row1LineEdit = qobject_cast<QLineEdit*>(this->row1Container[1]);
    Q_ASSERT(row1LineEdit);

    // Check if the required QEditLines are not empty
    if (row0LineEdit->text().trimmed().isEmpty() ||
        (
            this->openModes.at(this->comboBox_openMode->currentIndex()).needsRow1 &&
            row1LineEdit->text().trimmed().isEmpty())
        )
    {
        // Display error dialog and return
        qCWarning(openCompositionDialog) << "Not all the required fields are filled out";
        QMessageBox* msg = new QMessageBox(QMessageBox::Icon::Information, "Empty fields", "Please fill out all the required fields.", QMessageBox::StandardButton::Ok, this->window());
        connect(msg, &QDialog::finished, msg, &QWidget::deleteLater); // Delete the dialog after it finished
        msg->open();
        return;
    }

    // Set the result values
    this->row0Result = row0LineEdit->text();
    this->row1Result = row1LineEdit->text();
    const int currentIndex = this->comboBox_openMode->currentIndex();
    Q_ASSERT(currentIndex < this->openModes.count());
    this->openModeResult = (CompositionOpenModeResult)currentIndex;

    // Close Dialog with accept status
    this->accept();
}

void OpenCompositionDialog::buttonBox_onRejected()
{
    qCInfo(openCompositionDialogVerbose) << "Pressed the reject button";

    // Close Dialog with reject status
    this->reject();
}

void OpenCompositionDialog::comboBox_openMode_onCurrentIndexChanged(int index)
{
    Q_ASSERT(index < this->openModes.count());
    qCInfo(openCompositionDialogVerbose).noquote() << "Current index of the ComboBox changed to" << (CompositionOpenModeResult)index << QString("(%1)").arg(index);

    // Disable all
    if (index == -1)
    {
        qCInfo(openCompositionDialogVerbose) << "ComboBox is empty => disable accept button and rows";
        this->buttonBox->button(QDialogButtonBox::Open)->setEnabled(false);
        setRow0Enabled(false);
        setRow1Enabled(false);
        return;
    }

    // Get label texts
    const QString &labelRow0 = this->openModes.at(index).labelText[0];
    const QString &labelRow1 = this->openModes.at(index).labelText[1];

    // Change row 0 label
    QLabel *label0 = qobject_cast<QLabel*>(this->row0Container[0]);
    Q_ASSERT(label0);
    label0->setText(labelRow0.isEmpty() ? "-" : labelRow0);

    // Change row 1 label
    QLabel *label1 = qobject_cast<QLabel*>(this->row1Container[0]);
    Q_ASSERT(label1);
    label1->setText(labelRow1.isEmpty() ? "-" : labelRow1);

    // Enable rows as needed
    this->buttonBox->button(QDialogButtonBox::Open)->setEnabled(true);
    setRow0Enabled(true);
    setRow1Enabled(this->openModes.at(index).needsRow1);
}

/**
 * @brief [Helper Function] Try to autocomplete the given QLineEdit by combining the filter with the source path.
 * @param filePathSource From which path it should autocomplete.
 * @param fileDialogFilterDestination The filter of the destination. Should contain an extension which will be added to the basename of filePathSource. E.g.: "*.txt"
 * @param lineEditDestination Where to write the autocompleted path to.
 * @return An empty string if the filePathSource is empty, if the autocompleted file does not exist. The path to the autocompleted file if it exists.
 */
QString autoCompleteHelper(const QString& filePathSource, const QString& fileDialogFilterDestination, QWidget *lineEditDestination)
{
    // Get the QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(lineEditDestination);
    Q_ASSERT(lineEdit);

    // Check if filePathSource is empty
    if (filePathSource.isEmpty())
    {
        qCInfo(openCompositionDialogVerbose) << "Autocomplete - Row0 is empty => nothing to autocomplete";
        return QString(); // Return empty string
    }

    // Try to autocomplete row1
    QFileInfo fileInfoSource = QFileInfo(filePathSource);
    QFileInfo fileInfoDestination = QFileInfo(fileInfoSource.dir().filePath(fileInfoSource.completeBaseName().append('.').append(QFileInfo(fileDialogFilterDestination).suffix())));
    if (!fileInfoDestination.exists() || !fileInfoDestination.isFile())
    {
        qCInfo(openCompositionDialogVerbose) << "Autocomplete - Autocompleted file" << fileInfoDestination.filePath() << "does not exist => nothing to autocomplete";
        return QString(); // Return empty string
    }

    // Found a matching file
    qCInfo(openCompositionDialogVerbose) << "Autocomplete - Found autocompleted file:" << fileInfoDestination.filePath();
    lineEdit->setText(fileInfoDestination.filePath());

    return fileInfoDestination.filePath();
}
/**
 * @brief [Helper Function] Open the FileOpen Dialog and let the user choose a file. (Bit messy but what ever)
 * @param lineEdit Where to insert the fileName. Can be nullptr to skip setting the text of the QLineEdit.
 * @param mode The current CompositionOpenMode.
 * @param index The index of the mode.fileDialogCaption and mode.fileDialogFilter. Corresponds to the row
 */
QString browseButtonHelper(QWidget *lineEdit, const OpenCompositionDialog::CompositionOpenMode& mode, int rowIndex)
{
    // Get the file
    QString file = QFileDialog::getOpenFileName(nullptr, mode.fileDialogCaption[rowIndex], QString(), mode.fileDialogFilter[rowIndex], nullptr, QFileDialog::Option::ReadOnly);

    // Return if no file was selected
    if (file.isEmpty())
    {
        qCInfo(openCompositionDialogVerbose) << "Browse button - no file selected";
        return file;
    }

    qCInfo(openCompositionDialogVerbose) << "Browse button - selected file:" << file;

    // Set the text of QLineEdit
    if (lineEdit)
    {
        QLineEdit *lineEditCAST = qobject_cast<QLineEdit*>(lineEdit);
        Q_ASSERT(lineEditCAST);
        lineEditCAST->setText(file);
    }

    return file;
}
void OpenCompositionDialog::browseRow0FileButton_onClicked(bool checked)
{
    // Call the helper function
    const CompositionOpenMode& mode = openModes.at(this->comboBox_openMode->currentIndex());
    QString filePath = browseButtonHelper(this->row0Container[1], mode, 0);

    // Check if we should autocomplete
    if (!mode.needsRow1)
        return;

    // Try to autocomplete
    autoCompleteHelper(filePath, mode.fileDialogFilter[1], this->row1Container[1]);
}
void OpenCompositionDialog::browseRow1FileButton_onClicked(bool checked)
{
    // Call the helper function
    const CompositionOpenMode& mode = openModes.at(this->comboBox_openMode->currentIndex());
    QString filePath = browseButtonHelper(this->row1Container[1], mode, 1);

    // Try to autocomplete
    autoCompleteHelper(filePath, mode.fileDialogFilter[0], this->row0Container[1]);
}

/*
 * ==================================
 *           Deconstructor
 * ==================================
 */

OpenCompositionDialog::~OpenCompositionDialog(){
    // Qt deletes for us as long as it is a QObject and is linked through parents to this.
}
