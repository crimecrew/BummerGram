#include "settings_layout.h"
#include "../../core/localstorage.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>

namespace BummerGram {

SettingsLayout::SettingsLayout(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

SettingsLayout::~SettingsLayout() = default;

void SettingsLayout::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Sidebar options
    m_sidebarGroup = new QGroupBox("Sidebar", this);
    m_sidebarGroup->setObjectName("sidebarGroup");
    
    QFormLayout* sidebarLayout = new QFormLayout(m_sidebarGroup);
    
    m_sidebarWidthSpin = new QSpinBox(m_sidebarGroup);
    m_sidebarWidthSpin->setSuffix(" px");
    m_sidebarWidthSpin->setRange(200, 400);
    m_sidebarWidthSpin->setSingleStep(10);
    sidebarLayout->addRow("Width:", m_sidebarWidthSpin);
    
    m_compactModeCheck = new QCheckBox("Compact mode", m_sidebarGroup);
    sidebarLayout->addRow(m_compactModeCheck);
    
    m_showStatusIconsCheck = new QCheckBox("Show status icons", m_sidebarGroup);
    sidebarLayout->addRow(m_showStatusIconsCheck);
    
    // Chat options
    m_chatGroup = new QGroupBox("Chat View", this);
    m_chatGroup->setObjectName("chatGroup");
    
    QFormLayout* chatLayout = new QFormLayout(m_chatGroup);
    
    m_showTimestampsCheck = new QCheckBox("Show message timestamps", m_chatGroup);
    chatLayout->addRow(m_showTimestampsCheck);
    
    m_compactMessagesCheck = new QCheckBox("Compact message display", m_chatGroup);
    chatLayout->addRow(m_compactMessagesCheck);
    
    m_messageSpacingSpin = new QSpinBox(m_chatGroup);
    m_messageSpacingSpin->setSuffix(" px");
    m_messageSpacingSpin->setRange(0, 20);
    chatLayout->addRow("Message spacing:", m_messageSpacingSpin);
    
    // Custom tabs
    m_customTabsGroup = new QGroupBox("Custom Tabs", this);
    m_customTabsGroup->setObjectName("customTabsGroup");
    
    QFormLayout* tabsLayout = new QFormLayout(m_customTabsGroup);
    
    m_enableCustomTabsCheck = new QCheckBox("Enable custom organization tabs", m_customTabsGroup);
    tabsLayout->addRow(m_enableCustomTabsCheck);
    
    m_tabPositionCombo = new QComboBox(m_customTabsGroup);
    m_tabPositionCombo->addItem("Top", "top");
    m_tabPositionCombo->addItem("Left", "left");
    m_tabPositionCombo->addItem("Bottom", "bottom");
    tabsLayout->addRow("Tab position:", m_tabPositionCombo);
    
    mainLayout->addWidget(m_sidebarGroup);
    mainLayout->addWidget(m_chatGroup);
    mainLayout->addWidget(m_customTabsGroup);
    mainLayout->addStretch();
    
    // Style
    QString groupStyle = R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #0f3460;
            border-radius: 4px;
            margin-top: 8px;
            padding-top: 8px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 8px;
            padding: 0 4px;
        }
    )";
    
    m_sidebarGroup->setStyleSheet(groupStyle);
    m_chatGroup->setStyleSheet(groupStyle);
    m_customTabsGroup->setStyleSheet(groupStyle);
}

void SettingsLayout::loadSettings() {
    QSettings* settings = LocalStorage::getSettings();
    
    m_sidebarWidthSpin->setValue(
        settings->value("Layout/SidebarWidth", 280).toInt()
    );
    m_compactModeCheck->setChecked(
        settings->value("Layout/CompactMode", false).toBool()
    );
    m_showStatusIconsCheck->setChecked(
        settings->value("Layout/ShowStatusIcons", true).toBool()
    );
    
    m_showTimestampsCheck->setChecked(
        settings->value("Layout/ShowTimestamps", true).toBool()
    );
    m_compactMessagesCheck->setChecked(
        settings->value("Layout/CompactMessages", false).toBool()
    );
    m_messageSpacingSpin->setValue(
        settings->value("Layout/MessageSpacing", 4).toInt()
    );
    
    m_enableCustomTabsCheck->setChecked(
        settings->value("Layout/EnableCustomTabs", false).toBool()
    );
    
    int tabPosIndex = m_tabPositionCombo->findData(
        settings->value("Layout/TabPosition", "top")
    );
    if (tabPosIndex >= 0) {
        m_tabPositionCombo->setCurrentIndex(tabPosIndex);
    }
}

void SettingsLayout::saveSettings() {
    QSettings* settings = LocalStorage::getSettings();
    
    settings->setValue("Layout/SidebarWidth", m_sidebarWidthSpin->value());
    settings->setValue("Layout/CompactMode", m_compactModeCheck->isChecked());
    settings->setValue("Layout/ShowStatusIcons", m_showStatusIconsCheck->isChecked());
    
    settings->setValue("Layout/ShowTimestamps", m_showTimestampsCheck->isChecked());
    settings->setValue("Layout/CompactMessages", m_compactMessagesCheck->isChecked());
    settings->setValue("Layout/MessageSpacing", m_messageSpacingSpin->value());
    
    settings->setValue("Layout/EnableCustomTabs", m_enableCustomTabsCheck->isChecked());
    settings->setValue("Layout/TabPosition", m_tabPositionCombo->currentData());
}

void SettingsLayout::resetToDefaults() {
    m_sidebarWidthSpin->setValue(280);
    m_compactModeCheck->setChecked(false);
    m_showStatusIconsCheck->setChecked(true);
    
    m_showTimestampsCheck->setChecked(true);
    m_compactMessagesCheck->setChecked(false);
    m_messageSpacingSpin->setValue(4);
    
    m_enableCustomTabsCheck->setChecked(false);
    m_tabPositionCombo->setCurrentIndex(0);
}

} // namespace BummerGram
