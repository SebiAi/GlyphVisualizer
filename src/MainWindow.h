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

#ifndef GV_MAINWINDOW_H
#define GV_MAINWINDOW_H

#include <QAction>
#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QShowEvent>
#include <QStatusBar>
#include <QString>
#include <QStringLiteral>
#include <QStyle>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

#include "BuildInfo.h"
#include "CompositionManager.h"
#include "configurations/ConfigurationManager.h"
#include "configurations/DeviceBuild.h"
#include "Config.h"
#include "DonationDialog.h"
#include "OpenCompositionDialog.h"
#include "RenderingSettingsDialog.h"
#include "UpdateChecker.h"
#include "widgets/CompositionManagerControls.h"
#include "widgets/GlyphWidget.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(mainWindow)
Q_DECLARE_LOGGING_CATEGORY(mainWindowVerbose)

using namespace DeviceBuildNS;
using namespace OpenCompositionDialogNS;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Config* config, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void showEvent(QShowEvent *event) override;

    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private:
    Config* config;
    UpdateChecker* updateChecker;

    bool firstShow;

    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* openFileAction;
    QAction* exportAsVideoAction;
    QAction* quitAction;
    QMenu* helpMenu;
    QAction* checkForUpdateAction;
    QAction* donateAction;
    QAction* aboutAction;

    OpenCompositionDialog* openCompositionDialog;
    RenderingSettingsDialog* renderingSettingsDialog;

    QStatusBar* statusBar;
    QWidget* centralWidget;
    QVBoxLayout* centralLayout;
    GlyphWidget* glyphWidget;
    CompositionManagerControls* compositonManagerControls;

    ConfigurationManager configurationManager;
    CompositionManager compositonManager;

    bool compositionWasPlaying;

    void initUi();
    void loadComposition(const std::pair<OpenCompositionMode, QList<QString>>& compositionData, bool reopenOpenCompositionDialog);

private slots:
    void onUpdateCheckerUpdateAvailable(const QString& newVersion);
    void onUpdateCheckerUpdateCheckFailed(const QString& errorMessage);
    void onUpdateCheckerNoUpdateAvailable();

    void onCompositionManagerTick(qint64 position);
    void onCompositionManagerMediaStatusChanged(QMediaPlayer::MediaStatus status);

    void onOpenFileActionTriggered();
    void onExportAsVideoActionTriggered();
    void onCheckForUpdateActionTriggered();
    void onDonateActionTriggered();
    void onAboutActionTriggered();

    void onOpenCompositionDialogFinished(int result);
    void onRenderingSettingsDialogFinished(int result);
};
#endif // GV_MAINWINDOW_H
