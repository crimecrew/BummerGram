#include "avatar_widget.h"
#include "../../core/localstorage.h"

#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QFileDialog>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QFile>

#ifdef _WIN32
    #include <windows.h>
    #pragma comment(lib, "shell32.lib")
#endif

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

class AvatarWidget::Impl {
public:
    QLabel* avatarLabel = nullptr;
    QString userId;
    QString userName;
    QString avatarFilePath;
    QPixmap currentAvatar;
    QPixmap placeholderAvatar;
    
    QMenu* contextMenu = nullptr;
    QAction* copyLinkAction = nullptr;
    QAction* saveAction = nullptr;
    
    int avatarSize = 40;
};

void AvatarWidget::ImplDeleter::operator()(AvatarWidget::Impl* p) const {
    delete p;
}

} // namespace

// ============================================================================
// Construction
// ============================================================================

AvatarWidget::AvatarWidget(QWidget* parent)
    : QWidget(parent)
    , d(std::make_unique<Impl>())
{
    setMinimumSize(40, 40);
    setMaximumSize(200, 200);
    setCursor(Qt::PointingHandCursor);
    
    setupUi();
}

AvatarWidget::~AvatarWidget() = default;

// ============================================================================
// Setup
// ============================================================================

void AvatarWidget::setupUi() {
    d->avatarLabel = new QLabel(this);
    d->avatarLabel->setAlignment(Qt::AlignCenter);
    d->avatarLabel->setScaledContents(true);
    
    // Create context menu
    d->contextMenu = new QMenu(this);
    
    d->copyLinkAction = d->contextMenu->addAction("Copy Direct Media Link");
    connect(d->copyLinkAction, &QAction::triggered,
            this, &AvatarWidget::copyDirectMediaLink);
    
    d->saveAction = d->contextMenu->addAction("Save Profile Avatar...");
    connect(d->saveAction, &QAction::triggered,
            this, &AvatarWidget::saveProfileAvatar);
    
    // Create placeholder avatar (colored circle with initials)
    generatePlaceholder();
    d->avatarLabel->setPixmap(d->placeholderAvatar);
}

void AvatarWidget::generatePlaceholder() {
    // Create a simple placeholder avatar
    int size = d->avatarSize * 2;
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    
    // Draw circle background
    QColor bgColor(15, 52, 96); // Dark blue
    p.setBrush(bgColor);
    p.setPen(Qt::NoPen);
    p.drawEllipse(0, 0, size, size);
    
    // Draw initials
    if (!d->userName.isEmpty()) {
        QString initials;
        const QChar* chars = d->userName.unicode();
        int len = d->userName.length();
        
        if (len > 0) {
            initials.append(chars[0].toUpper());
            if (len > 1) {
                // Find last space
                for (int i = len - 1; i >= 0; --i) {
                    if (chars[i].isSpace()) {
                        if (i + 1 < len) {
                            initials.append(chars[i + 1].toUpper());
                        }
                        break;
                    }
                }
            }
        }
        
        if (initials.length() == 1 && len > 1) {
            for (int i = 1; i < len && initials.length() < 2; ++i) {
                if (chars[i].isSpace()) {
                    initials.append(chars[i + 1].toUpper());
                }
            }
        }
        
        if (initials.isEmpty()) {
            initials = "?";
        }
        
        p.setPen(Qt::white);
        p.setFont(QFont("Segoe UI", size / 3, QFont::Bold));
        p.drawText(pm.rect(), Qt::AlignCenter, initials);
    }
    
    p.end();
    d->placeholderAvatar = pm;
}

void AvatarWidget::updateDisplay() {
    if (d->currentAvatar.isNull()) {
        d->avatarLabel->setPixmap(d->placeholderAvatar);
    } else {
        d->avatarLabel->setPixmap(d->currentAvatar);
    }
}

// ============================================================================
// Public Methods
// ============================================================================

void AvatarWidget::setAvatar(const QString& filePath) {
    d->avatarFilePath = filePath;
    
    if (QFile::exists(filePath)) {
        d->currentAvatar.load(filePath);
        d->currentAvatar = d->currentAvatar.scaled(
            d->avatarSize * 2, d->avatarSize * 2,
            Qt::KeepAspectRatio, Qt::SmoothTransformation
        );
    }
    
    updateDisplay();
}

void AvatarWidget::setAvatar(const QPixmap& pixmap) {
    d->currentAvatar = pixmap;
    updateDisplay();
}

void AvatarWidget::setUserId(const QString& userId) {
    d->userId = userId;
    loadAvatarFromCache();
}

void AvatarWidget::setUserName(const QString& userName) {
    d->userName = userName;
    generatePlaceholder();
    updateDisplay();
}

QSize AvatarWidget::sizeHint() const {
    return QSize(d->avatarSize, d->avatarSize);
}

void AvatarWidget::loadAvatarFromCache() {
    if (d->userId.isEmpty()) return;
    
    QString cacheDir = LocalStorage::getAvatarCachePath();
    QString avatarPath = QString("%1/%2.jpg").arg(cacheDir, d->userId);
    
    if (QFile::exists(avatarPath)) {
        setAvatar(avatarPath);
    }
}

// ============================================================================
// Protected Methods
// ============================================================================

void AvatarWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit avatarClicked();
    }
    QWidget::mousePressEvent(event);
}

void AvatarWidget::contextMenuEvent(QContextMenuEvent* event) {
    d->contextMenu->exec(event->globalPos());
}

// ============================================================================
// Private Slots
// ============================================================================

void AvatarWidget::copyDirectMediaLink() {
    if (d->userId.isEmpty()) {
        qDebug() << "[AvatarWidget] No user ID set for media link";
        return;
    }
    
    // Generate a media link (placeholder format)
    QString mediaLink = QString("bummergram://media/avatar/%1").arg(d->userId);
    
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(mediaLink);
    
    qDebug() << "[AvatarWidget] Copied media link:" << mediaLink;
    emit copyDirectLinkClicked();
}

void AvatarWidget::saveProfileAvatar() {
    if (d->currentAvatar.isNull()) {
        qDebug() << "[AvatarWidget] No avatar to save";
        return;
    }
    
    QString defaultName = d->userName.isEmpty() ? 
        QString("avatar_%1.png").arg(d->userId) :
        QString("%1_avatar.png").arg(d->userName);
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Save Avatar",
        defaultName,
        "Images (*.png *.jpg *.jpeg)"
    );
    
    if (!filePath.isEmpty()) {
        d->currentAvatar.save(filePath);
        qDebug() << "[AvatarWidget] Saved avatar to:" << filePath;
        emit saveAvatarClicked();
    }
}

} // namespace BummerGram
