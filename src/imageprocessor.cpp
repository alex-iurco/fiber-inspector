#include "imageprocessor.h"

#include <QDebug>
#include <QMutexLocker>
#include <QFile>
#include <QFileInfo>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

ImageProcessor::ImageProcessor()
    : m_isProcessing(false)
{
    // Initialize filter names map
    m_filterNames[FilterType::None] = tr("No Filter");
    m_filterNames[FilterType::Grayscale] = tr("Grayscale");
    m_filterNames[FilterType::Threshold] = tr("Threshold");
    m_filterNames[FilterType::EdgeDetection] = tr("Edge Detection");
    m_filterNames[FilterType::Sharpen] = tr("Sharpen");
    m_filterNames[FilterType::MedianBlur] = tr("Median Blur");
    m_filterNames[FilterType::GaussianBlur] = tr("Gaussian Blur");
}

ImageProcessor::~ImageProcessor()
{
    cancelProcessing();
}

bool ImageProcessor::loadImage(const QString &filePath)
{
    if (!QFile::exists(filePath)) {
        qWarning() << "Image file does not exist: " << filePath;
        return false;
    }
    
    // Try loading with OpenCV first
    try {
        cv::Mat img = cv::imread(filePath.toStdString());
        if (img.empty()) {
            qWarning() << "OpenCV could not load image: " << filePath;
            
            // Fallback to Qt
            QImage qimg(filePath);
            return !qimg.isNull();
        }
        return true;
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when loading image: " << e.what();
        return false;
    }
}

bool ImageProcessor::saveImage(const QString &filePath, const QImage &image)
{
    if (image.isNull()) {
        qWarning() << "Cannot save null image";
        return false;
    }
    
    try {
        // Convert QImage to OpenCV Mat
        cv::Mat mat = qImageToMat(image);
        
        // Save using OpenCV
        return cv::imwrite(filePath.toStdString(), mat);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when saving image: " << e.what();
        
        // Fallback to Qt
        return image.save(filePath);
    }
}

QImage ImageProcessor::applyFilter(const QImage &sourceImage, FilterType filter)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    QMutexLocker locker(&m_mutex);
    m_isProcessing = true;
    
    try {
        // Convert QImage to OpenCV Mat for processing
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst;
        
        // Apply the selected filter
        switch (filter) {
            case FilterType::None:
                // No processing needed
                dst = src.clone();
                break;
                
            case FilterType::Grayscale:
                // Convert to grayscale
                if (src.channels() == 3 || src.channels() == 4) {
                    cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
                    cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR); // Convert back to BGR for consistency
                } else {
                    dst = src.clone();
                }
                break;
                
            case FilterType::Threshold:
                // Apply adaptive threshold
                dst = applyAdaptiveThreshold(sourceImage);
                break;
                
            case FilterType::EdgeDetection:
                // Apply edge detection
                dst = applyCannyEdgeDetection(sourceImage);
                break;
                
            case FilterType::Sharpen:
                // Apply sharpen filter
                dst = applySharpenFilter(sourceImage);
                break;
                
            case FilterType::MedianBlur:
                // Apply median blur (good for reducing noise)
                cv::medianBlur(src, dst, 5);  // Kernel size 5x5
                break;
                
            case FilterType::GaussianBlur:
                // Apply Gaussian blur
                cv::GaussianBlur(src, dst, cv::Size(5, 5), 0);
                break;
                
            case FilterType::CustomFilter:
                // Apply a custom filter (not implemented here)
                qWarning() << "Custom filter not implemented";
                dst = src.clone();
                break;
                
            default:
                qWarning() << "Unknown filter type";
                dst = src.clone();
                break;
        }
        
        // Convert back to QImage
        QImage result = matToQImage(dst);
        m_isProcessing = false;
        return result;
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying filter: " << e.what();
        m_isProcessing = false;
        return sourceImage; // Return original image on error
    }
}

QImage ImageProcessor::adjustBrightness(const QImage &sourceImage, int value)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst = src.clone();
        
        // Apply brightness adjustment
        dst.convertTo(dst, -1, 1.0, value);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when adjusting brightness: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::adjustContrast(const QImage &sourceImage, int value)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst = src.clone();
        
        // Calculate contrast factor (1.0 is neutral)
        double contrastFactor = 1.0 + (value / 100.0);
        
        // Apply contrast adjustment
        dst.convertTo(dst, -1, contrastFactor, 0);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when adjusting contrast: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::enhanceFiberEdges(const QImage &sourceImage)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst;
        
        // Convert to grayscale if needed
        cv::Mat gray;
        if (src.channels() == 3 || src.channels() == 4) {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = src.clone();
        }
        
        // Apply Canny edge detection with appropriate thresholds for fiber edges
        cv::Canny(gray, dst, 30, 90);
        
        // Dilate to make edges more visible
        cv::dilate(dst, dst, cv::Mat(), cv::Point(-1, -1), 1);
        
        // Convert back to color for display
        cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when enhancing fiber edges: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::removeNoise(const QImage &sourceImage)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst;
        
        // Apply non-local means denoising
        cv::fastNlMeansDenoisingColored(src, dst, 10, 10, 7, 21);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when removing noise: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::highlightDefects(const QImage &sourceImage)
{
    if (sourceImage.isNull()) {
        return QImage();
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst = src.clone();
        
        // Convert to grayscale
        cv::Mat gray;
        if (src.channels() == 3 || src.channels() == 4) {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = src.clone();
        }
        
        // Apply adaptive threshold to identify potential defects
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                            cv::THRESH_BINARY_INV, 11, 2);
        
        // Find contours of potential defects
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        // Filter contours by size and draw them in red
        for (const auto &contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 20 && area < 500) {  // Adjust thresholds based on expected defect size
                cv::drawContours(dst, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(0, 0, 255), 2);
            }
        }
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when highlighting defects: " << e.what();
        return sourceImage;
    }
}

cv::Mat ImageProcessor::qImageToMat(const QImage &image)
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

QImage ImageProcessor::matToQImage(const cv::Mat &mat)
{
    // Check if empty
    if (mat.empty())
        return QImage();
    
    // 8-bit, 3 channel
    if (mat.type() == CV_8UC3) {
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, 
                    rgbMat.step, QImage::Format_RGB888).copy();
    }
    // 8-bit, 1 channel
    else if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, 
                    mat.step, QImage::Format_Grayscale8).copy();
    }
    // 8-bit, 4 channel
    else if (mat.type() == CV_8UC4) {
        return QImage(mat.data, mat.cols, mat.rows, 
                    mat.step, QImage::Format_ARGB32).copy();
    }
    // 32-bit float single channel
    else if (mat.type() == CV_32FC1) {
        cv::Mat convertedMat;
        mat.convertTo(convertedMat, CV_8UC1, 255.0);
        return QImage(convertedMat.data, convertedMat.cols, convertedMat.rows, 
                    convertedMat.step, QImage::Format_Grayscale8).copy();
    }
    
    qWarning() << "Unsupported mat format for conversion to QImage";
    return QImage();
}

QImage ImageProcessor::applyCustomFilter(const QImage &sourceImage, const QVector<float> &kernelData, int kernelSize)
{
    if (sourceImage.isNull() || kernelData.isEmpty() || kernelSize <= 0) {
        return sourceImage;
    }
    
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst;
        
        // Create kernel from provided data
        cv::Mat kernel(kernelSize, kernelSize, CV_32F);
        for (int i = 0; i < kernelSize * kernelSize && i < kernelData.size(); ++i) {
            kernel.at<float>(i / kernelSize, i % kernelSize) = kernelData[i];
        }
        
        // Apply filter
        cv::filter2D(src, dst, -1, kernel);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying custom filter: " << e.what();
        return sourceImage;
    }
}

void ImageProcessor::cancelProcessing()
{
    QMutexLocker locker(&m_mutex);
    if (m_isProcessing) {
        // This would be used to signal worker threads to stop
        m_isProcessing = false;
    }
}

bool ImageProcessor::isProcessing() const
{
    return m_isProcessing;
}

QImage ImageProcessor::applySobelFilter(const QImage &sourceImage)
{
    try {
        cv::Mat src = qImageToMat(sourceImage);
        
        // Convert to grayscale
        cv::Mat gray;
        if (src.channels() == 3 || src.channels() == 4) {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = src.clone();
        }
        
        // Apply Sobel in X and Y directions
        cv::Mat sobelX, sobelY, sobelCombined;
        cv::Sobel(gray, sobelX, CV_16S, 1, 0);
        cv::Sobel(gray, sobelY, CV_16S, 0, 1);
        
        // Convert to CV_8U
        cv::Mat absX, absY;
        cv::convertScaleAbs(sobelX, absX);
        cv::convertScaleAbs(sobelY, absY);
        
        // Combine results
        cv::addWeighted(absX, 0.5, absY, 0.5, 0, sobelCombined);
        
        // Convert back to color for display
        cv::Mat result;
        cv::cvtColor(sobelCombined, result, cv::COLOR_GRAY2BGR);
        
        return matToQImage(result);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying Sobel filter: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::applyCannyEdgeDetection(const QImage &sourceImage)
{
    try {
        cv::Mat src = qImageToMat(sourceImage);
        
        // Convert to grayscale
        cv::Mat gray;
        if (src.channels() == 3 || src.channels() == 4) {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = src.clone();
        }
        
        // Apply Gaussian blur to reduce noise
        cv::Mat blurred;
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.4);
        
        // Apply Canny edge detection
        cv::Mat edges;
        cv::Canny(blurred, edges, 50, 150);
        
        // Convert back to color for display
        cv::Mat result;
        cv::cvtColor(edges, result, cv::COLOR_GRAY2BGR);
        
        return matToQImage(result);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying Canny edge detection: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::applySharpenFilter(const QImage &sourceImage)
{
    try {
        cv::Mat src = qImageToMat(sourceImage);
        cv::Mat dst;
        
        // Create sharpening kernel
        cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        
        // Apply filter
        cv::filter2D(src, dst, -1, kernel);
        
        return matToQImage(dst);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying sharpen filter: " << e.what();
        return sourceImage;
    }
}

QImage ImageProcessor::applyAdaptiveThreshold(const QImage &sourceImage)
{
    try {
        cv::Mat src = qImageToMat(sourceImage);
        
        // Convert to grayscale
        cv::Mat gray;
        if (src.channels() == 3 || src.channels() == 4) {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = src.clone();
        }
        
        // Apply adaptive threshold
        cv::Mat binary;
        cv::adaptiveThreshold(gray, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, 
                            cv::THRESH_BINARY, 11, 2);
        
        // Convert back to color for display
        cv::Mat result;
        cv::cvtColor(binary, result, cv::COLOR_GRAY2BGR);
        
        return matToQImage(result);
    } catch (const cv::Exception &e) {
        qWarning() << "OpenCV exception when applying adaptive threshold: " << e.what();
        return sourceImage;
    }
} 