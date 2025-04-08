#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QImageReader>
#include <QImageWriter>
#include <QScrollBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressDialog>
#include <QThread>
#include <QSettings>
#include <QDebug>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFile>

// Linux-specific includes
#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_zoomFactor(1.0)
    , m_isLiveMode(false)
{
    ui->setupUi(this);
    
    // Create core components
    m_imageProcessor = new ImageProcessor();
    m_fiberAnalyzer = new FiberAnalyzer();
    m_resultsManager = new ResultsManager(this);
    
    // Initialize UI
    setupUi();
    createActions();
    createMenus();
    createToolbars();
    createStatusBar();
    
    // Load application settings
    loadSettings();
    
    // Set window properties
    setWindowTitle(tr("Fiber Inspector"));
    setMinimumSize(800, 600);
    resize(1024, 768);
    
    // Check system resources on startup
    checkSystemResources();
    
    // Connect to Linux system info if available
    connectToLinuxSystemInfo();
}

MainWindow::~MainWindow()
{
    // Save settings before closing
    saveSettings();
    
    delete m_imageProcessor;
    delete m_fiberAnalyzer;
    delete ui;
}

void MainWindow::setupUi()
{
    // Create central widget with layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create image display area
    m_scrollArea = new QScrollArea(centralWidget);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    
    m_imageLabel = new QLabel(m_scrollArea);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);
    m_scrollArea->setWidget(m_imageLabel);
    
    // Create controls layout
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    // Create filter selection
    QGroupBox *filterGroupBox = new QGroupBox(tr("Image Filters"));
    QVBoxLayout *filterLayout = new QVBoxLayout(filterGroupBox);
    
    m_filterComboBox = new QComboBox(filterGroupBox);
    m_filterComboBox->addItem(tr("No Filter"), static_cast<int>(FilterType::None));
    m_filterComboBox->addItem(tr("Grayscale"), static_cast<int>(FilterType::Grayscale));
    m_filterComboBox->addItem(tr("Threshold"), static_cast<int>(FilterType::Threshold));
    m_filterComboBox->addItem(tr("Edge Detection"), static_cast<int>(FilterType::EdgeDetection));
    m_filterComboBox->addItem(tr("Sharpen"), static_cast<int>(FilterType::Sharpen));
    m_filterComboBox->addItem(tr("Median Blur"), static_cast<int>(FilterType::MedianBlur));
    m_filterComboBox->addItem(tr("Gaussian Blur"), static_cast<int>(FilterType::GaussianBlur));
    
    filterLayout->addWidget(m_filterComboBox);
    
    // Create adjustment controls
    QGroupBox *adjustmentGroupBox = new QGroupBox(tr("Adjustments"));
    QVBoxLayout *adjustmentLayout = new QVBoxLayout(adjustmentGroupBox);
    
    QLabel *brightnessLabel = new QLabel(tr("Brightness:"));
    m_brightnessSlider = new QSlider(Qt::Horizontal, adjustmentGroupBox);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(0);
    
    QLabel *contrastLabel = new QLabel(tr("Contrast:"));
    m_contrastSlider = new QSlider(Qt::Horizontal, adjustmentGroupBox);
    m_contrastSlider->setRange(-100, 100);
    m_contrastSlider->setValue(0);
    
    adjustmentLayout->addWidget(brightnessLabel);
    adjustmentLayout->addWidget(m_brightnessSlider);
    adjustmentLayout->addWidget(contrastLabel);
    adjustmentLayout->addWidget(m_contrastSlider);
    
    // Create analyze button
    QGroupBox *analyzeGroupBox = new QGroupBox(tr("Analysis"));
    QVBoxLayout *analyzeLayout = new QVBoxLayout(analyzeGroupBox);
    
    m_analyzeButton = new QPushButton(tr("Analyze Fiber"), analyzeGroupBox);
    m_analyzeButton->setEnabled(false);
    
    analyzeLayout->addWidget(m_analyzeButton);
    
    // Add all controls to layout
    controlsLayout->addWidget(filterGroupBox);
    controlsLayout->addWidget(adjustmentGroupBox);
    controlsLayout->addWidget(analyzeGroupBox);
    
    // Add layouts to main layout
    mainLayout->addWidget(m_scrollArea);
    mainLayout->addLayout(controlsLayout);
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Connect signals and slots
    connect(m_filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::applyFilter);
    connect(m_brightnessSlider, &QSlider::valueChanged, 
            this, &MainWindow::adjustBrightness);
    connect(m_contrastSlider, &QSlider::valueChanged, 
            this, &MainWindow::adjustContrast);
    connect(m_analyzeButton, &QPushButton::clicked, 
            this, &MainWindow::analyzeFiber);
}

void MainWindow::createActions()
{
    // File actions
    ui->actionOpen = new QAction(tr("&Open..."), this);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openImage);
    
    ui->actionSave = new QAction(tr("&Save Results..."), this);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSave->setEnabled(false);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveResults);
    
    ui->actionExport = new QAction(tr("&Export Report..."), this);
    ui->actionExport->setEnabled(false);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::exportReport);
    
    ui->actionExit = new QAction(tr("E&xit"), this);
    ui->actionExit->setShortcut(QKeySequence::Quit);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    
    // View actions
    ui->actionZoomIn = new QAction(tr("Zoom &In"), this);
    ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(ui->actionZoomIn, &QAction::triggered, this, &MainWindow::zoomIn);
    
    ui->actionZoomOut = new QAction(tr("Zoom &Out"), this);
    ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(ui->actionZoomOut, &QAction::triggered, this, &MainWindow::zoomOut);
    
    ui->actionResetView = new QAction(tr("&Reset View"), this);
    connect(ui->actionResetView, &QAction::triggered, this, &MainWindow::resetView);
    
    // Tools actions
    ui->actionAnalyze = new QAction(tr("&Analyze Fiber"), this);
    ui->actionAnalyze->setEnabled(false);
    connect(ui->actionAnalyze, &QAction::triggered, this, &MainWindow::analyzeFiber);
    
    ui->actionLiveMode = new QAction(tr("&Live Mode"), this);
    ui->actionLiveMode->setCheckable(true);
    connect(ui->actionLiveMode, &QAction::triggered, this, &MainWindow::toggleLiveMode);
    
    // Settings actions
    ui->actionSettings = new QAction(tr("&Settings..."), this);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showSettings);
    
    // Help actions
    ui->actionAbout = new QAction(tr("&About"), this);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
    // File menu
    ui->menuFile = menuBar()->addMenu(tr("&File"));
    ui->menuFile->addAction(ui->actionOpen);
    ui->menuFile->addAction(ui->actionSave);
    ui->menuFile->addAction(ui->actionExport);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(ui->actionExit);
    
    // View menu
    ui->menuView = menuBar()->addMenu(tr("&View"));
    ui->menuView->addAction(ui->actionZoomIn);
    ui->menuView->addAction(ui->actionZoomOut);
    ui->menuView->addAction(ui->actionResetView);
    
    // Tools menu
    ui->menuTools = menuBar()->addMenu(tr("&Tools"));
    ui->menuTools->addAction(ui->actionAnalyze);
    ui->menuTools->addAction(ui->actionLiveMode);
    
    // Settings menu
    ui->menuSettings = menuBar()->addMenu(tr("&Settings"));
    ui->menuSettings->addAction(ui->actionSettings);
    
    // Help menu
    ui->menuHelp = menuBar()->addMenu(tr("&Help"));
    ui->menuHelp->addAction(ui->actionAbout);
}

void MainWindow::createToolbars()
{
    // File toolbar
    ui->fileToolBar = addToolBar(tr("File"));
    ui->fileToolBar->addAction(ui->actionOpen);
    ui->fileToolBar->addAction(ui->actionSave);
    
    // Edit toolbar
    ui->editToolBar = addToolBar(tr("Edit"));
    ui->editToolBar->addAction(ui->actionZoomIn);
    ui->editToolBar->addAction(ui->actionZoomOut);
    
    // Tools toolbar
    ui->toolsToolBar = addToolBar(tr("Tools"));
    ui->toolsToolBar->addAction(ui->actionAnalyze);
    ui->toolsToolBar->addAction(ui->actionLiveMode);
}

void MainWindow::createStatusBar()
{
    // Create status bar with progress indicator
    m_progressBar = new QProgressBar(statusBar());
    m_progressBar->setMaximumWidth(150);
    m_progressBar->setVisible(false);
    
    statusBar()->addPermanentWidget(m_progressBar);
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::openImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Image"),
        QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    
    if (!filePath.isEmpty()) {
        if (m_imageProcessor->loadImage(filePath)) {
            m_currentFilePath = filePath;
            m_currentImage = QImage(filePath);
            m_processedImage = m_currentImage;
            
            updateImageDisplay();
            
            // Enable analysis and save buttons
            m_analyzeButton->setEnabled(true);
            ui->actionAnalyze->setEnabled(true);
            ui->actionSave->setEnabled(false); // Will be enabled after analysis
            
            // Reset sliders
            m_brightnessSlider->setValue(0);
            m_contrastSlider->setValue(0);
            
            statusBar()->showMessage(tr("Image loaded: %1").arg(QFileInfo(filePath).fileName()));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not load image: %1").arg(filePath));
        }
    }
}

void MainWindow::saveResults()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Results"),
        m_resultsManager->getDefaultSaveLocation(), tr("Result Files (*.fir);;All Files (*)"));
    
    if (!filePath.isEmpty()) {
        bool success = m_resultsManager->saveResultAs(m_fiberAnalyzer->analyzeImage(m_processedImage), filePath);
        if (success) {
            statusBar()->showMessage(tr("Results saved to: %1").arg(QFileInfo(filePath).fileName()), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not save results to: %1").arg(filePath));
        }
    }
}

void MainWindow::applyFilter(int filterIndex)
{
    if (m_currentImage.isNull()) {
        return;
    }
    
    FilterType filterType = static_cast<FilterType>(m_filterComboBox->itemData(filterIndex).toInt());
    
    m_processedImage = m_imageProcessor->applyFilter(m_currentImage, filterType);
    updateImageDisplay();
}

void MainWindow::analyzeFiber()
{
    if (m_processedImage.isNull()) {
        return;
    }
    
    statusBar()->showMessage(tr("Analyzing fiber..."));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    
    // Use a timer to update progress bar (in a real app, this would be tied to actual progress)
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        static int progress = 0;
        progress += 10;
        m_progressBar->setValue(progress);
        
        if (progress >= 100) {
            timer->stop();
            timer->deleteLater();
            
            // Perform the actual analysis
            FiberAnalysisResult result = m_fiberAnalyzer->analyzeImage(m_processedImage);
            
            // Display results
            updateResultsPanel();
            
            // Enable saving of results
            ui->actionSave->setEnabled(true);
            ui->actionExport->setEnabled(true);
            
            // Update status bar
            m_progressBar->setVisible(false);
            statusBar()->showMessage(tr("Analysis complete. Found %1 defects.").arg(result.defects.size()), 5000);
        }
    });
    
    timer->start(100);
}

void MainWindow::adjustBrightness(int value)
{
    if (m_currentImage.isNull()) {
        return;
    }
    
    // Apply both brightness and contrast adjustments
    QImage adjusted = m_imageProcessor->adjustBrightness(m_currentImage, value);
    adjusted = m_imageProcessor->adjustContrast(adjusted, m_contrastSlider->value());
    
    // Apply the current filter on top of adjustments
    FilterType filterType = static_cast<FilterType>(m_filterComboBox->itemData(m_filterComboBox->currentIndex()).toInt());
    m_processedImage = m_imageProcessor->applyFilter(adjusted, filterType);
    
    updateImageDisplay();
}

void MainWindow::adjustContrast(int value)
{
    if (m_currentImage.isNull()) {
        return;
    }
    
    // Apply both brightness and contrast adjustments
    QImage adjusted = m_imageProcessor->adjustContrast(m_currentImage, value);
    adjusted = m_imageProcessor->adjustBrightness(adjusted, m_brightnessSlider->value());
    
    // Apply the current filter on top of adjustments
    FilterType filterType = static_cast<FilterType>(m_filterComboBox->itemData(m_filterComboBox->currentIndex()).toInt());
    m_processedImage = m_imageProcessor->applyFilter(adjusted, filterType);
    
    updateImageDisplay();
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::resetView()
{
    m_zoomFactor = 1.0;
    updateImageDisplay();
}

void MainWindow::toggleLiveMode()
{
    m_isLiveMode = !m_isLiveMode;
    
    if (m_isLiveMode) {
        statusBar()->showMessage(tr("Live mode activated"));
        // In a real app, this would connect to a camera feed
    } else {
        statusBar()->showMessage(tr("Live mode deactivated"));
    }
}

void MainWindow::exportReport()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Report"),
        QDir::homePath(), tr("PDF Files (*.pdf);;CSV Files (*.csv);;JSON Files (*.json)"));
    
    if (!filePath.isEmpty()) {
        QString extension = QFileInfo(filePath).suffix().toLower();
        bool success = false;
        
        FiberAnalysisResult result = m_fiberAnalyzer->analyzeImage(m_processedImage);
        
        if (extension == "pdf") {
            success = m_resultsManager->exportToPDF(result, filePath);
        } else if (extension == "csv") {
            QVector<FiberAnalysisResult> results;
            results.append(result);
            success = m_resultsManager->exportToCSV(results, filePath);
        } else if (extension == "json") {
            success = m_resultsManager->exportToJSON(result, filePath);
        }
        
        if (success) {
            statusBar()->showMessage(tr("Report exported to: %1").arg(QFileInfo(filePath).fileName()), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not export report to: %1").arg(filePath));
        }
    }
}

void MainWindow::showSettings()
{
    QMessageBox::information(this, tr("Settings"), tr("Settings dialog would be shown here."));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Fiber Inspector"),
        tr("<h2>Fiber Inspector</h2>"
           "<p>Version 1.0.0</p>"
           "<p>A demonstration application for fiber inspection and analysis.</p>"
           "<p>Uses Qt %1 and OpenCV for image processing.</p>"
           "<p>Designed for Linux systems with hardware acceleration support.</p>")
           .arg(QT_VERSION_STR));
}

void MainWindow::updateImageDisplay()
{
    if (m_processedImage.isNull()) {
        return;
    }
    
    // Scale the image according to zoom factor
    QPixmap pixmap = QPixmap::fromImage(m_processedImage);
    
    if (m_zoomFactor != 1.0) {
        QSize scaledSize = m_processedImage.size() * m_zoomFactor;
        pixmap = pixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    m_imageLabel->setPixmap(pixmap);
    m_imageLabel->resize(pixmap.size());
}

void MainWindow::scaleImage(double factor)
{
    m_zoomFactor *= factor;
    
    // Adjust the image label size
    QSize newSize = m_processedImage.size() * m_zoomFactor;
    m_imageLabel->resize(newSize);
    
    // Create a scaled version of the image
    QPixmap scaledPixmap = QPixmap::fromImage(m_processedImage)
                          .scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Set the scaled image to the label
    m_imageLabel->setPixmap(scaledPixmap);
    
    // Adjust scrollbars
    adjustScrollBar(m_scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(m_scrollArea->verticalScrollBar(), factor);
    
    // Update zoom actions
    ui->actionZoomIn->setEnabled(m_zoomFactor < 3.0);
    ui->actionZoomOut->setEnabled(m_zoomFactor > 0.333);
    
    // Update status bar with zoom info
    statusBar()->showMessage(tr("Zoom: %1%").arg(m_zoomFactor * 100, 0, 'f', 0));
}

// Helper method to adjust scrollbars
void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}

void MainWindow::loadSettings()
{
    QSettings settings;
    
    // Window geometry
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    // Default save location
    m_resultsManager->setDefaultSaveLocation(
        settings.value("defaultSaveLocation", QDir::homePath()).toString());
    
    // Auto-save setting
    m_resultsManager->setAutoSave(
        settings.value("autoSave", false).toBool());
}

void MainWindow::saveSettings()
{
    QSettings settings;
    
    // Window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    
    // Other settings would be saved here
}

void MainWindow::updateResultsPanel()
{
    // This would create or update a results panel with analysis data
    // For demonstration purposes, we'll just show a message
    QMessageBox::information(this, tr("Analysis Results"), 
        tr("In a complete application, this would show a detailed results panel."));
}

bool MainWindow::checkSystemResources()
{
#ifdef Q_OS_LINUX
    // Check system memory
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long totalRam = info.totalram * info.mem_unit;
        unsigned long freeRam = info.freeram * info.mem_unit;
        
        // Convert to MB for display
        double totalRamMB = totalRam / (1024.0 * 1024.0);
        double freeRamMB = freeRam / (1024.0 * 1024.0);
        
        qDebug() << "Memory: Total:" << totalRamMB << "MB, Free:" << freeRamMB << "MB";
        
        // Check if we have enough RAM
        if (freeRamMB < 500) {
            QMessageBox::warning(this, tr("Low Memory Warning"),
                tr("System is low on memory (%.1f MB). Application may perform slowly.").arg(freeRamMB));
            return false;
        }
    }
    
    // Check disk space
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long diskTotal = stat.f_blocks * stat.f_frsize;
        unsigned long diskFree = stat.f_bavail * stat.f_frsize;
        
        // Convert to GB for display
        double diskTotalGB = diskTotal / (1024.0 * 1024.0 * 1024.0);
        double diskFreeGB = diskFree / (1024.0 * 1024.0 * 1024.0);
        
        qDebug() << "Disk: Total:" << diskTotalGB << "GB, Free:" << diskFreeGB << "GB";
        
        // Check if we have enough disk space
        if (diskFreeGB < 1.0) {
            QMessageBox::warning(this, tr("Low Disk Space Warning"),
                tr("System is low on disk space (%.1f GB). Save operations may fail.").arg(diskFreeGB));
            return false;
        }
    }
#endif
    
    return true;
}

void MainWindow::connectToLinuxSystemInfo()
{
#ifdef Q_OS_LINUX
    // In a real application, this could set up connections to system monitoring tools
    // For example, connecting to D-Bus for hardware events, etc.
    qDebug() << "Connected to Linux system monitoring";
#endif
}

void MainWindow::loadImage(const QString &imagePath)
{
    if (imagePath.isEmpty()) {
        return;
    }
    
    if (!QFile::exists(imagePath)) {
        QMessageBox::warning(this, tr("Image not found"),
                            tr("The specified image file does not exist: %1").arg(imagePath));
        return;
    }
    
    // Load the image
    QImage image(imagePath);
    if (image.isNull()) {
        QMessageBox::warning(this, tr("Invalid Image"),
                            tr("The specified file is not a valid image: %1").arg(imagePath));
        return;
    }
    
    // Store image and update display
    m_currentImage = image;
    m_processedImage = image;
    m_currentFilePath = imagePath;
    
    // Reset UI elements
    m_filterComboBox->setCurrentIndex(0);
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(0);
    
    // Enable analysis button
    m_analyzeButton->setEnabled(true);
    
    // Update the display
    updateImageDisplay();
    
    // Update window title
    QFileInfo fileInfo(imagePath);
    setWindowTitle(tr("Fiber Inspector - %1").arg(fileInfo.fileName()));
    
    // Update status bar
    statusBar()->showMessage(tr("Loaded image: %1 (%2x%3)")
                           .arg(fileInfo.fileName())
                           .arg(image.width())
                           .arg(image.height()));
} 