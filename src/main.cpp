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

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QStringLiteral>
#include <QTranslator>

#include "Config.h"
#include "BuildInfo.h"
#include "OpenCompositionDialog.h"
#include "Utils.h"
#include "WindowsLoggingWorkaround.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(mainFunction)
Q_DECLARE_LOGGING_CATEGORY(mainFunctionVerbose)
Q_LOGGING_CATEGORY(mainFunction, "Main")
Q_LOGGING_CATEGORY(mainFunctionVerbose, "Main.Verbose")

int main(int argc, char *argv[])
{
    // This object attaches to an existing console so logs are properly output to it.
    // Frees the console on deconstruction.
    // Does nothing on other platforms than Windows
    WindowsLoggingWorkaround wlw;

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("SebiAi"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("com.sebiai"));
    QCoreApplication::setApplicationName(QStringLiteral("GlyphVisualizer"));
    QCoreApplication::setApplicationVersion(QStringLiteral(BUILDINFO_VERSION));

    try {
        // QTranslator translator;
        // const QStringList uiLanguages = QLocale::system().uiLanguages();
        // for (const QString &locale : uiLanguages) {
        //     const QString baseName = "GlyphVisualizer_" + QLocale(locale).name();
        //     if (translator.load(QStringLiteral(":/i18n/") + baseName)) {
        //         a.installTranslator(&translator);
        //         break;
        //     }
        // }

        // Set up command line parser
        QCommandLineParser parser;
        parser.setApplicationDescription("A Glyph composition player written with the Qt6 framework in C++ that plays Glyph compositions from Nothing Phones.");
        parser.addHelpOption();
        parser.addVersionOption();
        // Add verbose logging option
        QCommandLineOption verboseLogging("verbose", "Enable verbose logging.");
        parser.addOption(verboseLogging);

        // Parse command line options
        parser.process(a);

        // Set verbose logging
        if (parser.isSet(verboseLogging))
        {
            QLoggingCategory::setFilterRules("*.Verbose=true");
            qCInfo(mainFunctionVerbose) << "Verbose logging activated";
        }
        else QLoggingCategory::setFilterRules("*.Verbose=false");

        qCInfo(mainFunctionVerbose) << "#### Software Information #####";
        qCInfo(mainFunctionVerbose) << "Current software version:" << BUILDINFO_VERSION;
        qCInfo(mainFunctionVerbose) << "Current software git hash:" << BUILDINFO_GIT_COMMIT_HASH;

        qCInfo(mainFunctionVerbose) << "#### System Information #####";
        qCInfo(mainFunctionVerbose) << "Product name and version:" << QSysInfo::prettyProductName();
        qCInfo(mainFunctionVerbose) << "Kernel type and version:" << QSysInfo::kernelType() << QSysInfo::kernelVersion();

        qCInfo(mainFunctionVerbose) << "#### Logging Start #####";

        // Init config
        Config config;
        bool resetConfig{false};
        do {
            try {
                config.load(getAppConfigLocation().absoluteFilePath(QStringLiteral("GlyphVisualizer.ini")), resetConfig);
                resetConfig = false;
            } catch (const Config::ConfigVersionTooHighError& e) {
                qCCritical(mainFunction) << "ConfigVersionTooHighError occurred";
                QMessageBox msg{
                    QMessageBox::Icon::Warning,
                    QStringLiteral("Config Error"),
                    QStringLiteral("The configuration file in '%1' is intended to be used with a higher version of this software.\n\n"
                                   "This usually happens when you either downgrade your software or manually modify the config.\n\n"
                                   "Do you want to clear the config (Recommended) or exit the application?").arg(e.filePath.c_str())
                };
                QPushButton* exitSoftwareButton{msg.addButton("Exit Software", QMessageBox::ButtonRole::AcceptRole)};
                QPushButton* clearConfigButton{msg.addButton("Clear Config (Recommended)", QMessageBox::ButtonRole::DestructiveRole)};
                msg.exec();

                if (msg.clickedButton() == exitSoftwareButton) {
                    qCWarning(mainFunction) << "ConfigVersionTooHighError - User decided to exit";
                    return 0;
                } else if (msg.clickedButton() == clearConfigButton) {
                    qCWarning(mainFunction) << "ConfigVersionTooHighError - User decided to clear";
                    resetConfig = true;
                }
            }
        } while (resetConfig);


        MainWindow w{&config};
        w.show();
        int returnCode{a.exec()};

        // Update config
        config.setValue(Config::Setting::FirstStart_Bool, false);

        return returnCode;
    } catch (const std::exception& e) {
        qCCritical(mainFunction) << "Unexpected Exception:" << e.what();

        QMessageBox msg{QMessageBox::Icon::Critical, QStringLiteral("Unexpected Exception"), QStringLiteral("An unexpected error occured:\n%1\n\nThe application will now exit!").arg(e.what()), QMessageBox::StandardButton::Ok};
        msg.exec();
        return 1;
    }
}
