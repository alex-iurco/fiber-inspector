#include <iostream>
#include <string>
#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <QDebug>
#include <QPainter>

#include "imageprocessor.h"
#include "fiberanalyzer.h"
#include "resultsmanager.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    // Redirect qDebug to std::cout
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        std::cout << qPrintable(msg) << std::endl;
    });
    
    std::cout << "===== Testing FiberInspector Core Functionality =====" << std::endl;
    
    // Initialize components
    ImageProcessor imageProcessor;
    FiberAnalyzer fiberAnalyzer;
    ResultsManager resultsManager;
    
    // Create a test image
    QImage testImage(640, 480, QImage::Format_RGB32);
    testImage.fill(Qt::black);
    
    // Draw a circle to simulate a fiber
    QPainter painter(&testImage);
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(QPoint(320, 240), 100, 100);  // Outer circle (cladding)
    painter.setBrush(QBrush(Qt::gray));
    painter.drawEllipse(QPoint(320, 240), 80, 80);    // Inner circle (core)
    painter.end();
    
    // Save the test image
    QString testImagePath = QDir::currentPath() + "/test_fiber.png";
    testImage.save(testImagePath);
    std::cout << "Created test image: " << testImagePath.toStdString() << std::endl;
    
    // Test image processing
    std::cout << "\nTesting image processing..." << std::endl;
    QImage processedImage = imageProcessor.applyFilter(testImage, FilterType::Grayscale);
    std::cout << "Applied grayscale filter: " << 
        (processedImage.isNull() ? "FAILED" : "SUCCESS") << std::endl;
    
    processedImage = imageProcessor.applyFilter(testImage, FilterType::EdgeDetection);
    std::cout << "Applied edge detection: " << 
        (processedImage.isNull() ? "FAILED" : "SUCCESS") << std::endl;
    
    // Test fiber analysis
    std::cout << "\nTesting fiber analysis..." << std::endl;
    FiberAnalysisResult result = fiberAnalyzer.analyzeImage(testImage);
    
    std::cout << "Analysis results:" << std::endl;
    std::cout << "- Core-cladding ratio: " << result.coreCladRatio << std::endl;
    std::cout << "- Concentricity: " << result.concentricity << std::endl;
    std::cout << "- Defects found: " << result.defects.size() << std::endl;
    std::cout << "- Quality score: " << result.overallQuality << std::endl;
    std::cout << "- Is acceptable: " << (result.isAcceptable ? "Yes" : "No") << std::endl;
    
    // Test results saving
    std::cout << "\nTesting results management..." << std::endl;
    QString resultPath = QDir::currentPath() + "/test_result.json";
    bool saveSuccess = resultsManager.exportToJSON(result, resultPath);
    std::cout << "Saved result to JSON: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test PDF export
    QString pdfPath = QDir::currentPath() + "/test_report.pdf";
    bool pdfSuccess = resultsManager.exportToPDF(result, pdfPath);
    std::cout << "Exported PDF report: " << (pdfSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    std::cout << "\n===== Core Functionality Test Complete =====" << std::endl;
    
    return 0;
} 