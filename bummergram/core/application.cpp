#include "application.h"
#include "localstorage.h"
#include "../plugins/plugin_manager.h"
#include "../ui/splash/splash_screen.h"

#include <QTimer>
#include <QDebug>
#include <QSessionManager>

namespace BummerGram {

class Application::Impl {
public:
    std::unique_ptr<PluginManager> pluginManager;
    bool initialized = false;
    bool firstLaunch = true;
};

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , d(std::make_unique<Impl>())
{
    // Set application identity
    setApplicationName("BummerGram");
    setApplicationVersion(LocalStorage::getVersionString());
    setApplicationDisplayName("BummerGram");
    setOrganizationName("BummerGram");
    setOrganizationDomain("bummergram.org");
    
    // Set desktop file name for Linux
    setDesktopFileName("BummerGram");
    
    // Check for first launch
    d->firstLaunch = !LocalStorage::getSettings()->contains("General/SetupComplete");
}

Application::~Application() = default;

void Application::initialize() {
    if (d->initialized) {
        return;
    }
    
    qDebug() << "[BummerGram] Initializing application...";
    
    // Ensure storage directories exist
    LocalStorage::ensureDirectoriesExist();
    
    // Initialize plugin system
    d->pluginManager = std::make_unique<PluginManager>();
    d->pluginManager->initialize();
    
    d->initialized = true;
    
    qDebug() << "[BummerGram] Application initialized successfully";
    
    emit initialized();
}

void Application::activate() {
    qDebug() << "[BummerGram] Application activated";
    emit activated();
}

bool Application::isFirstLaunch() const {
    return d->firstLaunch;
}

void Application::completeFirstLaunch() {
    d->firstLaunch = false;
    LocalStorage::getSettings()->setValue("General/SetupComplete", true);
}

PluginManager& Application::pluginManager() const {
    Q_ASSERT(d->pluginManager != nullptr);
    return *d->pluginManager;
}

void Application::commitData(QSessionManager& manager) {
    qDebug() << "[BummerGram] Committing session data...";
    
    // Allow session shutdown to proceed
    manager.release();
}

void Application::saveState(QSessionManager& manager) {
    qDebug() << "[BummerGram] Saving session state...";
    
    // Save plugin states
    if (d->pluginManager) {
        d->pluginManager->saveState();
    }
    
    manager.release();
}

} // namespace BummerGram
