#include "resultsmanager.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QStandardPaths>
#include <QProcess>

ResultsManager::ResultsManager(QObject *parent)
    : QObject(parent)
    , m_isSessionActive(false)
    , m_defaultSaveLocation(QDir::homePath() + "/FiberInspector/Results")
    , m_autoSaveEnabled(false)
{
    // Create default save directory if it doesn't exist
    QDir dir(m_defaultSaveLocation);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

ResultsManager::~ResultsManager()
{
    if (m_isSessionActive) {
        endSession();
    }
}

bool ResultsManager::saveResult(const FiberAnalysisResult &result, const QString &imagePath)
{
    QString filename = generateResultFilename(m_defaultSaveLocation);
    return saveResultAs(result, filename);
}

bool ResultsManager::saveResultAs(const FiberAnalysisResult &result, const QString &filePath)
{
    // Convert result to JSON and save
    QJsonObject resultJson = resultToJson(result);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(resultJson);
    file.write(doc.toJson());
    file.close();
    
    // If we're in a session, add this result to the session
    if (m_isSessionActive) {
        addToSession(result, filePath);
    }
    
    return true;
}

FiberAnalysisResult ResultsManager::loadResult(const QString &filePath)
{
    FiberAnalysisResult result;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return result;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return result;
    }
    
    QJsonObject obj = doc.object();
    return jsonToResult(obj);
}

void ResultsManager::startNewSession(const QString &operatorName)
{
    // End current session if active
    if (m_isSessionActive) {
        endSession();
    }
    
    // Initialize new session
    m_currentSession.timestamp = QDateTime::currentDateTime();
    m_currentSession.operatorName = operatorName;
    m_currentSession.notes = "";
    m_isSessionActive = true;
    
    qDebug() << "New session started by operator:" << operatorName;
}

void ResultsManager::endSession()
{
    if (!m_isSessionActive) {
        return;
    }
    
    // Add current session to history
    m_sessionHistory.append(m_currentSession);
    
    // Reset current session
    m_currentSession = AnalysisSession();
    m_isSessionActive = false;
    
    qDebug() << "Session ended, total sessions in history:" << m_sessionHistory.size();
}

void ResultsManager::addToSession(const FiberAnalysisResult &result, const QString &imagePath)
{
    if (!m_isSessionActive) {
        qWarning() << "No active session to add result to";
        return;
    }
    
    m_currentSession.imagePath = imagePath;
    m_currentSession.result = result;
    
    qDebug() << "Added result to session, image path:" << imagePath;
}

QVector<AnalysisSession> ResultsManager::getSessionHistory()
{
    return m_sessionHistory;
}

bool ResultsManager::exportToPDF(const FiberAnalysisResult &result, const QString &filePath)
{
    // In a real implementation, this would use a PDF generation library
    // For this demonstration, we'll just create a simple text file
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open PDF file for writing:" << filePath;
        return false;
    }
    
    QTextStream out(&file);
    out << "FIBER INSPECTION REPORT\n";
    out << "=====================\n\n";
    out << "Date: " << QDateTime::currentDateTime().toString() << "\n\n";
    out << "ANALYSIS RESULTS\n";
    out << "---------------\n";
    out << "Quality Score: " << result.overallQuality << "\n";
    out << "Core-Clad Ratio: " << result.coreCladRatio << "\n";
    out << "Concentricity: " << result.concentricity << "\n";
    out << "Defects found: " << result.defects.size() << "\n\n";
    
    out << "DEFECT DETAILS\n";
    out << "-------------\n";
    for (int i = 0; i < result.defects.size(); ++i) {
        const FiberDefect &defect = result.defects[i];
        out << (i + 1) << ". " << defect.description 
            << " (Severity: " << defect.severity << ")\n";
    }
    
    out << "\nSUMMARY\n";
    out << "-------\n";
    out << result.summary << "\n";
    
    file.close();
    
    qDebug() << "Exported PDF report to:" << filePath;
    return true;
}

bool ResultsManager::exportToCSV(const QVector<FiberAnalysisResult> &results, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open CSV file for writing:" << filePath;
        return false;
    }
    
    QTextStream out(&file);
    
    // Write CSV header
    out << "Result ID,Date,Quality Score,Core-Clad Ratio,Concentricity,Defect Count,Is Acceptable\n";
    
    // Write each result as a row
    for (int i = 0; i < results.size(); ++i) {
        const FiberAnalysisResult &result = results[i];
        out << i << ","
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << ","
            << result.overallQuality << ","
            << result.coreCladRatio << ","
            << result.concentricity << ","
            << result.defects.size() << ","
            << (result.isAcceptable ? "Yes" : "No") << "\n";
    }
    
    file.close();
    
    qDebug() << "Exported CSV report with" << results.size() << "results to:" << filePath;
    return true;
}

bool ResultsManager::exportToJSON(const FiberAnalysisResult &result, const QString &filePath)
{
    QJsonObject resultJson = resultToJson(result);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open JSON file for writing:" << filePath;
        return false;
    }
    
    QJsonDocument doc(resultJson);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    qDebug() << "Exported JSON report to:" << filePath;
    return true;
}

bool ResultsManager::backupToSystemLocation()
{
#ifdef Q_OS_LINUX
    // In a real implementation, this would backup to a system-defined location
    QString backupDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Backups";
    
    // Create directory if it doesn't exist
    QDir dir(backupDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create backup directory:" << backupDir;
            return false;
        }
    }
    
    // Create backup filename
    QString backupFile = backupDir + "/backup_" + 
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".json";
    
    // Get all session results and save them
    QJsonArray sessionsArray;
    for (const auto &session : m_sessionHistory) {
        QJsonObject sessionObj;
        sessionObj["operator"] = session.operatorName;
        sessionObj["timestamp"] = session.timestamp.toString(Qt::ISODate);
        sessionObj["image_path"] = session.imagePath;
        sessionObj["notes"] = session.notes;
        sessionObj["result"] = resultToJson(session.result);
        
        sessionsArray.append(sessionObj);
    }
    
    QJsonObject backupObj;
    backupObj["sessions"] = sessionsArray;
    
    QFile file(backupFile);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open backup file for writing:" << backupFile;
        return false;
    }
    
    QJsonDocument doc(backupObj);
    file.write(doc.toJson());
    file.close();
    
    qDebug() << "Created system backup at:" << backupFile;
    return true;
#else
    qWarning() << "System backup is only implemented for Linux";
    return false;
#endif
}

bool ResultsManager::saveToMountedDevice(const QString &mountPoint)
{
#ifdef Q_OS_LINUX
    // Check if the mount point exists
    QDir mountDir(mountPoint);
    if (!mountDir.exists()) {
        qWarning() << "Mount point does not exist:" << mountPoint;
        return false;
    }
    
    // Check if the device is writable
    if (!checkFilePermissions(mountPoint)) {
        qWarning() << "Mount point is not writable:" << mountPoint;
        return false;
    }
    
    // Create a directory on the device
    QString targetDir = mountPoint + "/FiberInspectorData";
    QDir dir(targetDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory on mounted device:" << targetDir;
            return false;
        }
    }
    
    // Generate a filename for the export
    QString exportFile = targetDir + "/export_" + 
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".json";
    
    // Export all session data
    QJsonArray sessionsArray;
    for (const auto &session : m_sessionHistory) {
        QJsonObject sessionObj;
        sessionObj["operator"] = session.operatorName;
        sessionObj["timestamp"] = session.timestamp.toString(Qt::ISODate);
        sessionObj["image_path"] = session.imagePath;
        sessionObj["notes"] = session.notes;
        sessionObj["result"] = resultToJson(session.result);
        
        sessionsArray.append(sessionObj);
    }
    
    QJsonObject exportObj;
    exportObj["sessions"] = sessionsArray;
    
    QFile file(exportFile);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open export file for writing:" << exportFile;
        return false;
    }
    
    QJsonDocument doc(exportObj);
    file.write(doc.toJson());
    file.close();
    
    qDebug() << "Exported data to mounted device:" << exportFile;
    return true;
#else
    qWarning() << "Saving to mounted device is only implemented for Linux";
    return false;
#endif
}

void ResultsManager::setDefaultSaveLocation(const QString &location)
{
    m_defaultSaveLocation = location;
    
    // Create directory if it doesn't exist
    QDir dir(m_defaultSaveLocation);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString ResultsManager::getDefaultSaveLocation() const
{
    return m_defaultSaveLocation;
}

void ResultsManager::setAutoSave(bool enabled)
{
    m_autoSaveEnabled = enabled;
}

bool ResultsManager::isAutoSaveEnabled() const
{
    return m_autoSaveEnabled;
}

QString ResultsManager::generateResultFilename(const QString &basePath)
{
    // Create a filename based on current date/time
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString filename = basePath + "/fiber_analysis_" + timestamp + ".fir";
    
    return filename;
}

QJsonObject ResultsManager::resultToJson(const FiberAnalysisResult &result)
{
    QJsonObject resultObj;
    
    // Add basic properties
    resultObj["is_acceptable"] = result.isAcceptable;
    resultObj["core_clad_ratio"] = result.coreCladRatio;
    resultObj["concentricity"] = result.concentricity;
    resultObj["overall_quality"] = result.overallQuality;
    resultObj["summary"] = result.summary;
    
    // Convert defects to JSON array
    QJsonArray defectsArray;
    for (const auto &defect : result.defects) {
        QJsonObject defectObj;
        defectObj["type"] = static_cast<int>(defect.type);
        
        QJsonObject boundingBoxObj;
        boundingBoxObj["x"] = defect.boundingBox.x();
        boundingBoxObj["y"] = defect.boundingBox.y();
        boundingBoxObj["width"] = defect.boundingBox.width();
        boundingBoxObj["height"] = defect.boundingBox.height();
        
        defectObj["bounding_box"] = boundingBoxObj;
        defectObj["severity"] = defect.severity;
        defectObj["description"] = defect.description;
        
        defectsArray.append(defectObj);
    }
    
    resultObj["defects"] = defectsArray;
    
    return resultObj;
}

FiberAnalysisResult ResultsManager::jsonToResult(const QJsonObject &json)
{
    FiberAnalysisResult result;
    
    // Extract basic properties
    result.isAcceptable = json["is_acceptable"].toBool();
    result.coreCladRatio = json["core_clad_ratio"].toDouble();
    result.concentricity = json["concentricity"].toDouble();
    result.overallQuality = json["overall_quality"].toDouble();
    result.summary = json["summary"].toString();
    
    // Extract defects from JSON array
    QJsonArray defectsArray = json["defects"].toArray();
    for (const auto &defectValue : defectsArray) {
        QJsonObject defectObj = defectValue.toObject();
        
        FiberDefect defect;
        defect.type = static_cast<FiberDefect::DefectType>(defectObj["type"].toInt());
        
        QJsonObject boundingBoxObj = defectObj["bounding_box"].toObject();
        defect.boundingBox = QRect(
            boundingBoxObj["x"].toInt(),
            boundingBoxObj["y"].toInt(),
            boundingBoxObj["width"].toInt(),
            boundingBoxObj["height"].toInt()
        );
        
        defect.severity = defectObj["severity"].toDouble();
        defect.description = defectObj["description"].toString();
        
        result.defects.append(defect);
    }
    
    return result;
}

bool ResultsManager::checkFilePermissions(const QString &filePath)
{
    // Check if we can write to the specified path
    QFileInfo fileInfo(filePath);
    return fileInfo.isWritable();
} 