#pragma once
/**
 * @file settings_general.h
 * @brief General settings tab
 */

#include <QWidget>

class QCheckBox;
class QSpinBox;
class QComboBox;
class QGroupBox;

namespace BummerGram {

class SettingsGeneral : public QWidget {
    Q_OBJECT

public:
    explicit SettingsGeneral(QWidget* parent = nullptr);
    ~SettingsGeneral() override;
    
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

private:
    void setupUi();
    
    // Widgets
    QCheckBox* m_autoStartCheck = nullptr;
    QCheckBox* m_minimizeToTrayCheck = nullptr;
    QCheckBox* m_closeToTrayCheck = nullptr;
    QCheckBox* m_startMinimizedCheck = nullptr;
    QCheckBox* m_playStartupSoundCheck = nullptr;
    QSpinBox* m_cacheSizeSpin = nullptr;
    QComboBox* m_themeCombo = nullptr;
    QGroupBox* m_soundGroup = nullptr;
    QGroupBox* m_startupGroup = nullptr;
};

} // namespace BummerGram
