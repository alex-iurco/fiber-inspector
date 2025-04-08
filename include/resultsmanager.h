#ifndef RESULTSMANAGER_H
#define RESULTSMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "fiberanalyzer.h"

// Structure to hold analysis session info
struct AnalysisSession {
    QDateTime timestamp;
    QString imagePath;
    QString operatorName;
    FiberAnalysisResult result;
    QString notes;
};

class ResultsManager : public QObject
{
    Q_OBJECT

public:
    explicit ResultsManager(QObject *parent = nullptr);
    ~ResultsManager();
    
    // Save/load results
    bool saveResult(const FiberAnalysisResult &result, const QString &imagePath);
    bool saveResultAs(const FiberAnalysisResult &result, const QString &filePath);
    FiberAnalysisResult loadResult(const QString &filePath);
    
    // Session management
    void startNewSession(const QString &operatorName);
    void endSession();
    void addToSession(const FiberAnalysisResult &result, const QString &imagePath);
    QVector<AnalysisSession> getSessionHistory();
    
    // Export options
    bool exportToPDF(const FiberAnalysisResult &result, const QString &filePath);
    bool exportToCSV(const QVector<FiberAnalysisResult> &results, const QString &filePath);
    bool exportToJSON(const FiberAnalysisResult &result, const QString &filePath);
    
    // Linux system integration
    bool backupToSystemLocation();
    bool saveToMountedDevice(const QString &mountPoint);
    
    // Settings
    void setDefaultSaveLocation(const QString &location);
    QString getDefaultSaveLocation() const;
    void setAutoSave(bool enabled);
    bool isAutoSaveEnabled() const;

private:
    QVector<AnalysisSession> m_sessionHistory;
    AnalysisSession m_currentSession;
    bool m_isSessionActive;
    QString m_defaultSaveLocation;
    bool m_autoSaveEnabled;
    
    // Helper methods
    QString generateResultFilename(const QString &basePath);
    QJsonObject resultToJson(const FiberAnalysisResult &result);
    FiberAnalysisResult jsonToResult(const QJsonObject &json);
    bool checkFilePermissions(const QString &filePath);
};

#endif // RESULTSMANAGER_H 