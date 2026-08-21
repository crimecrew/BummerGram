#pragma once
/**
 * @file splash_screen.h
 * @brief Custom text-based splash screen for BummerGram
 * 
 * Renders "Bummer" in bold neon-styled text with pulsing glow animation.
 * All graphics are purely text-based using Qt's painter APIs.
 */

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QString>

namespace BummerGram {

/**
 * @class SplashScreen
 * @brief Animated text-based splash screen
 * 
 * Features:
 * - Pure text rendering (no image assets)
 * - Neon glow effect with pulsing animation
 * - Smooth fade-out transition
 * - Keyboard/mouse dismissal support
 */
class SplashScreen : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct splash screen
     * @param parent Parent widget
     */
    explicit SplashScreen(QWidget* parent = nullptr);
    
    /**
     * @brief Destructor
     */
    ~SplashScreen() override;

public slots:
    /**
     * @brief Fade out and close the splash
     */
    void fadeOutAndClose();

signals:
    /**
     * @brief Emitted when splash is closed
     */
    void closed();

protected:
    // ========================================================================
    // Qt Events
    // ========================================================================
    
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onFadeOutFinished();
    void animateGlowPulse();

private:
    // ========================================================================
    // Setup Methods
    // ========================================================================
    
    void setupStyling();
    void setupAnimations();
    void setupVersionLabel();
    
    // ========================================================================
    // Rendering Methods
    // ========================================================================
    
    void renderBackground(QPainter& painter);
    void renderTextLogo(QPainter& painter);
    void drawNeonText(QPainter& painter, 
                      const QString& text, 
                      const QFont& font, 
                      const QPointF& position,
                      const QColor& baseColor,
                      const QColor& glowColor);
    void drawTextShadow(QPainter& painter,
                        const QString& text,
                        const QFont& font,
                        const QPointF& position,
                        const QColor& shadowColor);

    // ========================================================================
    // Member Variables
    // ========================================================================
    
    QLabel* m_versionLabel = nullptr;
    QLabel* m_taglineLabel = nullptr;
    QGraphicsOpacityEffect* m_opacityEffect = nullptr;
    QPropertyAnimation* m_fadeAnimation = nullptr;
    QTimer* m_pulseTimer = nullptr;
    
    // Animation state
    qreal m_glowIntensity = 0.5;
    bool m_glowIncreasing = true;
    
    // Dimensions
    static constexpr int WIDTH = 520;
    static constexpr int HEIGHT = 360;
    static constexpr int BORDER_RADIUS = 20;
};

} // namespace BummerGram
