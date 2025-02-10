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

#ifndef GV_RENDERINGSETTINGSDIALOG_H
#define GV_RENDERINGSETTINGSDIALOG_H

#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QShowEvent>
#include <QSize>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStringLiteral>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "CompositionRenderer.h"
#include "configurations/IConfiguration.h"
#include "Utils.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(renderingSettingsDialog)
Q_DECLARE_LOGGING_CATEGORY(renderingSettingsDialogVerbose)

class RenderingSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RenderingSettingsDialog(QWidget* parent = nullptr);
    ~RenderingSettingsDialog();

public slots:
    void open() override { throw std::logic_error("RenderingSettingsDialog: Please use the correct open function!"); };
    void open(const QString& audioPath, IConfiguration* config);

protected:
    virtual void showEvent(QShowEvent* event) override;

private:
    QVBoxLayout* layout;

    QLineEdit* filePathLineEdit;
    QPushButton* filePathBrowseButton;

    QComboBox* resolutionDropdown;

    QToolButton* backgroundColorButton;
    QLabel* backgroundColorLabel;

    QLineEdit* ffmpegPathLineEdit;
    QPushButton* ffmpegPathAutoDetectButton;
    QPushButton* ffmpegPathBrowseButton;

    QDialogButtonBox* buttonBox;

    QString audioPath;
    IConfiguration* config;

    QProgressDialog* progressDialog;
    CompositionRenderer* renderer;

    void initUi();
    void updatebackgroundColorUI(const QColor& color);
    void afterRenderCleanup();

private slots:
    void onFilePathBrowseButtonClicked();
    void onBackgroundColorButtonClicked();
    void onFFmpegPathAutoDetectButton();
    void onFFmpegPathBrowseButtonClicked();
    void onButtonBoxAccepted();

    void onRendererAborted();
    void onRendererFinished();
    void onRendererFFmpegError(const QString& error);
    void onRendererUnexpectedError(const QString& error);
};

#endif // GV_RENDERINGSETTINGSDIALOG_H
