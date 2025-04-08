#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QDebug>
#include <QSplashScreen>
#include <QThread>
#include <QtCore>

#include "mainwindow.h"

// Detect if running on Linux
#ifdef Q_OS_LINUX
#include <sys/utsname.h>
#include <unistd.h>
#endif

void checkSystemRequirements() {
#ifdef Q_OS_LINUX
    // Check for system resources and capabilities
    struct utsname systemInfo;
    uname(&systemInfo);
    qDebug() << "System: " << systemInfo.sysname << systemInfo.release;
    
    // Check for OpenCV dependencies and GPU availability
    QProcess process;
    process.start("ldconfig -p | grep libopencv");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    if (output.isEmpty()) {
        qWarning() << "Warning: OpenCV libraries not found in system paths.";
    }
    
    // Check for access to camera devices
    QFile cameraDevice("/dev/video0");
    if (!cameraDevice.exists()) {
        qWarning() << "Warning: No camera device found at /dev/video0";
    }
#endif
}

void setupLogging(bool verbose)
{
    // Set up message handling
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz}] [%{type}] %{message}");
    
    // Filter out debug messages unless verbose mode is enabled
    if (!verbose) {
        qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
            if (type == QtDebugMsg) {
                return; // Drop debug messages
            }
            
            // Print other messages to stderr
            fprintf(stderr, "%s\n", qPrintable(qFormatLogMessage(type, context, msg)));
        });
    }
}

int main(int argc, char *argv[])
{
    // Set application attributes
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("FiberTools");
    QCoreApplication::setApplicationName("FiberInspector");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    QApplication app(argc, argv);
    
    // Set up command line options
    QCommandLineParser parser;
    parser.setApplicationDescription("Fiber optic endface inspection tool");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Enable verbose output");
    parser.addOption(verboseOption);
    
    QCommandLineOption imageOption(QStringList() << "i" << "image", "Open image file on startup", "file");
    parser.addOption(imageOption);
    
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Start in fullscreen mode");
    parser.addOption(fullscreenOption);
    
    QCommandLineOption darkModeOption(QStringList() << "d" << "dark-mode", "Use dark color theme");
    parser.addOption(darkModeOption);
    
    // Process the command line arguments
    parser.process(app);
    
    // Check system requirements
    checkSystemRequirements();
    
    // Set up logging based on verbose flag
    setupLogging(parser.isSet(verboseOption));
    
    // Load application style
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.exists()) {
        styleFile.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&styleFile);
        app.setStyleSheet(ts.readAll());
    }
    
    // Show splash screen
    QSplashScreen splash(QPixmap(":/images/splash.png"));
    splash.show();
    app.processEvents();
    
    // Create the main window
    MainWindow mainWindow;
    
    // Apply settings from command line
    if (parser.isSet(fullscreenOption)) {
        mainWindow.showFullScreen();
    } else {
        mainWindow.show();
    }
    
    if (parser.isSet(darkModeOption)) {
        // Set dark palette for the application
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        app.setPalette(darkPalette);
    }
    
    // Load image if specified
    if (parser.isSet(imageOption)) {
        QString imagePath = parser.value(imageOption);
        mainWindow.loadImage(imagePath);
    }
    
    // Hide splash screen
    splash.finish(&mainWindow);
    
    qDebug() << "FiberInspector application started";
    
    return app.exec();
} 