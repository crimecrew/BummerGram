#include "splash_screen.h"
#include "../../core/localstorage.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>
#include <QGradient>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

namespace BummerGram {

// ============================================================================
// Constants
// ============================================================================

namespace {
    // Color scheme
    const QColor BACKGROUND_COLOR(18, 18, 28, 250);
    const QColor BORDER_COLOR(60, 60, 80, 180);
    const QColor TEXT_BASE_COLOR(140, 100, 220);    // Purple base
    const QColor TEXT_GLOW_COLOR(0, 200, 255);     // Cyan neon glow
    const QColor GRAM_COLOR(0, 220, 180);           // Teal accent
    const QColor VERSION_COLOR(100, 100, 120);
}

// ============================================================================
// Construction
// ============================================================================

SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent)
    , m_glowIntensity(0.4)
    , m_glowIncreasing(true)
{
    setupStyling();
    setupAnimations();
    setupVersionLabel();
    
    // Center on primary screen
    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    if (primaryScreen) {
        const QRect screenGeometry = primaryScreen->geometry();
        move(screenGeometry.center() - QPoint(width() / 2, height() / 2));
    }
    
    qDebug() << "[SplashScreen] Initialized";
}

SplashScreen::~SplashScreen() = default;

// ============================================================================
// Setup Methods
// ============================================================================

void SplashScreen::setupStyling() {
    // Frameless, always-on-top splash window
    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::Tool
    );
    
    // Transparent background for rounded corners
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    
    // Fixed dimensions
    setFixedSize(WIDTH, HEIGHT);
    
    // Install event filter for keyboard dismissal
    qApp->installEventFilter(this);
}

void SplashScreen::setupAnimations() {
    // Opacity effect for fade animation
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);
    
    // Fade-out animation
    m_fadeAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_fadeAnimation->setDuration(500);
    m_fadeAnimation->setStartValue(1.0);
    m_fadeAnimation->setEndValue(0.0);
    m_fadeAnimation->setEasingCurve(QEasingCurve::InQuad);
    
    connect(m_fadeAnimation, &QPropertyAnimation::finished,
            this, &SplashScreen::onFadeOutFinished);
    
    // Glow pulse animation (20 FPS)
    m_pulseTimer = new QTimer(this);
    connect(m_pulseTimer, &QTimer::timeout, this, &SplashScreen::animateGlowPulse);
    m_pulseTimer->start(50);
}

void SplashScreen::setupVersionLabel() {
    // Version label
    m_versionLabel = new QLabel(
        QString("v%1").arg(LocalStorage::getVersionString()),
        this
    );
    m_versionLabel->setAlignment(Qt::AlignCenter);
    m_versionLabel->setStyleSheet(R"(
        QLabel {
            color: #646480;
            font-size: 13px;
            font-family: 'Segoe UI', 'Ubuntu', sans-serif;
        }
    )");
    m_versionLabel->setGeometry(0, HEIGHT - 50, WIDTH, 30);
    
    // Tagline
    m_taglineLabel = new QLabel("Enhanced Desktop Client", this);
    m_taglineLabel->setAlignment(Qt::AlignCenter);
    m_taglineLabel->setStyleSheet(R"(
        QLabel {
            color: #505068;
            font-size: 12px;
            font-family: 'Segoe UI', 'Ubuntu', sans-serif;
            font-style: italic;
        }
    )");
    m_taglineLabel->setGeometry(0, HEIGHT - 75, WIDTH, 25);
}

// ============================================================================
// Event Handlers
// ============================================================================

void SplashScreen::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    // Draw background
    renderBackground(painter);
    
    // Draw "Bummer" text with neon effect
    renderTextLogo(painter);
}

bool SplashScreen::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        fadeOutAndClose();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void SplashScreen::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event)
    fadeOutAndClose();
}

void SplashScreen::keyPressEvent(QKeyEvent* event) {
    Q_UNUSED(event)
    fadeOutAndClose();
}

// ============================================================================
// Animation Slots
// ============================================================================

void SplashScreen::animateGlowPulse() {
    // Oscillate glow intensity between 0.3 and 1.0
    const qreal minGlow = 0.3;
    const qreal maxGlow = 1.0;
    const qreal step = 0.025;
    
    if (m_glowIncreasing) {
        m_glowIntensity += step;
        if (m_glowIntensity >= maxGlow) {
            m_glowIntensity = maxGlow;
            m_glowIncreasing = false;
        }
    } else {
        m_glowIntensity -= step;
        if (m_glowIntensity <= minGlow) {
            m_glowIntensity = minGlow;
            m_glowIncreasing = true;
        }
    }
    
    update(); // Trigger repaint with new glow intensity
}

void SplashScreen::fadeOutAndClose() {
    if (m_pulseTimer) {
        m_pulseTimer->stop();
    }
    m_fadeAnimation->start();
}

void SplashScreen::onFadeOutFinished() {
    qDebug() << "[SplashScreen] Closed";
    emit closed();
    close();
}

// ============================================================================
// Rendering Methods
// ============================================================================

void SplashScreen::renderBackground(QPainter& painter) {
    QPainterPath path;
    path.addRoundedRect(rect(), BORDER_RADIUS, BORDER_RADIUS);
    
    // Background fill
    painter.fillPath(path, BACKGROUND_COLOR);
    
    // Border
    QPen borderPen(BORDER_COLOR);
    borderPen.setWidth(1.5);
    painter.strokePath(path, borderPen);
    
    // Subtle inner glow
    QPainterPath innerPath;
    innerPath.addRoundedRect(rect().adjusted(2, 2, -2, -2), 
                             BORDER_RADIUS - 1, BORDER_RADIUS - 1);
    
    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, QColor(80, 80, 120, 20));
    gradient.setColorAt(0.5, Qt::transparent);
    gradient.setColorAt(1, QColor(80, 80, 120, 20));
    
    painter.fillPath(innerPath, gradient);
}

void SplashScreen::renderTextLogo(QPainter& painter) {
    // Main "Bummer" text
    QFont bummerFont("Segoe UI", 68, QFont::Bold);
    bummerFont.setStyleStrategy(QFont::PreferAntialias);
    
    QString bummerText = "Bummer";
    QFontMetrics bummerMetrics(bummerFont);
    qreal bummerWidth = bummerMetrics.horizontalAdvance(bummerText);
    
    // Position "Bummer" in upper-center area
    qreal bummerX = (WIDTH - bummerWidth) / 2.0 - 10;
    qreal bummerY = 100.0;
    QPointF bummerPos(bummerX, bummerY);
    
    // Draw shadow first
    drawTextShadow(painter, bummerText, bummerFont, 
                   bummerPos + QPointF(3, 3), QColor(0, 0, 0, 100));
    
    // Draw neon text
    drawNeonText(painter, bummerText, bummerFont, bummerPos,
                 TEXT_BASE_COLOR, TEXT_GLOW_COLOR);
    
    // "Gram" suffix - smaller, teal accent
    QFont gramFont("Segoe UI", 42, QFont::Bold);
    gramFont.setStyleStrategy(QFont::PreferAntialias);
    
    QString gramText = "Gram";
    qreal gramX = bummerX + bummerWidth + 5;
    qreal gramY = 95.0;
    QPointF gramPos(gramX, gramY);
    
    // Draw Gram shadow
    drawTextShadow(painter, gramText, gramFont,
                   gramPos + QPointF(2, 2), QColor(0, 0, 0, 80));
    
    // Draw Gram with teal glow
    QColor gramGlow(0, 200, 160, 180);
    drawNeonText(painter, gramText, gramFont, gramPos,
                 GRAM_COLOR, gramGlow);
}

void SplashScreen::drawNeonText(QPainter& painter,
                                 const QString& text,
                                 const QFont& font,
                                 const QPointF& position,
                                 const QColor& baseColor,
                                 const QColor& glowColor)
{
    painter.setFont(font);
    
    // Draw multiple glow layers (outer to inner)
    const int glowLayers = 5;
    for (int i = glowLayers; i >= 1; --i) {
        // Reduce alpha and spread based on layer
        qreal alpha = (m_glowIntensity * 60.0) / (i * 1.5);
        qreal spread = i * 2.5;
        
        QColor layerGlow = glowColor;
        layerGlow.setAlpha(static_cast<int>(alpha));
        
        QPen glowPen(layerGlow);
        glowPen.setWidthF(spread);
        glowPen.setCapStyle(Qt::RoundCap);
        glowPen.setJoinStyle(Qt::RoundJoin);
        
        painter.setPen(glowPen);
        painter.drawText(position, text);
    }
    
    // Draw core text (solid)
    QPen corePen(baseColor);
    corePen.setWidthF(1.0);
    painter.setPen(corePen);
    painter.drawText(position, text);
}

void SplashScreen::drawTextShadow(QPainter& painter,
                                   const QString& text,
                                   const QFont& font,
                                   const QPointF& position,
                                   const QColor& shadowColor)
{
    painter.setFont(font);
    painter.setPen(shadowColor);
    painter.drawText(position, text);
}

} // namespace BummerGram
