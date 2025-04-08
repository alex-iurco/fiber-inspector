#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QDebug>
#include <QDir>
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

int main(int argc, char *argv[])
{
    // Set application attributes
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("FiberTools");
    QCoreApplication::setApplicationName("FiberInspector");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    QApplication app(argc, argv);
    
    // Parse command line arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("Fiber Inspection Tool");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // Add custom options
    QCommandLineOption imageOption(QStringList() << "i" << "image", 
                                 "Open the specified image file", "image_path");
    parser.addOption(imageOption);
    
    QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", 
                                      "Start in fullscreen mode");
    parser.addOption(fullscreenOption);
    
    QCommandLineOption debugOption(QStringList() << "d" << "debug", 
                                 "Enable debug mode");
    parser.addOption(debugOption);
    
    parser.process(app);
    
    // Check system requirements
    checkSystemRequirements();
    
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
    
    // Create and show the main window
    MainWindow w;
    
    // Apply command line options
    if (parser.isSet(fullscreenOption)) {
        w.showFullScreen();
    } else {
        w.show();
    }
    
    // Open image if specified
    if (parser.isSet(imageOption)) {
        QString imagePath = parser.value(imageOption);
        QMetaObject::invokeMethod(&w, "openImageFromPath", 
                                Qt::QueuedConnection, 
                                Q_ARG(QString, imagePath));
    }
    
    // Hide splash screen
    splash.finish(&w);
    
    return app.exec();
} 