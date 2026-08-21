#pragma once
/**
 * @file settings_layout.h
 * @brief Layout customization settings tab
 */

#include <QWidget>

class QCheckBox;
class QSpinBox;
class QComboBox;
class QGroupBox;

namespace BummerGram {

class SettingsLayout : public QWidget {
    Q_OBJECT

public:
    explicit SettingsLayout(QWidget* parent = nullptr);
    ~SettingsLayout() override;
    
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

private:
    void setupUi();
    
    QGroupBox* m_sidebarGroup = nullptr;
    QSpinBox* m_sidebarWidthSpin = nullptr;
    QCheckBox* m_compactModeCheck = nullptr;
    QCheckBox* m_showStatusIconsCheck = nullptr;
    
    QGroupBox* m_chatGroup = nullptr;
    QCheckBox* m_showTimestampsCheck = nullptr;
    QCheckBox* m_compactMessagesCheck = nullptr;
    QSpinBox* m_messageSpacingSpin = nullptr;
    
    QGroupBox* m_customTabsGroup = nullptr;
    QCheckBox* m_enableCustomTabsCheck = nullptr;
    QComboBox* m_tabPositionCombo = nullptr;
};

} // namespace BummerGram
