#pragma once
/**
 * @file localstorage.h
 * @brief Isolated storage paths for BummerGram
 * 
 * This module ensures BummerGram stores all user data, cache, and
 * configuration in its own isolated directory structure, completely
 * separate from any official Telegram Desktop installation.
 */

#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <filesystem>

namespace BummerGram {

/**
 * @class LocalStorage
 * @brief Manages isolated file system paths for BummerGram
 * 
 * BummerGram uses its own dedicated directories for:
 * - Application data (%AppData%/BummerGram/)
 * - Configuration files
 * - User cache
 * - Plugin storage
 * - Log files
 * - Downloaded media
 */
class LocalStorage {
public:
    // =========================================================================
    // Path Accessors
    // =========================================================================
    
    /**
     * @brief Returns the base application data directory
     * Windows: %AppData%/BummerGram/
     * Linux: ~/.config/BummerGram/
     * macOS: ~/Library/Application Support/BummerGram/
     */
    static QString getAppDataPath() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    
    /**
     * @brief Returns the configuration directory
     */
    static QString getConfigPath() {
        return getAppDataPath() + "/config";
    }
    
    /**
     * @brief Returns the cache directory for temporary files
     */
    static QString getCachePath() {
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    }
    
    /**
     * @brief Returns the logs directory for application logs
     */
    static QString getLogsPath() {
        return getAppDataPath() + "/logs";
    }
    
    /**
     * @brief Returns the plugins directory for Lua scripts
     */
    static QString getPluginsPath() {
        return getAppDataPath() + "/plugins";
    }
    
    /**
     * @brief Returns the sounds directory for audio files
     */
    static QString getSoundsPath() {
        return getAppDataPath() + "/sounds";
    }
    
    /**
     * @brief Returns the downloads directory for saved media
     */
    static QString getDownloadsPath() {
        return getAppDataPath() + "/downloads";
    }
    
    /**
     * @brief Returns the session data directory
     */
    static QString getSessionsPath() {
        return getAppDataPath() + "/sessions";
    }
    
    // =========================================================================
    // Initialization
    // =========================================================================
    
    /**
     * @brief Creates all necessary directories if they don't exist
     */
    static void ensureDirectoriesExist() {
        QDir dir;
        
        // Create base directory
        dir.mkpath(getAppDataPath());
        
        // Create subdirectories
        dir.mkpath(getConfigPath());
        dir.mkpath(getCachePath());
        dir.mkpath(getLogsPath());
        dir.mkpath(getPluginsPath());
        dir.mkpath(getSoundsPath());
        dir.mkpath(getDownloadsPath());
        dir.mkpath(getSessionsPath());
    }
    
    /**
     * @brief Configures QSettings to use BummerGram's isolated paths
     */
    static void setAsDefaultSettingsLocation() {
        QCoreApplication::setApplicationName("BummerGram");
        QCoreApplication::setOrganizationName("BummerGram");
        QCoreApplication::setOrganizationDomain("bummergram.org");
        
        // Use INI format for cross-platform compatibility
        QSettings::setDefaultFormat(QSettings::IniFormat);
        
        // Set custom path for settings
        QSettings::setPath(
            QSettings::IniFormat,
            QSettings::UserScope,
            getConfigPath()
        );
    }
    
    /**
     * @brief Returns a QSettings object for application settings
     */
    static QSettings* getSettings() {
        static QSettings settings(
            getConfigPath() + "/bummergram.ini",
            QSettings::IniFormat
        );
        return &settings;
    }
    
    // =========================================================================
    // Path Validation
    // =========================================================================
    
    /**
     * @brief Checks if a path exists and is writable
     */
    static bool isPathWritable(const QString& path) {
        QFileInfo info(path);
        if (!info.exists()) {
            return QDir().mkpath(path);
        }
        return info.isWritable();
    }
    
    /**
     * @brief Gets the total size of the app data directory
     */
    static qint64 getAppDataSize() {
        std::filesystem::path path = getAppDataPath().toStdString();
        if (!std::filesystem::exists(path)) {
            return 0;
        }
        
        qint64 totalSize = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                totalSize += entry.file_size();
            }
        }
        return totalSize;
    }
    
    /**
     * @brief Formats bytes into human-readable string
     */
    static QString formatSize(qint64 bytes) {
        const QStringList suffixes = {"B", "KB", "MB", "GB", "TB"};
        int suffixIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && suffixIndex < suffixes.size() - 1) {
            size /= 1024;
            ++suffixIndex;
        }
        
        return QString::number(size, 'f', 2) + " " + suffixes[suffixIndex];
    }
    
    /**
     * @brief Clears the cache directory
     */
    static void clearCache() {
        QDir cacheDir(getCachePath());
        cacheDir.removeRecursively();
        cacheDir.mkpath(getCachePath());
    }
    
    /**
     * @brief Returns the current version string
     */
    static QString getVersionString() {
        return QString("%1.%2.%3")
            .arg(BUMMERGRAM_VERSION_MAJOR)
            .arg(BUMMERGRAM_VERSION_MINOR)
            .arg(BUMMERGRAM_VERSION_PATCH);
    }
};

} // namespace BummerGram

// Preprocessor shortcuts for version
#ifndef BUMMERGRAM_VERSION_MAJOR
#define BUMMERGRAM_VERSION_MAJOR 1
#endif

#ifndef BUMMERGRAM_VERSION_MINOR
#define BUMMERGRAM_VERSION_MINOR 0
#endif

#ifndef BUMMERGRAM_VERSION_PATCH
#define BUMMERGRAM_VERSION_PATCH 0
#endif
