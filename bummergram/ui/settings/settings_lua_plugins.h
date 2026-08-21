#pragma once
/**
 * @file settings_lua_plugins.h
 * @brief Lua plugins settings tab
 */

#include <QWidget>
#include <QMap>

class QListWidget;
class QPushButton;
class QLabel;

namespace BummerGram {

struct PluginInfo;

class SettingsLuaPlugins : public QWidget {
    Q_OBJECT

public:
    explicit SettingsLuaPlugins(QWidget* parent = nullptr);
    ~SettingsLuaPlugins() override;
    
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

private slots:
    void onPluginSelected(int row);
    void onReloadAll();
    void onOpenPluginsFolder();
    void onEnablePlugin();
    void onDisablePlugin();

private:
    void setupUi();
    void refreshPluginList();
    void updatePluginDetails();
    
    QListWidget* m_pluginList = nullptr;
    QLabel* m_pluginDetailsLabel = nullptr;
    QPushButton* m_reloadButton = nullptr;
    QPushButton* m_openFolderButton = nullptr;
    QPushButton* m_enableButton = nullptr;
    QPushButton* m_disableButton = nullptr;
    
    QMap<QString, PluginInfo> m_plugins;
};

} // namespace BummerGram
