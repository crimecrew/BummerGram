/**
 * @file main.cpp
 * @brief BummerGram Application Entry Point
 * 
 * This is the main entry point for the BummerGram application.
 * It handles:
 * - Application initialization
 * - Isolated storage setup
 * - Splash screen display
 * - Audio playback
 * - Plugin system initialization
 * - Clean shutdown handling
 */

#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QStyleFactory>
#include <csignal>
#include <iostream>
#include <memory>

#include "core/application.h"
#include "core/main_window.h"
#include "core/localstorage.h"
#include "ui/splash/splash_screen.h"
#include "audio/startup_sound.h"

namespace {

/**
 * @brief Signal handler for clean shutdown
 */
void signalHandler(int signal) {
    std::cerr << "\n[BummerGram] Received signal " << signal 
              << ", initiating clean shutdown..." << std::endl;
    
    // Get application instance and quit
    if (auto* app = BummerGram::Application::instance()) {
        QMetaObject::invokeMethod(app, [&app]() {
            app->quit();
        }, Qt::QueuedConnection);
    }
}

/**
 * @brief Setup global exception handlers
 */
void setupExceptionHandlers() {
    // Set up crash handler (basic implementation)
    std::set_terminate([]() {
        std::cerr << "[BummerGram] Unhandled exception - terminating" << std::endl;
        std::abort();
    });
}

} // anonymous namespace

/**
 * @brief Main application entry point
 * @param argc Command line argument count
 * @param argv Command line argument vector
 * @return Application exit code
 */
int main(int argc, char* argv[]) {
    // ========================================================================
    // Early Initialization
    // ========================================================================
    
    // Set isolated storage paths BEFORE QApplication construction
    BummerGram::LocalStorage::setAsDefaultSettingsLocation();
    BummerGram::LocalStorage::ensureDirectoriesExist();
    
    // Install signal handlers for clean shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Setup exception handlers
    setupExceptionHandlers();
    
    // ========================================================================
    // Application Creation
    // ========================================================================
    
    // Create application with isolated identity
    QApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("BummerGram");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("BummerGram");
    app.setDesktopFileName("BummerGram");
    app.setOrganizationName("BummerGram");
    app.setOrganizationDomain("bummergram.org");
    
    // Use Fusion style for modern look
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // ========================================================================
    // Splash Screen (Early Display)
    // ========================================================================
    
    auto splash = std::make_unique<BummerGram::SplashScreen>();
    splash->show();
    splash->activateWindow();
    splash->raise();
    app.processEvents();
    
    // ========================================================================
    // Startup Audio
    // ========================================================================
    
    // Play startup sound asynchronously while splash is showing
    BummerGram::StartupSoundPlayer::instance().playAsync();
    
    // ========================================================================
    // Application Initialization
    // ========================================================================
    
    // Create application wrapper
    auto bummerApp = std::make_unique<BummerGram::Application>(argc, argv);
    bummerApp->initialize();
    
    // ========================================================================
    // Main Window Creation
    // ========================================================================
    
    auto mainWindow = std::make_unique<BummerGram::MainWindow>();
    mainWindow->show();
    mainWindow->activateWindow();
    mainWindow->raise();
    
    // ========================================================================
    // Splash Screen Fade Out
    // ========================================================================
    
    // Connect splash close to main window ready
    QObject::connect(
        mainWindow.get(),
        &BummerGram::MainWindow::windowReady,
        splash.get(),
        &BummerGram::SplashScreen::fadeOutAndClose
    );
    
    // Release splash ownership (Qt will delete when closed)
    // Note: This is safe because we connected to windowReady above
    splash.release();
    
    // ========================================================================
    // Application Event Loop
    // ========================================================================
    
    int exitCode = app.exec();
    
    // ========================================================================
    // Clean Shutdown
    // ========================================================================
    
    qDebug() << "[BummerGram] Application shutting down...";
    
    // Unload all plugins
    bummerApp->pluginManager().unloadAll();
    
    return exitCode;
}
