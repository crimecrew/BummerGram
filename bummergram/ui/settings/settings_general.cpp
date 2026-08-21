#include "settings_general.h"
#include "../../core/localstorage.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

namespace BummerGram {

SettingsGeneral::SettingsGeneral(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

SettingsGeneral::~SettingsGeneral() = default;

void SettingsGeneral::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Startup options
    m_startupGroup = new QGroupBox("Startup", this);
    m_startupGroup->setObjectName("startupGroup");
    
    QFormLayout* startupLayout = new QFormLayout(m_startupGroup);
    
    m_autoStartCheck = new QCheckBox("Start with Windows", m_startupGroup);
    m_startMinimizedCheck = new QCheckBox("Start minimized", m_startupGroup);
    m_minimizeToTrayCheck = new QCheckBox("Minimize to system tray", m_startupGroup);
    m_closeToTrayCheck = new QCheckBox("Close to system tray instead of quitting", m_startupGroup);
    
    startupLayout->addRow(m_autoStartCheck);
    startupLayout->addRow(m_startMinimizedCheck);
    startupLayout->addRow(m_minimizeToTrayCheck);
    startupLayout->addRow(m_closeToTrayCheck);
    
    // Sound options
    m_soundGroup = new QGroupBox("Sound", this);
    m_soundGroup->setObjectName("soundGroup");
    
    QFormLayout* soundLayout = new QFormLayout(m_soundGroup);
    
    m_playStartupSoundCheck = new QCheckBox("Play startup sound", m_soundGroup);
    
    soundLayout->addRow(m_playStartupSoundCheck);
    
    // General options
    QGroupBox* generalGroup = new QGroupBox("General", this);
    generalGroup->setObjectName("generalGroup");
    
    QFormLayout* generalLayout = new QFormLayout(generalGroup);
    
    // Theme selection
    m_themeCombo = new QComboBox(generalGroup);
    m_themeCombo->addItem("Dark", "dark");
    m_themeCombo->addItem("Light", "light");
    m_themeCombo->addItem("System", "system");
    generalLayout->addRow("Theme:", m_themeCombo);
    
    // Cache size
    m_cacheSizeSpin = new QSpinBox(generalGroup);
    m_cacheSizeSpin->setSuffix(" MB");
    m_cacheSizeSpin->setRange(100, 10000);
    m_cacheSizeSpin->setSingleStep(100);
    generalLayout->addRow("Cache size:", m_cacheSizeSpin);
    
    mainLayout->addWidget(m_startupGroup);
    mainLayout->addWidget(m_soundGroup);
    mainLayout->addWidget(generalGroup);
    mainLayout->addStretch();
    
    // Style
    m_startupGroup->setStyleSheet(R"(
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
    )");
}

void SettingsGeneral::loadSettings() {
    QSettings* settings = LocalStorage::getSettings();
    
    m_autoStartCheck->setChecked(
        settings->value("General/AutoStart", false).toBool()
    );
    m_startMinimizedCheck->setChecked(
        settings->value("General/StartMinimized", false).toBool()
    );
    m_minimizeToTrayCheck->setChecked(
        settings->value("General/MinimizeToTray", true).toBool()
    );
    m_closeToTrayCheck->setChecked(
        settings->value("General/CloseToTray", false).toBool()
    );
    m_playStartupSoundCheck->setChecked(
        settings->value("General/PlayStartupSound", true).toBool()
    );
    m_cacheSizeSpin->setValue(
        settings->value("General/CacheSizeMB", 512).toInt()
    );
    
    int themeIndex = m_themeCombo->findData(
        settings->value("General/Theme", "dark")
    );
    if (themeIndex >= 0) {
        m_themeCombo->setCurrentIndex(themeIndex);
    }
}

void SettingsGeneral::saveSettings() {
    QSettings* settings = LocalStorage::getSettings();
    
    settings->setValue("General/AutoStart", m_autoStartCheck->isChecked());
    settings->setValue("General/StartMinimized", m_startMinimizedCheck->isChecked());
    settings->setValue("General/MinimizeToTray", m_minimizeToTrayCheck->isChecked());
    settings->setValue("General/CloseToTray", m_closeToTrayCheck->isChecked());
    settings->setValue("General/PlayStartupSound", m_playStartupSoundCheck->isChecked());
    settings->setValue("General/CacheSizeMB", m_cacheSizeSpin->value());
    settings->setValue("General/Theme", m_themeCombo->currentData());
}

void SettingsGeneral::resetToDefaults() {
    m_autoStartCheck->setChecked(false);
    m_startMinimizedCheck->setChecked(false);
    m_minimizeToTrayCheck->setChecked(true);
    m_closeToTrayCheck->setChecked(false);
    m_playStartupSoundCheck->setChecked(true);
    m_cacheSizeSpin->setValue(512);
    m_themeCombo->setCurrentIndex(0);
}

} // namespace BummerGram
