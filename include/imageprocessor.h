#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QString>
#include <QMap>
#include <QMutex>
#include <opencv2/opencv.hpp>

enum class FilterType {
    None,
    Grayscale,
    Threshold,
    EdgeDetection,
    Sharpen,
    MedianBlur,
    GaussianBlur,
    CustomFilter
};

class ImageProcessor
{
public:
    ImageProcessor();
    ~ImageProcessor();

    bool loadImage(const QString &filePath);
    bool saveImage(const QString &filePath, const QImage &image);
    
    QImage applyFilter(const QImage &sourceImage, FilterType filter);
    QImage adjustBrightness(const QImage &sourceImage, int value);
    QImage adjustContrast(const QImage &sourceImage, int value);
    
    // Advanced image processing methods
    QImage enhanceFiberEdges(const QImage &sourceImage);
    QImage removeNoise(const QImage &sourceImage);
    QImage highlightDefects(const QImage &sourceImage);
    
    // OpenCV integration
    cv::Mat qImageToMat(const QImage &image);
    QImage matToQImage(const cv::Mat &mat);
    
    // Custom filter application
    QImage applyCustomFilter(const QImage &sourceImage, const QVector<float> &kernelData, int kernelSize);
    
    // Processing state management
    void cancelProcessing();
    bool isProcessing() const;

private:
    QMutex m_mutex;
    bool m_isProcessing;
    QMap<FilterType, QString> m_filterNames;
    
    // Helper methods for specific filters
    QImage applySobelFilter(const QImage &sourceImage);
    QImage applyCannyEdgeDetection(const QImage &sourceImage);
    QImage applySharpenFilter(const QImage &sourceImage);
    QImage applyAdaptiveThreshold(const QImage &sourceImage);
};

#endif // IMAGEPROCESSOR_H 