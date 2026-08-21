#include "bummer_settings_widget.h"
#include "../../core/localstorage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include "settings_general.h"
#include "settings_lua_plugins.h"
#include "settings_layout.h"

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

struct BummerSettingsWidget::Impl {
    QTabWidget* tabWidget = nullptr;
    
    SettingsGeneral* generalTab = nullptr;
    SettingsLuaPlugins* pluginsTab = nullptr;
    SettingsLayout* layoutTab = nullptr;
    
    QPushButton* saveButton = nullptr;
    QPushButton* cancelButton = nullptr;
    QPushButton* resetButton = nullptr;
};

void BummerSettingsWidget::ImplDeleter::operator()(
    BummerSettingsWidget::Impl* p) const 
{
    delete p;
}

} // namespace

// ============================================================================
// Construction
// ============================================================================

BummerSettingsWidget::BummerSettingsWidget(QWidget* parent)
    : QDialog(parent)
    , d(std::make_unique<Impl>())
{
    setWindowTitle("BummerGram Settings");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    
    setupUi();
    loadSettings();
    createConnections();
    
    // Center on parent or screen
    if (parent) {
        move(parent->geometry().center() - rect().center());
    } else {
        move(QApplication::desktop()->geometry().center() - rect().center());
    }
}

BummerSettingsWidget::~BummerSettingsWidget() = default;

// ============================================================================
// Setup
// ============================================================================

void BummerSettingsWidget::setupUi() {
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Create tab widget
    d->tabWidget = new QTabWidget(this);
    d->tabWidget->setObjectName("settingsTabWidget");
    
    // Create tabs
    d->generalTab = new SettingsGeneral(d->tabWidget);
    d->pluginsTab = new SettingsLuaPlugins(d->tabWidget);
    d->layoutTab = new SettingsLayout(d->tabWidget);
    
    // Add tabs
    d->tabWidget->addTab(d->generalTab, "General");
    d->tabWidget->addTab(d->pluginsTab, "Lua Plugins");
    d->tabWidget->addTab(d->layoutTab, "Layout");
    
    mainLayout->addWidget(d->tabWidget);
    
    // Button box
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    d->resetButton = new QPushButton("Reset to Defaults", this);
    d->resetButton->setObjectName("resetButton");
    buttonLayout->addWidget(d->resetButton);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );
    
    d->saveButton = buttonBox->button(QDialogButtonBox::Ok);
    d->saveButton->setText("Save");
    d->saveButton->setObjectName("saveButton");
    
    d->cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    d->cancelButton->setObjectName("cancelButton");
    
    buttonLayout->addWidget(buttonBox);
    mainLayout->addLayout(buttonLayout);
    
    // Set minimum size
    setMinimumSize(600, 450);
    resize(700, 500);
    
    // Apply dark theme
    setStyleSheet(R"(
        QDialog {
            background-color: #1a1a2e;
            color: #eaeaea;
        }
        QTabWidget::pane {
            border: 1px solid #0f3460;
            background-color: #16213e;
        }
        QTabBar::tab {
            background-color: #16213e;
            color: #888;
            padding: 8px 16px;
            border: 1px solid #0f3460;
            border-bottom: none;
        }
        QTabBar::tab:selected {
            background-color: #0f3460;
            color: #eaeaea;
        }
        QTabBar::tab:hover {
            background-color: #1a2744;
        }
        QPushButton {
            background-color: #0f3460;
            color: #eaeaea;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #1a5090;
        }
        QPushButton:pressed {
            background-color: #0a2540;
        }
        QPushButton#saveButton {
            background-color: #2e7d32;
        }
        QPushButton#saveButton:hover {
            background-color: #388e3c;
        }
        QLabel {
            color: #eaeaea;
        }
    )");
}

void BummerSettingsWidget::loadSettings() {
    d->generalTab->loadSettings();
    d->pluginsTab->loadSettings();
    d->layoutTab->loadSettings();
}

void BummerSettingsWidget::createConnections() {
    // Tab change
    connect(d->tabWidget, &QTabWidget::currentChanged,
            this, &BummerSettingsWidget::onTabChanged);
    
    // Buttons
    connect(d->saveButton, &QPushButton::clicked,
            this, &BummerSettingsWidget::saveSettings);
    
    connect(d->cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    
    connect(d->resetButton, &QPushButton::clicked,
            this, &BummerSettingsWidget::resetToDefaults);
}

// ============================================================================
// Public Methods
// ============================================================================

void BummerSettingsWidget::switchToTab(const QString& tabName) {
    QMap<QString, int> tabIndex = {
        {"general", 0},
        {"plugins", 1},
        {"layout", 2}
    };
    
    if (tabIndex.contains(tabName.toLower())) {
        d->tabWidget->setCurrentIndex(tabIndex[tabName.toLower()]);
    }
}

// ============================================================================
// Private Slots
// ============================================================================

void BummerSettingsWidget::onTabChanged(int index) {
    Q_UNUSED(index)
    // Could load/save tab-specific data here
}

void BummerSettingsWidget::saveSettings() {
    d->generalTab->saveSettings();
    d->pluginsTab->saveSettings();
    d->layoutTab->saveSettings();
    
    emit settingsChanged();
    accept();
}

void BummerSettingsWidget::resetToDefaults() {
    d->generalTab->resetToDefaults();
    d->pluginsTab->resetToDefaults();
    d->layoutTab->resetToDefaults();
}

} // namespace BummerGram
