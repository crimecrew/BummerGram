#include "plugin_manager.h"
#include "lua_state.h"
#include "lua_safe_wrappers.h"
#include "sandbox/lua_sandbox.h"
#include "../core/localstorage.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

namespace BummerGram {

// ============================================================================
// PluginInfo Implementation
// ============================================================================

// PluginInfo is a simple struct, defined in header

// ============================================================================
// Private Implementation
// ============================================================================

class PluginManager::Impl {
public:
    // Lua VM for each plugin (isolated)
    QMap<QString, std::unique_ptr<LuaState>> luaStates;
    
    // Plugin metadata
    QMap<QString, PluginInfo> pluginInfos;
    
    // Plugins directory
    QString pluginsDir;
    
    // Sandbox for security
    std::unique_ptr<LuaSandbox> sandbox;
    
    // Initialization state
    bool initialized = false;
    
    // Auto-reload timer
    QTimer* reloadTimer = nullptr;
};

} // namespace

// ============================================================================
// Singleton
// ============================================================================

PluginManager& PluginManager::instance() {
    static PluginManager manager;
    return manager;
}

// ============================================================================
// Construction
// ============================================================================

PluginManager::PluginManager(QObject* parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
    // Set plugins directory
    d->pluginsDir = LocalStorage::getPluginsPath();
}

PluginManager::~PluginManager() = default;

// ============================================================================
// Plugin Lifecycle
// ============================================================================

void PluginManager::initialize() {
    if (d->initialized) {
        return;
    }
    
    qDebug() << "[PluginManager] Initializing...";
    emit initializationProgress("Initializing plugin system...", 0);
    
    // Create sandbox
    d->sandbox = std::make_unique<LuaSandbox>();
    
    // Ensure plugins directory exists
    QDir dir(d->pluginsDir);
    if (!dir.exists()) {
        dir.mkpath(d->pluginsDir);
        
        // Create example plugin
        createExamplePlugin();
    }
    
    emit initializationProgress("Scanning for plugins...", 25);
    
    // Scan for plugins
    QStringList plugins = scanForPlugins();
    qDebug() << "[PluginManager] Found" << plugins.size() << "plugins";
    
    emit initializationProgress("Loading plugins...", 50);
    
    // Load all plugins
    loadAll();
    
    emit initializationProgress("Plugin system ready", 100);
    
    d->initialized = true;
    
    qDebug() << "[PluginManager] Initialization complete";
}

void PluginManager::loadAll() {
    qDebug() << "[PluginManager] Loading all plugins...";
    
    QStringList plugins = scanForPlugins();
    int total = plugins.size();
    int current = 0;
    
    for (const QString& pluginPath : plugins) {
        ++current;
        int progress = 25 + (current * 75 / total);
        emit initializationProgress(
            QString("Loading %1...").arg(QFileInfo(pluginPath).baseName()),
            progress
        );
        
        if (!loadPlugin(pluginPath)) {
            qWarning() << "[PluginManager] Failed to load:" << pluginPath;
        }
    }
    
    qDebug() << "[PluginManager] Loaded" << loadedPlugins().size() 
             << "plugins successfully";
}

bool PluginManager::loadPlugin(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString pluginName = fileInfo.baseName();
    
    // Check if already loaded
    if (d->luaStates.contains(pluginName)) {
        qDebug() << "[PluginManager] Plugin already loaded:" << pluginName;
        return true;
    }
    
    // Validate plugin
    if (!validatePlugin(filePath)) {
        qWarning() << "[PluginManager] Plugin validation failed:" << filePath;
        return false;
    }
    
    qDebug() << "[PluginManager] Loading plugin:" << pluginName;
    
    // Create Lua state
    auto luaState = std::make_unique<LuaState>(pluginName);
    
    // Register safe wrappers
    registerSafeWrappers();
    
    // Set sandbox restrictions
    setSandboxRestrictions();
    
    // Load the plugin script
    bool loaded = luaState->loadFile(filePath);
    
    if (loaded) {
        // Create plugin info
        PluginInfo info;
        info.name = pluginName;
        info.filePath = filePath;
        info.enabled = true;
        info.loaded = true;
        info.hasError = false;
        info.loadTime = QDateTime::currentMSecsSinceEpoch();
        
        // Try to get metadata from script
        QString version, author, description;
        if (luaState->getMetadata(version, author, description)) {
            info.version = version;
            info.author = author;
            info.description = description;
        } else {
            info.version = "1.0.0";
            info.author = "Unknown";
            info.description = "No description available";
        }
        
        // Store state and info
        d->luaStates[pluginName] = std::move(luaState);
        d->pluginInfos[pluginName] = info;
        
        // Execute onLoad if exists
        executeFunction(pluginName, "onLoad");
        
        emit pluginLoaded(pluginName);
        
        qDebug() << "[PluginManager] Plugin loaded successfully:" << pluginName;
        return true;
    } else {
        // Record error
        PluginInfo info;
        info.name = pluginName;
        info.filePath = filePath;
        info.loaded = false;
        info.hasError = true;
        info.errorMessage = "Failed to load Lua script";
        d->pluginInfos[pluginName] = info;
        
        emit pluginError(pluginName, info.errorMessage);
        
        return false;
    }
}

void PluginManager::unloadPlugin(const QString& name) {
    if (!d->luaStates.contains(name)) {
        return;
    }
    
    qDebug() << "[PluginManager] Unloading plugin:" << name;
    
    // Execute onUnload if exists
    executeFunction(name, "onUnload");
    
    // Remove Lua state
    d->luaStates.remove(name);
    
    // Update info
    if (d->pluginInfos.contains(name)) {
        d->pluginInfos[name].loaded = false;
        d->pluginInfos[name].enabled = false;
    }
    
    emit pluginUnloaded(name);
}

void PluginManager::unloadAll() {
    qDebug() << "[PluginManager] Unloading all plugins...";
    
    for (const QString& name : d->luaStates.keys()) {
        unloadPlugin(name);
    }
}

void PluginManager::reloadPlugin(const QString& name) {
    qDebug() << "[PluginManager] Reloading plugin:" << name;
    
    if (d->pluginInfos.contains(name)) {
        QString filePath = d->pluginInfos[name].filePath;
        unloadPlugin(name);
        loadPlugin(filePath);
    }
}

void PluginManager::reloadAll() {
    qDebug() << "[PluginManager] Reloading all plugins...";
    
    QStringList loaded = loadedPlugins();
    unloadAll();
    loadAll();
    
    emit pluginsReloaded();
}

// ============================================================================
// Plugin Information
// ============================================================================

QStringList PluginManager::availablePlugins() const {
    return d->pluginInfos.keys();
}

QStringList PluginManager::loadedPlugins() const {
    QStringList loaded;
    for (const QString& name : d->pluginInfos.keys()) {
        if (d->pluginInfos[name].loaded) {
            loaded.append(name);
        }
    }
    return loaded;
}

PluginInfo PluginManager::getPluginInfo(const QString& name) const {
    return d->pluginInfos.value(name);
}

QMap<QString, PluginInfo> PluginManager::allPlugins() const {
    return d->pluginInfos;
}

QString PluginManager::pluginsDirectory() const {
    return d->pluginsDir;
}

QStringList PluginManager::scanForPlugins() const {
    QStringList plugins;
    
    QDir dir(d->pluginsDir);
    if (!dir.exists()) {
        return plugins;
    }
    
    // Find all .lua files
    QStringList filters = {"*.lua"};
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    
    for (const QFileInfo& file : files) {
        plugins.append(file.absoluteFilePath());
    }
    
    return plugins;
}

// ============================================================================
// Execution
// ============================================================================

bool PluginManager::executeFunction(const QString& pluginName,
                                     const QString& functionName,
                                     const QStringList& args)
{
    if (!d->luaStates.contains(pluginName)) {
        qWarning() << "[PluginManager] Plugin not found:" << pluginName;
        return false;
    }
    
    return d->luaStates[pluginName]->callFunction(functionName, args);
}

void PluginManager::saveState() {
    qDebug() << "[PluginManager] Saving plugin state...";
    
    QSettings settings(LocalStorage::getConfigPath() + "/plugins.ini",
                       QSettings::IniFormat);
    
    // Save enabled plugins
    settings.beginGroup("EnabledPlugins");
    for (const QString& name : d->pluginInfos.keys()) {
        settings.setValue(name, d->pluginInfos[name].enabled);
    }
    settings.endGroup();
}

void PluginManager::restoreState() {
    qDebug() << "[PluginManager] Restoring plugin state...";
    
    QSettings settings(LocalStorage::getConfigPath() + "/plugins.ini",
                       QSettings::IniFormat);
    
    // Restore enabled state
    settings.beginGroup("EnabledPlugins");
    for (const QString& name : settings.childKeys()) {
        if (d->pluginInfos.contains(name)) {
            bool enabled = settings.value(name).toBool();
            d->pluginInfos[name].enabled = enabled;
        }
    }
    settings.endGroup();
}

// ============================================================================
// Private Methods
// ============================================================================

void PluginManager::createLuaState() {
    // Lua state creation is handled per-plugin in loadPlugin
}

void PluginManager::registerSafeWrappers() {
    // Register safe Qt wrappers for all Lua states
    // This is handled by LuaState::registerWrappers()
}

bool PluginManager::validatePlugin(const QString& filePath) {
    QFileInfo file(filePath);
    
    // Check file exists
    if (!file.exists()) {
        return false;
    }
    
    // Check extension
    if (file.suffix().toLower() != "lua") {
        return false;
    }
    
    // Check file is readable
    QFile scriptFile(filePath);
    if (!scriptFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    scriptFile.close();
    
    return true;
}

void PluginManager::createExamplePlugin() {
    QString examplePluginPath = d->pluginsDir + "/example_message_logger.lua";
    
    if (QFile::exists(examplePluginPath)) {
        return;
    }
    
    QFile file(examplePluginPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << R"(--[[
    Example Plugin for BummerGram
    Version: 1.0.0
    Author: BummerGram Team
    Description: Demonstrates the Lua plugin API
]]

-- Plugin metadata
local PLUGIN_VERSION = "1.0.0"
local PLUGIN_AUTHOR = "BummerGram Team"
local PLUGIN_DESCRIPTION = "Example plugin demonstrating Lua API"

-- Called when plugin is loaded
function onLoad()
    bummer.log("Example plugin loaded")
    bummer.log("Version: " .. PLUGIN_VERSION)
end

-- Called when plugin is unloaded
function onUnload()
    bummer.log("Example plugin unloaded")
end

-- Example: Log a message
function logMessage(message)
    bummer.log("Message logged: " .. tostring(message))
end

-- Example: Get plugin info
function getInfo()
    return {
        name = "Example Plugin",
        version = PLUGIN_VERSION,
        author = PLUGIN_AUTHOR,
        description = PLUGIN_DESCRIPTION
    }
end
)";
        file.close();
        qDebug() << "[PluginManager] Created example plugin";
    }
}

} // namespace BummerGram
