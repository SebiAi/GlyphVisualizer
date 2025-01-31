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

#include "OpenCompositionDialog.h"

// Logging
Q_LOGGING_CATEGORY(openCompositionDialog, "OpenCompositionDialog")
Q_LOGGING_CATEGORY(openCompositionDialogVerbose, "OpenCompositionDialog.Verbose")

using namespace OpenCompositionDialogNS;

/*
 * =============================
 *          IBaseWidget
 * =============================
 */

IBaseWidget::IBaseWidget(const OpenCompositionMode& openMode, const QString& comboBoxLabel, const QList<FormData>& formData, QWidget* parent)
    : QWidget{parent}, layout{new QFormLayout(this)}, openMode{openMode}, comboBoxLabel{comboBoxLabel}, formData{formData}
{
    setObjectName(QMetaEnum::fromType<OpenCompositionMode>().valueToKey((int)openMode));
    setLayout(this->layout);

    // Generate the form layout from the formData
    for (qsizetype i = 0; i < this->formData.size(); ++i) {
        QGridLayout* row{new QGridLayout{}};
        row->addWidget(new QLineEdit{}, 0, 0);
        QPushButton* button = new QPushButton{QStringLiteral("Browse...")};
        row->addWidget(button, 0, 1);
        connect(button, &QPushButton::clicked, this, [=](){ onBrowseButtonClicked(i); });
        this->layout->addRow(this->formData.at(i).labelText, row);
    }
}

QMap<qsizetype, QString> IBaseWidget::getValues() const {
    QMap<qsizetype, QString> values;
    for (qsizetype i = 0; i < this->formData.size(); ++i) {
        QLineEdit* lineEdit{
            static_cast<QLineEdit*>(
                static_cast<QGridLayout*>(
                    this->layout->itemAt(1 + 2 * i)->layout() // We know that this is a QGridLayout => cast
                    )->itemAtPosition(0, 0)->widget() // We know that this is a QLineEdit => cast
                )
        };

        values[this->formData.at(i).id] = lineEdit->text();
    }

    return values;
}

void IBaseWidget::setValues(QMap<qsizetype, QString> values) {
    for (const auto& e: values.asKeyValueRange()) {
        // Find the FormData with the matching id
        auto formDataElement = std::find_if(this->formData.cbegin(), this->formData.cend(), [&](const FormData& f){ return f.id == e.first; });
        if (formDataElement == this->formData.cend())
            continue; // FormData not found (iterator is end iterator)
        // Find the index int he formData list - we need it to get the QLineEdit
        qsizetype index{this->formData.indexOf(*formDataElement)};
        if (index == -1)
            continue;

        // Get the QLineEdit and set the new value
        QLineEdit* lineEdit{
            static_cast<QLineEdit*>(
                static_cast<QGridLayout*>(
                    this->layout->itemAt(1 + 2 * index)->layout() // We know that this is a QGridLayout => cast
                    )->itemAtPosition(0, 0)->widget() // We know that this is a QLineEdit => cast
                )
        };
        lineEdit->setText(e.second);
    }
}

QList<QString> IBaseWidget::getFileExtensionsFromFormDataFilter(qsizetype id) const {
    QList<QString> extensions;

    auto fd{std::find_if(this->formData.cbegin(), this->formData.cend(), [&](const FormData& f){ return f.id == id; })};
    if (fd == this->formData.cend())
        return extensions; // FormData not found (iterator is end iterator)
    // Extract the string between the round brackets
    qsizetype start{fd->openFileNameFilter.indexOf('(')+1};
    qsizetype n{fd->openFileNameFilter.lastIndexOf(')') - start};
    // Split by comma because it could contain multiple extensions
    for (const QString& s: fd->openFileNameFilter.sliced(start, n).split(',')) {
        // Trim and append the complete suffix
        QFileInfo info{s.trimmed()};
        extensions.append(info.completeSuffix());
    }

    return extensions;
}

QString IBaseWidget::getValidPath(const QString& path) {
    QFileInfo pathInfo{path};

    if (pathInfo.exists())
    {
        // Path exists (could be file or dir)
        return pathInfo.canonicalFilePath();
    }

    if (!pathInfo.suffix().isEmpty()) {
        // If path is a deleted directory with a dot in its name we also end up here
        // does not matter - how often does this happen, right?
        // Might be a file => see if the parent dir exists
        QDir parentDir{pathInfo.absoluteDir()};
        if (parentDir.exists())
            return parentDir.canonicalPath();
    }

    // Parent dir of file or dir non existent
    return QDir::homePath();
}

void IBaseWidget::onBrowseButtonClicked(qsizetype rowIndex) {
    qCInfo(openCompositionDialogVerbose) << "Browse button clicked in row" << rowIndex;

    QGridLayout* row{static_cast<QGridLayout*>(this->layout->itemAt(1 + 2 * rowIndex)->layout())};
    static_cast<QLineEdit*>(row->itemAtPosition(0, 0)->widget())->text();

    // Get the current QLineEdit
    QLineEdit* lineEdit{
        static_cast<QLineEdit*>(
            static_cast<QGridLayout*>(
                this->layout->itemAt(1 + 2 * rowIndex)->layout() // We know that this is a QGridLayout => cast
                )->itemAtPosition(0, 0)->widget() // We know that this is a QLineEdit => cast
            )
    };

    // Get the current path from the QLineEdit
    QString currentPath{IBaseWidget::getValidPath(lineEdit->text())};

    // Open file dialog
    const FormData& formData{this->formData.at(rowIndex)};
    QString filePath = QFileDialog::getOpenFileName(this, formData.openFileNameCaption, currentPath, formData.openFileNameFilter, nullptr, QFileDialog::Option::ReadOnly);

    if (filePath.isEmpty())
        return; // User canceled file picking

    // Set the new path
    lineEdit->setText(filePath);

    emit fileSelected(this->openMode, filePath, rowIndex);
}

/*
 * =============================
 *     OpenCompositionDialog
 * =============================
 */

OpenCompositionDialog::OpenCompositionDialog(QWidget* parent)
    : QDialog{parent}
{
    initUi();
}

void OpenCompositionDialog::initUi() {
    // Window title and size
    setWindowTitle(QStringLiteral("Open Composition"));
    resize(800, 200);

    // Layout
    setLayout(new QVBoxLayout(this));
    layout()->setAlignment(Qt::AlignmentFlag::AlignTop);

    // Info label
    QLabel* infoLabel{new QLabel{QStringLiteral("Select the type of composition with the dropdown and then select the required files.")}};
    infoLabel->setWordWrap(true);
    layout()->addWidget(infoLabel);

    // Dropdown
    this->comboBoxOpenMode = new QComboBox{};
    layout()->addWidget(this->comboBoxOpenMode);
    connect(this->comboBoxOpenMode, &QComboBox::currentIndexChanged, this, &OpenCompositionDialog::onComboBoxCurrentIndexChanged);

    // Create forms
    this->forms[AudioFormWidget::staticOpenMode] = QSharedPointer<AudioFormWidget>::create();
    // TODO: Implement NGlyph format first
    //this->forms[AudioNGlyphFormWidget::staticOpenMode] = QSharedPointer<AudioNGlyphFormWidget>::create();

    // QStackedWidget for the forms
    this->formContainer = new QStackedWidget{this};
    for (const std::pair<const OpenCompositionMode&, const QSharedPointer<IBaseWidget>&>& e : this->forms.asKeyValueRange()) {
        IBaseWidget* widget{e.second.get()};
        connect(widget, &IBaseWidget::fileSelected, this, &OpenCompositionDialog::onFileSelected);
        this->formContainer->addWidget(widget);
        this->comboBoxOpenMode->addItem(widget->comboBoxLabel, QVariant::fromValue(widget->openMode));
    }
    layout()->addWidget(this->formContainer);

    // Configure QDialogButtonBox
    this->buttonBox = new QDialogButtonBox{QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, Qt::Orientation::Horizontal, this};
    this->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(QStringLiteral("Open"));
    this->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setText(QStringLiteral("Cancel"));
    layout()->addWidget(this->buttonBox);
    connect(this->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(this->buttonBox, &QDialogButtonBox::accepted, this, &OpenCompositionDialog::onButtonBoxAccepted);
}

QString OpenCompositionDialog::autoCompleteFilePath(const QString& sourcePath, const QList<QString>& possibleFileExtensions) {
    if (sourcePath.isEmpty())
        return QString();

    QFileInfo sourcePathInfo{sourcePath};
    for (const QString& extension: possibleFileExtensions) {
        QFileInfo destinationPathInfo{sourcePathInfo.dir().filePath(sourcePathInfo.completeBaseName().append('.').append(extension))};
        if (destinationPathInfo.isFile()) {
            qCInfo(openCompositionDialogVerbose) << "Found auto complete" << destinationPathInfo.filePath() << "for" << sourcePath;
            return destinationPathInfo.filePath();
        }
    }

    return QString();
}

void OpenCompositionDialog::onComboBoxCurrentIndexChanged(int index) {
    OpenCompositionMode openMode{this->comboBoxOpenMode->itemData(index).value<OpenCompositionMode>()};

    qCInfo(openCompositionDialogVerbose) << "ComboBox selection changed to" << openMode;

    switch (openMode) {
    case OpenCompositionMode::AUDIO_ONLY:
        // Copy the audio file path from the AUDIO_AND_NGLYPH form to the AUDIO form
        // TODO: Implement NGlyph format first
        //this->forms[openMode]->setValues(this->forms[OpenCompositionMode::AUDIO_AND_NGLYPH]->getValues());
        break;
    case OpenCompositionMode::AUDIO_AND_NGLYPH:
        // Copy the audio file path from the AUDIO form to the AUDIO_AND_NGLYPH form
        this->forms[openMode]->setValues(this->forms[OpenCompositionMode::AUDIO_ONLY]->getValues());
        // Autocomplete the path - we switched from AUDIO_ONLY so we only need to auto complete the second value (nglyph file)
        QString autocompletedFilePath{OpenCompositionDialog::autoCompleteFilePath(this->forms[openMode]->getValues()[0], this->forms[openMode]->getFileExtensionsFromFormDataFilter(1))};
        if (!autocompletedFilePath.isEmpty())
            this->forms[openMode]->setValues({{1, autocompletedFilePath}});
        break;
    }

    this->formContainer->setCurrentIndex(this->formContainer->indexOf(this->forms[openMode].get()));
}

void OpenCompositionDialog::onFileSelected(OpenCompositionMode mode, const QString& path, qsizetype rowIndex) {
    qCInfo(openCompositionDialogVerbose) << "Selected file" << path << "for" << mode << "in row" << rowIndex;

    // Extra logic based on the open mode
    qsizetype otherRow;
    QString autocompletedFilePath;
    switch (mode) {
    case OpenCompositionMode::AUDIO_AND_NGLYPH:
        otherRow = rowIndex == 0 ? 1 : 0;
        autocompletedFilePath = OpenCompositionDialog::autoCompleteFilePath(path, this->forms[mode]->getFileExtensionsFromFormDataFilter(otherRow));
        if (!autocompletedFilePath.isEmpty())
            this->forms[mode]->setValues({{otherRow, autocompletedFilePath}});
        break;
    default:
        break;
    }
}

void OpenCompositionDialog::onButtonBoxAccepted() {
    // Get the currently selected mode and the values
    OpenCompositionMode openMode{this->comboBoxOpenMode->currentData().value<OpenCompositionMode>()};
    QMap<qsizetype, QString> values = this->forms.value(openMode)->getValues();

    // Strip the values
    QList<QString> strippedValues;
    std::transform(values.cbegin(), values.cend(), std::back_inserter(strippedValues), [](const QString& s){ return s.trimmed(); });

    // Check if any value is empty
    if (std::any_of(strippedValues.cbegin(), strippedValues.cend(), [](const QString& s){ return s.isEmpty(); })) {
        // Display an error message and return
        QMessageBox* msg{new QMessageBox{QMessageBox::Icon::Information, QStringLiteral("Empty fields"), QStringLiteral("Please fill in all the fields or select a different open mode with the dropdown."), QMessageBox::StandardButton::Ok, this}};
        connect(msg, &QDialog::finished, msg, &QObject::deleteLater); // Delete the dialog after it is closed
        msg->open();
        return;
    }

    // Set the result data and close the dialog with accept status
    this->resultData = std::pair<OpenCompositionMode, QList<QString>>{openMode, strippedValues};
    accept();
}


