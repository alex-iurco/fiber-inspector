#ifndef FIBERANALYZER_H
#define FIBERANALYZER_H

#include <QImage>
#include <QVector>
#include <QPair>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QMutex>
#include <opencv2/opencv.hpp>

// Struct to hold defect information
struct FiberDefect {
    enum class DefectType {
        Scratch,
        Chip,
        Crack,
        Contamination,
        Unknown
    };
    
    DefectType type;
    QRect boundingBox;
    double severity;
    QString description;
};

// Analysis results
struct FiberAnalysisResult {
    bool isAcceptable;
    double coreCladRatio;
    double concentricity;
    double overallQuality;
    QVector<FiberDefect> defects;
    QImage annotatedImage;
    QString summary;
};

class FiberAnalyzer
{
public:
    FiberAnalyzer();
    ~FiberAnalyzer();
    
    void setReferenceParameters(double idealCoreCladRatio, double maxAllowedDefects);
    FiberAnalysisResult analyzeImage(const QImage &processedImage);
    
    // Detection methods
    QPoint detectFiberCenter(const QImage &image);
    double measureFiberDiameter(const QImage &image);
    QPair<double, double> detectCoreAndCladding(const QImage &image);
    QVector<FiberDefect> detectDefects(const QImage &image);
    
    // Classification methods
    FiberDefect::DefectType classifyDefect(const QImage &defectRegion);
    double assessDefectSeverity(const FiberDefect &defect);
    
    // Analysis methods
    bool isFiberAcceptable(const QVector<FiberDefect> &defects, double coreCladRatio);
    QImage createAnnotatedImage(const QImage &original, const QVector<FiberDefect> &defects);
    
    // Linux system integration for improved performance
    void enableGPUAcceleration(bool enable);
    bool isGPUAccelerationAvailable();

private:
    QMutex m_mutex;
    double m_idealCoreCladRatio;
    double m_maxAllowedDefects;
    bool m_useGPUAcceleration;
    
    // OpenCV-based methods
    cv::Mat preProcessForAnalysis(const cv::Mat &inputImage);
    std::vector<cv::Rect> detectDefectRegions(const cv::Mat &processedImage);
    double calculateConcentricity(const cv::Point &center, double coreRadius, double claddingRadius);
    
    // Missing functions that need to be added
    cv::Mat QImage2Mat(const QImage &image);
    QString generateSummary(const FiberAnalysisResult &result);
    double calculateQualityScore(const FiberAnalysisResult &result);
};

#endif // FIBERANALYZER_H 