#include "fiberanalyzer.h"

#include <QDebug>
#include <QMutexLocker>
#include <QRect>
#include <QPoint>
#include <QPainter>
#include <QColor>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

FiberAnalyzer::FiberAnalyzer()
    : m_idealCoreCladRatio(0.8)
    , m_maxAllowedDefects(5.0)
    , m_useGPUAcceleration(false)
{
    // Initialize with default parameters
}

FiberAnalyzer::~FiberAnalyzer()
{
    // Cleanup resources if needed
}

void FiberAnalyzer::setReferenceParameters(double idealCoreCladRatio, double maxAllowedDefects)
{
    QMutexLocker locker(&m_mutex);
    m_idealCoreCladRatio = idealCoreCladRatio;
    m_maxAllowedDefects = maxAllowedDefects;
}

FiberAnalysisResult FiberAnalyzer::analyzeImage(const QImage &processedImage)
{
    QMutexLocker locker(&m_mutex);
    
    FiberAnalysisResult result;
    
    // Initialize default result values
    result.isAcceptable = true;
    result.coreCladRatio = 0.0;
    result.concentricity = 0.0;
    result.overallQuality = 1.0; // 1.0 is perfect, 0.0 is unusable
    result.defects.clear();
    result.annotatedImage = processedImage.copy();
    result.summary = "No defects detected.";
    
    try {
        // Convert QImage to OpenCV Mat
        cv::Mat cvImage = QImage2Mat(processedImage);
        
        // Detect fiber center
        QPoint center = detectFiberCenter(processedImage);
        
        // Measure fiber properties
        QPair<double, double> coreAndCladding = detectCoreAndCladding(processedImage);
        double coreRadius = coreAndCladding.first;
        double claddingRadius = coreAndCladding.second;
        
        // Calculate core-cladding ratio
        if (claddingRadius > 0) {
            result.coreCladRatio = coreRadius / claddingRadius;
        }
        
        // Calculate concentricity
        if (coreRadius > 0 && claddingRadius > 0) {
            result.concentricity = calculateConcentricity(
                cv::Point(center.x(), center.y()), coreRadius, claddingRadius);
        }
        
        // Detect defects
        result.defects = detectDefects(processedImage);
        
        // Analyze results
        result.isAcceptable = isFiberAcceptable(result.defects, result.coreCladRatio);
        
        // Generate annotated image
        result.annotatedImage = createAnnotatedImage(processedImage, result.defects);
        
        // Generate summary
        result.summary = generateSummary(result);
        
        // Calculate overall quality score
        result.overallQuality = calculateQualityScore(result);
        
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception during analysis: " << e.what();
        result.isAcceptable = false;
        result.summary = QString("Analysis error: %1").arg(e.what());
    } catch (const std::exception &e) {
        qWarning() << "Standard exception during analysis: " << e.what();
        result.isAcceptable = false;
        result.summary = QString("Analysis error: %1").arg(e.what());
    }
    
    return result;
}

QPoint FiberAnalyzer::detectFiberCenter(const QImage &image)
{
    // Convert QImage to OpenCV Mat
    cv::Mat cvImage = QImage2Mat(image);
    
    // Convert to grayscale
    cv::Mat gray;
    if (cvImage.channels() > 1) {
        cv::cvtColor(cvImage, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = cvImage.clone();
    }
    
    // Apply Gaussian blur
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    
    // Apply Hough Circle transform to detect circular fiber
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, 
                   blurred.rows/8, 100, 30, 0, 0);
    
    // If circles are found, use the largest one as the fiber
    if (!circles.empty()) {
        cv::Vec3f largest = circles[0];
        for (const auto &circle : circles) {
            if (circle[2] > largest[2]) {
                largest = circle;
            }
        }
        
        return QPoint(cvRound(largest[0]), cvRound(largest[1]));
    }
    
    // If no circles found, return the center of the image
    return QPoint(image.width() / 2, image.height() / 2);
}

double FiberAnalyzer::measureFiberDiameter(const QImage &image)
{
    // Similar to detectFiberCenter but returns the diameter
    cv::Mat cvImage = QImage2Mat(image);
    
    cv::Mat gray;
    if (cvImage.channels() > 1) {
        cv::cvtColor(cvImage, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = cvImage.clone();
    }
    
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
    
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, 
                   blurred.rows/8, 100, 30, 0, 0);
    
    if (!circles.empty()) {
        cv::Vec3f largest = circles[0];
        for (const auto &circle : circles) {
            if (circle[2] > largest[2]) {
                largest = circle;
            }
        }
        
        return largest[2] * 2.0; // Diameter is 2x radius
    }
    
    return 0.0;
}

QPair<double, double> FiberAnalyzer::detectCoreAndCladding(const QImage &image)
{
    // For demo purposes, we'll simulate core and cladding detection
    // In a real implementation, this would use more sophisticated algorithms
    
    double diameter = measureFiberDiameter(image);
    
    // Simulate core radius as 80% of the cladding radius
    double claddingRadius = diameter / 2.0;
    double coreRadius = claddingRadius * 0.8;
    
    return QPair<double, double>(coreRadius, claddingRadius);
}

QVector<FiberDefect> FiberAnalyzer::detectDefects(const QImage &image)
{
    QVector<FiberDefect> defects;
    
    try {
        cv::Mat cvImage = QImage2Mat(image);
        
        // Convert to grayscale
        cv::Mat gray;
        if (cvImage.channels() > 1) {
            cv::cvtColor(cvImage, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = cvImage.clone();
        }
        
        // Apply adaptive threshold to identify potential defects
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                            cv::THRESH_BINARY_INV, 11, 2);
        
        // Find contours of potential defects
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        // Filter contours by size and create defects
        for (const auto &contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 20 && area < 500) {  // Adjust thresholds for expected defect sizes
                cv::Rect boundingRect = cv::boundingRect(contour);
                
                // Create a defect
                FiberDefect defect;
                defect.type = classifyDefect(image.copy(
                    QRect(boundingRect.x, boundingRect.y, boundingRect.width, boundingRect.height)));
                defect.boundingBox = QRect(boundingRect.x, boundingRect.y, 
                                         boundingRect.width, boundingRect.height);
                defect.severity = assessDefectSeverity(defect);
                
                // Set description based on type
                switch (defect.type) {
                    case FiberDefect::DefectType::Scratch:
                        defect.description = "Surface scratch";
                        break;
                    case FiberDefect::DefectType::Chip:
                        defect.description = "Edge chip";
                        break;
                    case FiberDefect::DefectType::Crack:
                        defect.description = "Internal crack";
                        break;
                    case FiberDefect::DefectType::Contamination:
                        defect.description = "Surface contamination";
                        break;
                    default:
                        defect.description = "Unknown defect";
                        break;
                }
                
                defects.append(defect);
            }
        }
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception in defect detection: " << e.what();
    }
    
    return defects;
}

FiberDefect::DefectType FiberAnalyzer::classifyDefect(const QImage &defectRegion)
{
    // Simulate defect classification based on aspect ratio
    // In a real application, this would use machine learning or more sophisticated algorithms
    
    // For demonstration, we'll just use the aspect ratio of the region to classify
    double aspectRatio = static_cast<double>(defectRegion.width()) / defectRegion.height();
    
    if (aspectRatio > 3.0) {
        return FiberDefect::DefectType::Scratch;
    } else if (aspectRatio < 0.33) {
        return FiberDefect::DefectType::Crack;
    } else if (defectRegion.width() > 50) {
        return FiberDefect::DefectType::Chip;
    } else {
        return FiberDefect::DefectType::Contamination;
    }
}

double FiberAnalyzer::assessDefectSeverity(const FiberDefect &defect)
{
    // Calculate severity based on defect type and size
    // Scale from 0.0 (minor) to 1.0 (severe)
    
    double baseScore = 0.0;
    
    // Assign base score by type
    switch (defect.type) {
        case FiberDefect::DefectType::Scratch:
            baseScore = 0.3;
            break;
        case FiberDefect::DefectType::Chip:
            baseScore = 0.5;
            break;
        case FiberDefect::DefectType::Crack:
            baseScore = 0.8;
            break;
        case FiberDefect::DefectType::Contamination:
            baseScore = 0.2;
            break;
        default:
            baseScore = 0.4;
            break;
    }
    
    // Adjust by size (area)
    double area = defect.boundingBox.width() * defect.boundingBox.height();
    double sizeFactor = std::min(1.0, area / 1000.0);
    
    // Calculate final severity
    return std::min(1.0, baseScore + (sizeFactor * 0.5));
}

bool FiberAnalyzer::isFiberAcceptable(const QVector<FiberDefect> &defects, double coreCladRatio)
{
    // Check if the fiber meets quality standards
    
    // Count critical defects (severity > 0.7)
    int criticalDefects = 0;
    double totalSeverity = 0.0;
    
    for (const auto &defect : defects) {
        if (defect.severity > 0.7) {
            criticalDefects++;
        }
        totalSeverity += defect.severity;
    }
    
    // Check core-cladding ratio is within tolerance
    bool ratioAcceptable = (coreCladRatio >= 0.7 * m_idealCoreCladRatio) && 
                         (coreCladRatio <= 1.3 * m_idealCoreCladRatio);
    
    // Check if total severity is below threshold
    bool severityAcceptable = totalSeverity < m_maxAllowedDefects;
    
    // Check if critical defects are below threshold
    bool criticalAcceptable = criticalDefects < 2;
    
    // Fiber is acceptable if all criteria are met
    return ratioAcceptable && severityAcceptable && criticalAcceptable;
}

QImage FiberAnalyzer::createAnnotatedImage(const QImage &original, const QVector<FiberDefect> &defects)
{
    QImage annotated = original.copy();
    QPainter painter(&annotated);
    
    // Draw detected defects with different colors based on type and severity
    for (const auto &defect : defects) {
        QColor color;
        
        // Choose color based on defect type
        switch (defect.type) {
            case FiberDefect::DefectType::Scratch:
                color = QColor(255, 165, 0);  // Orange
                break;
            case FiberDefect::DefectType::Chip:
                color = QColor(255, 0, 0);    // Red
                break;
            case FiberDefect::DefectType::Crack:
                color = QColor(255, 0, 255);  // Magenta
                break;
            case FiberDefect::DefectType::Contamination:
                color = QColor(0, 255, 255);  // Cyan
                break;
            default:
                color = QColor(128, 128, 128); // Gray
                break;
        }
        
        // Adjust opacity based on severity (more opaque = more severe)
        color.setAlphaF(0.3 + (defect.severity * 0.7));
        
        // Draw the defect bounding box
        painter.setPen(QPen(color, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(defect.boundingBox);
        
        // Add label with defect type and severity
        QString label = QString("%1 (%.2f)")
                      .arg(defect.description)
                      .arg(defect.severity);
        
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);
        
        painter.setPen(Qt::white);
        painter.drawText(
            defect.boundingBox.adjusted(0, -20, 0, 0),
            Qt::AlignTop | Qt::AlignHCenter,
            label
        );
    }
    
    // Draw fiber center and measurements
    QPoint center = detectFiberCenter(original);
    QPair<double, double> radii = detectCoreAndCladding(original);
    
    // Draw cladding circle
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, static_cast<int>(radii.second), static_cast<int>(radii.second));
    
    // Draw core circle
    painter.setPen(QPen(QColor(0, 0, 255), 2));
    painter.drawEllipse(center, static_cast<int>(radii.first), static_cast<int>(radii.first));
    
    // Draw center point
    painter.setPen(QPen(Qt::red, 3));
    painter.drawPoint(center);
    
    return annotated;
}

void FiberAnalyzer::enableGPUAcceleration(bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_useGPUAcceleration = enable;
    
    // If this were a real implementation, we would initialize or release
    // GPU resources here as needed
}

bool FiberAnalyzer::isGPUAccelerationAvailable()
{
    // Check if OpenCV was built with CUDA support
    bool hasCuda = false;
    
#ifdef CV_CUDA
    // In a real implementation, we would check if CUDA devices are available
    int deviceCount = cv::cuda::getCudaEnabledDeviceCount();
    hasCuda = (deviceCount > 0);
#endif
    
    return hasCuda;
}

cv::Mat FiberAnalyzer::preProcessForAnalysis(const cv::Mat &inputImage)
{
    // Pre-process the image to improve analysis accuracy
    cv::Mat processed;
    
    // Convert to grayscale if needed
    if (inputImage.channels() > 1) {
        cv::cvtColor(inputImage, processed, cv::COLOR_BGR2GRAY);
    } else {
        processed = inputImage.clone();
    }
    
    // Apply Gaussian blur to reduce noise
    cv::GaussianBlur(processed, processed, cv::Size(5, 5), 0);
    
    // Apply contrast enhancement
    cv::equalizeHist(processed, processed);
    
    return processed;
}

std::vector<cv::Rect> FiberAnalyzer::detectDefectRegions(const cv::Mat &processedImage)
{
    // Use thresholding to identify potential defects
    cv::Mat binary;
    cv::adaptiveThreshold(processedImage, binary, 255, 
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                        cv::THRESH_BINARY_INV, 11, 2);
    
    // Find contours of potential defects
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    // Convert contours to rectangles
    std::vector<cv::Rect> defectRegions;
    for (const auto &contour : contours) {
        double area = cv::contourArea(contour);
        if (area > 20 && area < 500) {  // Filter by size
            cv::Rect boundingRect = cv::boundingRect(contour);
            defectRegions.push_back(boundingRect);
        }
    }
    
    return defectRegions;
}

double FiberAnalyzer::calculateConcentricity(const cv::Point &center, double coreRadius, double claddingRadius)
{
    // Calculate how well-centered the core is within the cladding
    // 1.0 means perfectly centered, 0.0 means completely off-center
    
    if (claddingRadius <= 0) {
        return 0.0;
    }
    
    // The maximum possible distance between centers is the cladding radius
    double maxDistance = claddingRadius - coreRadius;
    if (maxDistance <= 0) {
        return 1.0;  // Core and cladding are basically the same
    }
    
    // For this demo, we'll just return a simulated value
    // In a real application, we would calculate the actual distance between centers
    
    // Simulate slight off-center with value between 0.95 and 1.0
    return 0.95 + (rand() % 5) / 100.0;
}

QString FiberAnalyzer::generateSummary(const FiberAnalysisResult &result)
{
    // Create a human-readable summary of the analysis results
    QString summary;
    
    // Overall status
    if (result.isAcceptable) {
        summary += "PASS: Fiber meets quality standards.\n";
    } else {
        summary += "FAIL: Fiber does not meet quality standards.\n";
    }
    
    // Add measurement information
    summary += QString("Core-Cladding Ratio: %.3f (Ideal: %.3f)\n")
              .arg(result.coreCladRatio)
              .arg(m_idealCoreCladRatio);
    
    summary += QString("Concentricity: %.3f\n")
              .arg(result.concentricity);
    
    summary += QString("Overall Quality Score: %.2f\n")
              .arg(result.overallQuality);
    
    // Add defect information
    summary += QString("Defects found: %1\n").arg(result.defects.size());
    
    if (!result.defects.isEmpty()) {
        summary += "Defect List:\n";
        for (int i = 0; i < result.defects.size(); ++i) {
            const FiberDefect &defect = result.defects[i];
            summary += QString("%1. %2 (Severity: %.2f)\n")
                      .arg(i + 1)
                      .arg(defect.description)
                      .arg(defect.severity);
        }
    }
    
    return summary;
}

double FiberAnalyzer::calculateQualityScore(const FiberAnalysisResult &result)
{
    // Calculate an overall quality score from 0.0 (worst) to 1.0 (best)
    
    // Start with perfect score
    double score = 1.0;
    
    // Reduce score based on defects
    for (const auto &defect : result.defects) {
        score -= defect.severity * 0.1;  // Each defect can reduce up to 0.1
    }
    
    // Reduce score based on concentricity
    score -= (1.0 - result.concentricity) * 0.3;
    
    // Reduce score based on core-cladding ratio deviation
    double ratioDifference = std::abs(result.coreCladRatio - m_idealCoreCladRatio) / m_idealCoreCladRatio;
    score -= ratioDifference * 0.3;
    
    // Ensure score is between 0 and 1
    return std::max(0.0, std::min(1.0, score));
}

cv::Mat FiberAnalyzer::QImage2Mat(const QImage &image)
{
    switch (image.format()) {
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied: {
        cv::Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.constBits()), image.bytesPerLine());
        cv::Mat matNoAlpha;
        cv::cvtColor(mat, matNoAlpha, cv::COLOR_BGRA2BGR);
        return matNoAlpha;
    }
    case QImage::Format_RGB888: {
        cv::Mat mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.constBits()), image.bytesPerLine());
        cv::Mat matSwapped;
        cv::cvtColor(mat, matSwapped, cv::COLOR_RGB2BGR);
        return matSwapped;
    }
    case QImage::Format_Grayscale8: {
        cv::Mat mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.constBits()), image.bytesPerLine());
        return mat;
    }
    default: {
        // Convert to ARGB32 format if format is not directly supported
        QImage converted = image.convertToFormat(QImage::Format_ARGB32);
        cv::Mat mat(converted.height(), converted.width(), CV_8UC4, const_cast<uchar*>(converted.constBits()), converted.bytesPerLine());
        cv::Mat matNoAlpha;
        cv::cvtColor(mat, matNoAlpha, cv::COLOR_BGRA2BGR);
        return matNoAlpha;
    }
    }
} 