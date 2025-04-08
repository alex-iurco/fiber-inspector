#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QSlider>
#include <QComboBox>
#include <QPushButton>
#include <QStatusBar>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "imageprocessor.h"
#include "fiberanalyzer.h"
#include "resultsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveResults();
    void applyFilter(int filterIndex);
    void analyzeFiber();
    void adjustBrightness(int value);
    void adjustContrast(int value);
    void zoomIn();
    void zoomOut();
    void resetView();
    void toggleLiveMode();
    void exportReport();
    void showSettings();
    void about();

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolbars();
    void createStatusBar();
    void updateImageDisplay();
    void loadSettings();
    void saveSettings();
    void updateResultsPanel();
    bool checkSystemResources();
    void connectToLinuxSystemInfo();

    Ui::MainWindow *ui;
    ImageProcessor *m_imageProcessor;
    FiberAnalyzer *m_fiberAnalyzer;
    ResultsManager *m_resultsManager;
    
    QImage m_currentImage;
    QImage m_processedImage;
    QLabel *m_imageLabel;
    QScrollArea *m_scrollArea;
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QComboBox *m_filterComboBox;
    QPushButton *m_analyzeButton;
    QProgressBar *m_progressBar;
    
    double m_zoomFactor;
    bool m_isLiveMode;
    QString m_currentFilePath;
};

#endif // MAINWINDOW_H 