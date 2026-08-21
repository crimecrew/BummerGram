#pragma once
/**
 * @file application.h
 * @brief BummerGram Application class
 */

#include <QApplication>
#include <QIcon>
#include <QString>
#include <QTimer>
#include <memory>

namespace BummerGram {

class PluginManager;
class SplashScreen;

/**
 * @class Application
 * @brief Main application class for BummerGram
 * 
 * Extends QApplication to provide:
 * - Isolated application identity
 * - Plugin system initialization
 * - Clean startup/shutdown lifecycle
 */
class Application : public QApplication {
    Q_OBJECT

public:
    /**
     * @brief Construct the application
     * @param argc Argument count
     * @param argv Argument vector
     */
    explicit Application(int& argc, char** argv);
    
    /**
     * @brief Destructor - clean shutdown
     */
    ~Application() override;
    
    // =========================================================================
    // Singleton Access
    // =========================================================================
    
    /**
     * @brief Get the application instance
     */
    static Application* instance() {
        return qobject_cast<Application*>(QApplication::instance());
    }
    
    // =========================================================================
    // Plugin System
    // =========================================================================
    
    /**
     * @brief Access the plugin manager
     */
    PluginManager& pluginManager() const;
    
    // =========================================================================
    // Application Lifecycle
    // =========================================================================
    
    /**
     * @brief Perform early initialization
     */
    void initialize();
    
    /**
     * @brief Called when application is ready
     */
    void activate();
    
    // =========================================================================
    // Settings
    // =========================================================================
    
    /**
     * @brief Check if this is first launch
     */
    bool isFirstLaunch() const;
    
    /**
     * @brief Mark first launch as complete
     */
    void completeFirstLaunch();

signals:
    /**
     * @brief Emitted when initialization is complete
     */
    void initialized();
    
    /**
     * @brief Emitted when the app is activated
     */
    void activated();
    
    /**
     * @brief Emitted during shutdown
     */
    void aboutToQuit();

protected:
    /**
     * @brief Handle commit data for session management
     */
    void commitData(QSessionManager& manager) override;
    
    /**
     * @brief Handle save state for session management
     */
    void saveState(QSessionManager& manager) override;

private:
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
