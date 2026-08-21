#include "settings_lua_plugins.h"
#include "../../../plugins/plugin_manager.h"
#include "../../core/localstorage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QHostInfo>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>

namespace BummerGram {

SettingsLuaPlugins::SettingsLuaPlugins(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    refreshPluginList();
}

SettingsLuaPlugins::~SettingsLuaPlugins() = default;

void SettingsLuaPlugins::setupUi() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    
    // Left side - plugin list
    QVBoxLayout* leftLayout = new QVBoxLayout();
    
    QLabel* listLabel = new QLabel("Installed Plugins:", this);
    leftLayout->addWidget(listLabel);
    
    m_pluginList = new QListWidget(this);
    m_pluginList->setObjectName("pluginList");
    leftLayout->addWidget(m_pluginList);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_reloadButton = new QPushButton("Reload All", this);
    m_openFolderButton = new QPushButton("Open Folder", this);
    
    buttonLayout->addWidget(m_reloadButton);
    buttonLayout->addWidget(m_openFolderButton);
    
    leftLayout->addLayout(buttonLayout);
    
    // Right side - details
    QVBoxLayout* rightLayout = new QVBoxLayout();
    
    QLabel* detailsLabel = new QLabel("Plugin Details:", this);
    rightLayout->addWidget(detailsLabel);
    
    m_pluginDetailsLabel = new QLabel(
        "Select a plugin to view details.",
        this
    );
    m_pluginDetailsLabel->setObjectName("pluginDetailsLabel");
    m_pluginDetailsLabel->setWordWrap(true);
    m_pluginDetailsLabel->setStyleSheet(R"(
        QLabel {
            background-color: #0f3460;
            padding: 12px;
            border-radius: 4px;
            min-height: 150px;
        }
    )");
    rightLayout->addWidget(m_pluginDetailsLabel);
    
    // Enable/disable buttons
    QHBoxLayout* toggleLayout = new QHBoxLayout();
    
    m_enableButton = new QPushButton("Enable", this);
    m_disableButton = new QPushButton("Disable", this);
    
    toggleLayout->addWidget(m_enableButton);
    toggleLayout->addWidget(m_disableButton);
    
    rightLayout->addLayout(toggleLayout);
    rightLayout->addStretch();
    
    // Add layouts
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addSpacing(16);
    mainLayout->addLayout(rightLayout, 2);
    
    // Connections
    connect(m_pluginList, &QListWidget::currentRowChanged,
            this, &SettingsLuaPlugins::onPluginSelected);
    
    connect(m_reloadButton, &QPushButton::clicked,
            this, &SettingsLuaPlugins::onReloadAll);
    
    connect(m_openFolderButton, &QPushButton::clicked,
            this, &SettingsLuaPlugins::onOpenPluginsFolder);
    
    connect(m_enableButton, &QPushButton::clicked,
            this, &SettingsLuaPlugins::onEnablePlugin);
    
    connect(m_disableButton, &QPushButton::clicked,
            this, &SettingsLuaPlugins::onDisablePlugin);
}

void SettingsLuaPlugins::refreshPluginList() {
    m_pluginList->clear();
    m_plugins.clear();
    
    PluginManager& pm = PluginManager::instance();
    m_plugins = pm.allPlugins();
    
    for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
        QListWidgetItem* item = new QListWidgetItem(it.value().name, m_pluginList);
        
        if (it.value().hasError) {
            item->setForeground(Qt::red);
        } else if (!it.value().loaded) {
            item->setForeground(Qt::gray);
        }
        
        m_pluginList->addItem(item);
    }
    
    // Add note about plugins directory
    if (m_plugins.isEmpty()) {
        m_pluginList->addItem("(No plugins found)");
        m_pluginList->item(0)->setForeground(Qt::gray);
    }
}

void SettingsLuaPlugins::updatePluginDetails() {
    int currentRow = m_pluginList->currentRow();
    if (currentRow < 0 || currentRow >= m_pluginList->count()) {
        m_pluginDetailsLabel->setText("Select a plugin to view details.");
        return;
    }
    
    QString pluginName = m_pluginList->item(currentRow)->text();
    if (pluginName.startsWith('(')) {
        m_pluginDetailsLabel->setText("No plugins installed.\n\n"
            "Add .lua files to:\n" + LocalStorage::getPluginsPath());
        return;
    }
    
    if (!m_plugins.contains(pluginName)) {
        return;
    }
    
    const PluginInfo& info = m_plugins[pluginName];
    
    QString details = QString(
        "<b>Name:</b> %1<br/><br/>"
        "<b>Version:</b> %2<br/><br/>"
        "<b>Author:</b> %3<br/><br/>"
        "<b>Description:</b> %4<br/><br/>"
        "<b>Status:</b> %5<br/><br/>"
        "<b>Path:</b> <span style='font-size: 10px;'>%6</span>"
    )
    .arg(info.name)
    .arg(info.version)
    .arg(info.author)
    .arg(info.description)
    .arg(info.hasError ? "<span style='color: red;'>Error</span>" :
         info.loaded ? "<span style='color: green;'>Loaded</span>" : "Disabled")
    .arg(info.filePath);
    
    m_pluginDetailsLabel->setText(details);
}

void SettingsLuaPlugins::loadSettings() {
    // Refresh plugin list
    refreshPluginList();
}

void SettingsLuaPlugins::saveSettings() {
    // Plugin state is saved by PluginManager
    PluginManager::instance().saveState();
}

void SettingsLuaPlugins::resetToDefaults() {
    // Reset doesn't affect plugins
}

void SettingsLuaPlugins::onPluginSelected(int row) {
    Q_UNUSED(row)
    updatePluginDetails();
}

void SettingsLuaPlugins::onReloadAll() {
    PluginManager::instance().reloadAll();
    refreshPluginList();
}

void SettingsLuaPlugins::onOpenPluginsFolder() {
    QDesktopServices::openUrl(QUrl::fromLocalFile(LocalStorage::getPluginsPath()));
}

void SettingsLuaPlugins::onEnablePlugin() {
    int currentRow = m_pluginList->currentRow();
    if (currentRow < 0) return;
    
    QString pluginName = m_pluginList->item(currentRow)->text();
    if (m_plugins.contains(pluginName) && !m_plugins[pluginName].loaded) {
        PluginManager::instance().loadPlugin(m_plugins[pluginName].filePath);
        refreshPluginList();
    }
}

void SettingsLuaPlugins::onDisablePlugin() {
    int currentRow = m_pluginList->currentRow();
    if (currentRow < 0) return;
    
    QString pluginName = m_pluginList->item(currentRow)->text();
    if (m_plugins.contains(pluginName) && m_plugins[pluginName].loaded) {
        PluginManager::instance().unloadPlugin(pluginName);
        refreshPluginList();
    }
}

} // namespace BummerGram
