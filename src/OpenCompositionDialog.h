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

#ifndef GV_OPENCOMPOSITIONDIALOG_H
#define GV_OPENCOMPOSITIONDIALOG_H

#include <algorithm>

#include <QDialogButtonBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QMetaEnum>
#include <QPushButton>
#include <QSharedPointer>
#include <QStackedWidget>
#include <QString>
#include <QStringLiteral>
#include <QVBoxLayout>
#include <QWidget>

#include "Utils.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(openCompositionDialog)
Q_DECLARE_LOGGING_CATEGORY(openCompositionDialogVerbose)

// This whole thing is a mess... Don't look at it
// Needs work but it works

namespace OpenCompositionDialogNS {
    Q_NAMESPACE

    enum class OpenCompositionMode {
        AUDIO_ONLY = 0,
        AUDIO_AND_NGLYPH = 1
    };
    Q_ENUM_NS(OpenCompositionMode);


    class FormData {
    public:
        const qsizetype id;
        const QString labelText;
        const QString openFileNameCaption;
        const QString openFileNameFilter;

        FormData(qsizetype id , const QString& labelText, const QString& openFileNameCaption, const QString& openFileNameFilter)
            : id{id}, labelText{labelText}, openFileNameCaption{openFileNameCaption}, openFileNameFilter{openFileNameFilter}
        {}

        bool operator==(const FormData& other) const {
            return this->id == other.id;
        }
    };


    class IBaseWidget : public QWidget {
        Q_OBJECT
    public:
        const OpenCompositionMode openMode;
        const QString comboBoxLabel;

        explicit IBaseWidget(const OpenCompositionMode& openMode, const QString& comboBoxLabel, const QList<FormData>& formData, QWidget* parent = nullptr);

        QMap<qsizetype, QString> getValues() const;
        void setValues(QMap<qsizetype, QString> values);
        QList<QString> getFileExtensionsFromFormDataFilter(qsizetype id) const;

    signals:
        void fileSelected(OpenCompositionMode mode, QString path, qsizetype rowIndex);

    private:
        QFormLayout* layout;

        const QList<FormData> formData;

    private slots:
        void onBrowseButtonClicked(qsizetype rowIndex);
    };
    class AudioFormWidget : public IBaseWidget {
    public:
        static constexpr OpenCompositionMode staticOpenMode = OpenCompositionMode::AUDIO_ONLY;

        explicit AudioFormWidget(QWidget* parent = nullptr): IBaseWidget{
                  AudioFormWidget::staticOpenMode,
                  QStringLiteral("Audio file (ogg)"),
                  QList<FormData>{
                      FormData{0, QStringLiteral("Audio file:"), QStringLiteral("Select Audio (ogg)"), QStringLiteral("Compositions (*.ogg)")}
                  },
                  parent
              } {}
    };
    class AudioNGlyphFormWidget : public IBaseWidget {
    public:
        static constexpr OpenCompositionMode staticOpenMode = OpenCompositionMode::AUDIO_AND_NGLYPH;

        explicit AudioNGlyphFormWidget(QWidget* parent = nullptr): IBaseWidget{
                  AudioNGlyphFormWidget::staticOpenMode,
                  QStringLiteral("Audio file (ogg) + NGlyph file (nglyph)"),
                  QList<FormData>{
                      FormData{0, QStringLiteral("Audio file:"), QStringLiteral("Select Audio (ogg)"), QStringLiteral("Compositions (*.ogg)")},
                      FormData{1, QStringLiteral("NGlyph file:"), QStringLiteral("Select NGlyph (nglyph)"), QStringLiteral("NGlyph (*.nglyph)")}
                  },
                  parent
              } {}
    };


    class OpenCompositionDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit OpenCompositionDialog(QWidget* parent = nullptr);

        const std::pair<OpenCompositionMode, QList<QString>>& getResultData() const { return this->resultData; };

    private:
        QComboBox* comboBoxOpenMode;
        QMap<OpenCompositionMode, QSharedPointer<IBaseWidget>> forms;
        QStackedWidget* formContainer;
        QDialogButtonBox* buttonBox;

        std::pair<OpenCompositionMode, QList<QString>> resultData;

        void initUi();
        static QString autoCompleteFilePath(const QString& sourcePath, const QList<QString>& possibleFileExtensions);

    private slots:
        void onComboBoxCurrentIndexChanged(int index);
        void onFileSelected(OpenCompositionMode mode, const QString& path, qsizetype rowIndex);
        void onButtonBoxAccepted();
    };
}

#endif // GV_OPENCOMPOSITIONDIALOG_H
