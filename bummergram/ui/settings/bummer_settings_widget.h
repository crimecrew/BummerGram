#pragma once
/**
 * @file bummer_settings_widget.h
 * @brief BummerGram Settings Widget
 */

#include <QWidget>
#include <QTabWidget>
#include <QDialog>

namespace BummerGram {

/**
 * @class BummerSettingsWidget
 * @brief Main settings dialog for BummerGram
 */
class BummerSettingsWidget : public QDialog {
    Q_OBJECT

public:
    explicit BummerSettingsWidget(QWidget* parent = nullptr);
    ~BummerSettingsWidget() override;
    
    void switchToTab(const QString& tabName);

signals:
    void settingsChanged();
    void pluginsChanged();

private slots:
    void onTabChanged(int index);
    void saveSettings();
    void resetToDefaults();

private:
    void setupUi();
    void loadSettings();
    void createConnections();
    
    class Impl;
    struct ImplDeleter {
        void operator()(BummerSettingsWidget::Impl* p) const;
    };
    
    std::unique_ptr<Impl, ImplDeleter> d;
};

} // namespace BummerGram
