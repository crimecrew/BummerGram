#include "main_window.h"
#include "localstorage.h"
#include "../ui/settings/bummer_settings_widget.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QStyle>
#include <QDebug>

namespace BummerGram {

class MainWindow::Impl {
public:
    std::unique_ptr<BummerSettingsWidget> settingsWidget;
    QLabel* statusLabel = nullptr;
    QLabel* versionLabel = nullptr;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , d(std::make_unique<Impl>())
{
    setupUi();
    
    // Emit ready signal after construction
    QTimer::singleShot(100, this, [this]() {
        emit windowReady();
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    // Window properties
    setWindowTitle("BummerGram");
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    // Set window icon (using standard icon as fallback)
    setWindowIcon(QIcon::fromTheme("utilities-system-monitor", 
                                    QIcon(":/icons/bummergram.png")));
    
    // Central widget - placeholder for future content
    QLabel* centralWidget = new QLabel(
        "<h1>BummerGram</h1>"
        "<p style='font-size: 16px;'>Enhanced Desktop Client</p>"
        "<p style='color: #666;'>Use <b>Settings → Bummer Settings</b> to configure the application.</p>"
        "<p style='color: #666;'>Access <b>Settings → Plugin Manager</b> to manage Lua plugins.</p>",
        this
    );
    centralWidget->setAlignment(Qt::AlignCenter);
    setCentralWidget(centralWidget);
    
    // Setup components
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    
    // Apply stylesheet
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a2e;
            color: #eaeaea;
        }
        QLabel {
            color: #eaeaea;
        }
        QMenuBar {
            background-color: #16213e;
            color: #eaeaea;
            border-bottom: 1px solid #0f3460;
        }
        QMenuBar::item:selected {
            background-color: #0f3460;
        }
        QMenu {
            background-color: #16213e;
            color: #eaeaea;
            border: 1px solid #0f3460;
        }
        QMenu::item:selected {
            background-color: #0f3460;
        }
        QToolBar {
            background-color: #16213e;
            border-bottom: 1px solid #0f3460;
            spacing: 6px;
        }
        QStatusBar {
            background-color: #16213e;
            color: #888;
        }
        QLabel#versionLabel {
            color: #666;
        }
    )");
    
    // Log initialization
    qDebug() << "[BummerGram] MainWindow initialized";
}

void MainWindow::setupMenuBar() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    
    QAction* settingsAction = fileMenu->addAction(
        QIcon::fromTheme("preferences-system"),
        "&Settings",
        this,
        &MainWindow::showSettings,
        QKeySequence(Qt::CTRL | Qt::Key_O)
    );
    settingsAction->setObjectName("settingsAction");
    
    fileMenu->addSeparator();
    
    QAction* quitAction = fileMenu->addAction(
        QIcon::fromTheme("application-exit"),
        "&Quit",
        qApp,
        &QApplication::quit,
        QKeySequence(Qt::CTRL | Qt::Key_Q)
    );
    quitAction->setObjectName("quitAction");
    
    // Plugins Menu
    QMenu* pluginsMenu = menuBar()->addMenu("&Plugins");
    
    QAction* pluginManagerAction = pluginsMenu->addAction(
        QIcon::fromTheme("extension"),
        "&Plugin Manager",
        this,
        &MainWindow::showPluginManager
    );
    pluginManagerAction->setObjectName("pluginManagerAction");
    
    pluginsMenu->addSeparator();
    
    QAction* reloadPluginsAction = pluginsMenu->addAction(
        QIcon::fromTheme("view-refresh"),
        "&Reload All Plugins",
        [this]() {
            auto& pm = Application::instance()->pluginManager();
            pm.unloadAll();
            pm.loadAll();
        }
    );
    reloadPluginsAction->setObjectName("reloadPluginsAction");
    
    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction(
        QIcon::fromTheme("help-about"),
        "&About BummerGram",
        this,
        &MainWindow::showAbout
    );
    aboutAction->setObjectName("aboutAction");
    
    QAction* aboutQtAction = helpMenu->addAction(
        "About &Qt",
        qApp,
        &QApplication::aboutQt
    );
    aboutQtAction->setObjectName("aboutQtAction");
}

void MainWindow::setupToolBar() {
    QToolBar* mainToolBar = addToolBar("Main Toolbar");
    mainToolBar->setMovable(false);
    mainToolBar->setFloatable(false);
    
    // Settings button
    QAction* settingsAction = mainToolBar->addAction(
        QIcon::fromTheme("preferences-system"),
        "Settings",
        this,
        &MainWindow::showSettings
    );
    settingsAction->setObjectName("toolbarSettingsAction");
    
    mainToolBar->addSeparator();
    
    // Plugin manager button
    QAction* pluginAction = mainToolBar->addAction(
        QIcon::fromTheme("extension"),
        "Plugins",
        this,
        &MainWindow::showPluginManager
    );
    pluginAction->setObjectName("toolbarPluginAction");
}

void MainWindow::setupStatusBar() {
    // Status message
    d->statusLabel = new QLabel("Ready", this);
    d->statusLabel->setObjectName("statusLabel");
    statusBar()->addWidget(d->statusLabel, 1);
    
    // Version info
    d->versionLabel = new QLabel(
        QString("BummerGram v%1").arg(LocalStorage::getVersionString()),
        this
    );
    d->versionLabel->setObjectName("versionLabel");
    statusBar()->addPermanentWidget(d->versionLabel);
}

void MainWindow::showSettings() {
    if (!d->settingsWidget) {
        d->settingsWidget = std::make_unique<BummerSettingsWidget>(this);
    }
    
    d->settingsWidget->show();
    d->settingsWidget->activateWindow();
    d->settingsWidget->raise();
    
    emit settingsRequested();
}

void MainWindow::showAbout() {
    QString aboutText = QString(
        "<h2>BummerGram</h2>"
        "<p>Version %1</p>"
        "<p>Enhanced Desktop Client</p>"
        "<p style='margin-top: 12px;'>A powerful, customizable desktop application "
        "with Lua scripting support for workflow automation.</p>"
        "<p style='margin-top: 12px; color: #888;'>"
        "Licensed under <a href='https://www.gnu.org/licenses/gpl-3.0.html'>"
        "GNU General Public License v3.0</a></p>"
    ).arg(LocalStorage::getVersionString());
    
    QMessageBox::about(this, "About BummerGram", aboutText);
}

void MainWindow::showPluginManager() {
    // Open plugin manager in settings
    showSettings();
    
    // Switch to plugins tab
    if (d->settingsWidget) {
        d->settingsWidget->switchToTab("plugins");
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "[BummerGram] MainWindow closing...";
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    // Handle Escape to close dialogs
    if (event->key() == Qt::Key_Escape) {
        if (d->settingsWidget && d->settingsWidget->isVisible()) {
            d->settingsWidget->hide();
            event->accept();
            return;
        }
    }
    
    QMainWindow::keyPressEvent(event);
}

} // namespace BummerGram
