#pragma once
/**
 * @file plugin_manager.h
 * @brief Lua Plugin Manager for BummerGram
 * 
 * Manages loading, executing, and unloading of Lua scripts
 * placed in the plugins directory.
 */

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <memory>

namespace BummerGram {

class LuaState;
class LuaSandbox;

/**
 * @struct PluginInfo
 * @brief Information about a loaded plugin
 */
struct PluginInfo {
    QString name;
    QString filePath;
    QString version;
    QString author;
    QString description;
    bool enabled;
    bool loaded;
    bool hasError;
    QString errorMessage;
    qint64 loadTime;
};

/**
 * @class PluginManager
 * @brief Manages the Lua plugin lifecycle
 */
class PluginManager : public QObject {
    Q_OBJECT

public:
    // ========================================================================
    // Singleton Access
    // ========================================================================
    
    static PluginManager& instance();
    
    // Prevent copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    // ========================================================================
    // Plugin Lifecycle
    // ========================================================================
    
    /**
     * @brief Initialize the plugin system
     */
    void initialize();
    
    /**
     * @brief Load all plugins from the plugins directory
     */
    void loadAll();
    
    /**
     * @brief Load a specific plugin
     * @param filePath Path to the plugin file
     * @return true if loaded successfully
     */
    bool loadPlugin(const QString& filePath);
    
    /**
     * @brief Unload a specific plugin
     * @param name Plugin name
     */
    void unloadPlugin(const QString& name);
    
    /**
     * @brief Unload all plugins
     */
    void unloadAll();
    
    /**
     * @brief Reload a specific plugin
     * @param name Plugin name
     */
    void reloadPlugin(const QString& name);
    
    /**
     * @brief Reload all plugins
     */
    void reloadAll();

    // ========================================================================
    // Plugin Information
    // ========================================================================
    
    /**
     * @brief Get list of all available plugins
     */
    QStringList availablePlugins() const;
    
    /**
     * @brief Get list of loaded plugins
     */
    QStringList loadedPlugins() const;
    
    /**
     * @brief Get information about a plugin
     * @param name Plugin name
     * @return PluginInfo struct
     */
    PluginInfo getPluginInfo(const QString& name) const;
    
    /**
     * @brief Get all plugin information
     */
    QMap<QString, PluginInfo> allPlugins() const;

    // ========================================================================
    // Plugin Directory
    // ========================================================================
    
    /**
     * @brief Get the plugins directory path
     */
    QString pluginsDirectory() const;
    
    /**
     * @brief Scan directory for new plugins
     */
    QStringList scanForPlugins() const;

    // ========================================================================
    // Execution
    // ========================================================================
    
    /**
     * @brief Execute a Lua function in a plugin
     * @param pluginName Plugin name
     * @param functionName Function name
     * @param args Arguments to pass
     * @return true if executed successfully
     */
    bool executeFunction(const QString& pluginName, 
                        const QString& functionName,
                        const QStringList& args = {});
    
    /**
     * @brief Save current plugin state
     */
    void saveState();
    
    /**
     * @brief Restore plugin state
     */
    void restoreState();

signals:
    /**
     * @brief Emitted when a plugin is loaded
     */
    void pluginLoaded(const QString& name);
    
    /**
     * @brief Emitted when a plugin is unloaded
     */
    void pluginUnloaded(const QString& name);
    
    /**
     * @brief Emitted when a plugin error occurs
     */
    void pluginError(const QString& name, const QString& error);
    
    /**
     * @brief Emitted when all plugins are reloaded
     */
    void pluginsReloaded();
    
    /**
     * @brief Emitted during initialization progress
     */
    void initializationProgress(const QString& status, int percent);

private:
    /**
     * @brief Private constructor for singleton
     */
    explicit PluginManager(QObject* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~PluginManager() override;

    // ========================================================================
    // Private Methods
    // ========================================================================
    
    void createLuaState();
    void registerSafeWrappers();
    void setSandboxRestrictions();
    bool validatePlugin(const QString& filePath);
    
    // ========================================================================
    // Member Variables
    // ========================================================================
    
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
