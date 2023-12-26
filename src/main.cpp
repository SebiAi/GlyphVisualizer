#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineParser>

#include "APPLICATION_INFO.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(mainFunction)
Q_DECLARE_LOGGING_CATEGORY(mainFunctionVerbose)
Q_LOGGING_CATEGORY(mainFunction, "Main")
Q_LOGGING_CATEGORY(mainFunctionVerbose, "Main.Verbose")

int main(int argc, char *argv[])
{
    // Set up application
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("SebiAi");
    QCoreApplication::setOrganizationDomain("com.sebiai");
    QCoreApplication::setApplicationName("GlyphVisualizer");
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

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

    qCInfo(mainFunctionVerbose) << "Current software version:" << APPLICATION_VERSION;
    qCInfo(mainFunctionVerbose) << "Current software git hash:" << APPLICATION_GIT_COMMIT_HASH;

    // Set up MainWindow
    MainWindow w;
    w.show();

    // Handle early quit
    if (w.earlyQuit)
    {
        qCInfo(mainFunction) << "Early quit activated. Returning early...";
        return 0;
    }

    // Execute main event loop
    return a.exec();
}
