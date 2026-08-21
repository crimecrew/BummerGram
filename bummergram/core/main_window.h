#pragma once
/**
 * @file main_window.h
 * @brief BummerGram Main Window
 */

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QToolBar>
#include <memory>

namespace BummerGram {

class BummerSettingsWidget;

/**
 * @class MainWindow
 * @brief Main application window for BummerGram
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construct the main window
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MainWindow() override;

signals:
    /**
     * @brief Emitted when the window is fully initialized
     */
    void windowReady();

    /**
     * @brief Emitted when settings are requested
     */
    void settingsRequested();

public slots:
    /**
     * @brief Show the settings dialog
     */
    void showSettings();
    
    /**
     * @brief Show about dialog
     */
    void showAbout();
    
    /**
     * @brief Open the plugin manager
     */
    void showPluginManager();

protected:
    /**
     * @brief Handle window close event
     */
    void closeEvent(QCloseEvent* event) override;
    
    /**
     * @brief Handle keyboard shortcuts
     */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onMenuActionTriggered(QAction* action);

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createConnections();

    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
