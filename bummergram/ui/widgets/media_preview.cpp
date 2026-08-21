#include "media_preview.h"
#include "../../core/localstorage.h"

#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

class MediaPreview::Impl {
public:
    QLabel* previewLabel = nullptr;
    QLabel* progressLabel = nullptr;
    QLabel* typeBadge = nullptr;
    
    QString mediaPath;
    QString thumbnailPath;
    MediaType mediaType = MediaType::Image;
    
    QPixmap thumbnail;
    QImage originalImage;
    
    bool editable = false;
    qreal downloadProgress = -1.0;
    
    QMenu* contextMenu = nullptr;
    
    int maxWidth = 300;
    int maxHeight = 200;
};

void MediaPreview::ImplDeleter::operator()(MediaPreview::Impl* p) const {
    delete p;
}

} // namespace

// ============================================================================
// Construction
// ============================================================================

MediaPreview::MediaPreview(QWidget* parent)
    : QWidget(parent)
    , d(std::make_unique<Impl>())
{
    setupUi();
}

MediaPreview::~MediaPreview() = default;

// ============================================================================
// Setup
// ============================================================================

void MediaPreview::setupUi() {
    setMinimumSize(80, 60);
    setMaximumSize(600, 400);
    setCursor(Qt::PointingHandCursor);
    
    d->previewLabel = new QLabel(this);
    d->previewLabel->setAlignment(Qt::AlignCenter);
    d->previewLabel->setScaledContents(false);
    d->previewLabel->setWordWrap(true);
    
    d->progressLabel = new QLabel(this);
    d->progressLabel->setAlignment(Qt::AlignCenter);
    d->progressLabel->hide();
    
    d->typeBadge = new QLabel(this);
    d->typeBadge->setAlignment(Qt::AlignCenter);
    d->typeBadge->setStyleSheet(R"(
        QLabel {
            background-color: rgba(0, 0, 0, 0.7);
            color: white;
            padding: 2px 6px;
            border-radius: 3px;
            font-size: 10px;
        }
    )");
    
    // Create context menu
    d->contextMenu = new QMenu(this);
    
    d->contextMenu->addAction("View Full Size",
        this, &MediaPreview::showFullScreen);
    
    d->contextMenu->addSeparator();
    
    d->contextMenu->addAction("Copy",
        this, &MediaPreview::copyMediaClicked);
    
    d->contextMenu->addAction("Save As...",
        this, &MediaPreview::saveMediaClicked);
    
    d->contextMenu->addSeparator();
    
    d->contextMenu->addAction("Delete",
        this, &MediaPreview::deleteMediaClicked);
}

void MediaPreview::updateThumbnail() {
    if (!d->thumbnail.isNull()) {
        d->previewLabel->setPixmap(d->thumbnail);
    } else {
        d->previewLabel->setText("No preview");
    }
}

void MediaPreview::loadMediaThumbnail() {
    if (d->mediaPath.isEmpty() || !QFile::exists(d->mediaPath)) {
        d->previewLabel->setText("File not found");
        return;
    }
    
    switch (d->mediaType) {
        case MediaType::Image:
            loadImageThumbnail();
            break;
        case MediaType::Video:
            loadVideoThumbnail();
            break;
        default:
            showFileIcon();
            break;
    }
}

void MediaPreview::loadImageThumbnail() {
    QImageReader reader(d->mediaPath);
    reader.setAutoTransform(true);
    
    // Scale down for thumbnail
    QSize thumbSize = reader.size();
    thumbSize.scale(d->maxWidth, d->maxHeight, Qt::KeepAspectRatio);
    reader.setScaledSize(thumbSize);
    
    QImage img = reader.read();
    if (!img.isNull()) {
        d->originalImage = img;
        d->thumbnail = QPixmap::fromImage(img);
        d->previewLabel->setPixmap(d->thumbnail);
    } else {
        d->previewLabel->setText("Failed to load image");
    }
}

void MediaPreview::loadVideoThumbnail() {
    // Video thumbnails would need a video frame extraction library
    // For now, just show a placeholder
    QPixmap pm(100, 75);
    pm.fill(QColor(30, 30, 50));
    
    QPainter p(&pm);
    p.setPen(Qt::white);
    p.drawText(pm.rect(), Qt::AlignCenter, "▶ VIDEO");
    p.end();
    
    d->thumbnail = pm;
    d->previewLabel->setPixmap(d->thumbnail);
}

void MediaPreview::showFileIcon() {
    QString icon;
    QString label;
    
    switch (d->mediaType) {
        case MediaType::Document:
            icon = "📄";
            label = "Document";
            break;
        case MediaType::Audio:
            icon = "🎵";
            label = "Audio";
            break;
        case MediaType::Voice:
            icon = "🎤";
            label = "Voice";
            break;
        default:
            icon = "📎";
            label = "File";
            break;
    }
    
    QFileInfo fi(d->mediaPath);
    label = QString("%1\n%2")
        .arg(label)
        .arg(fi.fileName());
    
    d->previewLabel->setText(QString("%1\n%2").arg(icon).arg(label));
}

// ============================================================================
// Public Methods
// ============================================================================

void MediaPreview::setMedia(const QString& filePath, MediaType type) {
    d->mediaPath = filePath;
    d->mediaType = type;
    loadMediaThumbnail();
    
    // Update type badge
    QString typeText;
    switch (type) {
        case MediaType::Image: typeText = "IMG"; break;
        case MediaType::Video: typeText = "VID"; break;
        case MediaType::Document: typeText = "DOC"; break;
        case MediaType::Audio: typeText = "AUD"; break;
        case MediaType::Voice: typeText = "VC"; break;
    }
    d->typeBadge->setText(typeText);
}

void MediaPreview::setImage(const QImage& image) {
    d->originalImage = image;
    d->thumbnail = QPixmap::fromImage(image.scaled(
        d->maxWidth, d->maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation
    ));
    d->previewLabel->setPixmap(d->thumbnail);
}

void MediaPreview::setThumbnail(const QString& thumbPath) {
    if (QFile::exists(thumbPath)) {
        d->thumbnailPath = thumbPath;
        d->thumbnail.load(thumbPath);
        d->previewLabel->setPixmap(d->thumbnail);
    }
}

void MediaPreview::setEditable(bool editable) {
    d->editable = editable;
}

void MediaPreview::setDownloadProgress(qreal progress) {
    d->downloadProgress = progress;
    
    if (progress >= 0.0 && progress < 1.0) {
        d->progressLabel->setVisible(true);
        d->progressLabel->setText(QString("Downloading... %1%")
            .arg(int(progress * 100)));
    } else {
        d->progressLabel->setVisible(false);
    }
}

QSize MediaPreview::sizeHint() const {
    return QSize(d->maxWidth, d->maxHeight);
}

QSize MediaPreview::minimumSizeHint() const {
    return QSize(80, 60);
}

// ============================================================================
// Protected Methods
// ============================================================================

void MediaPreview::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit mediaClicked();
    }
    QWidget::mousePressEvent(event);
}

void MediaPreview::contextMenuEvent(QContextMenuEvent* event) {
    d->contextMenu->exec(event->globalPos());
}

void MediaPreview::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    
    // Reposition type badge
    d->typeBadge->move(4, 4);
    
    // Reposition progress label
    d->progressLabel->move(0, height() / 2 - d->progressLabel->height() / 2);
    d->progressLabel->resize(width(), 20);
}

// ============================================================================
// Public Slots
// ============================================================================

void MediaPreview::showFullScreen() {
    if (!d->originalImage.isNull()) {
        // Create a simple fullscreen viewer
        QLabel* viewer = new QLabel();
        viewer->setWindowFlags(Qt::Window);
        viewer->setScaledContents(true);
        viewer->setPixmap(QPixmap::fromImage(d->originalImage));
        viewer->showMaximized();
        viewer->show();
    }
}

} // namespace BummerGram
