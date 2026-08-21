#pragma once
/**
 * @file media_preview.h
 * @brief Media preview widget with context menu
 */

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QImage>

class QLabel;
class QMenu;
class QPushButton;

namespace BummerGram {

enum class MediaType {
    Image,
    Video,
    Document,
    Audio,
    Voice
};

/**
 * @class MediaPreview
 * @brief Preview widget for media items with context menu
 */
class MediaPreview : public QWidget {
    Q_OBJECT

public:
    explicit MediaPreview(QWidget* parent = nullptr);
    ~MediaPreview() override;
    
    void setMedia(const QString& filePath, MediaType type);
    void setImage(const QImage& image);
    void setThumbnail(const QString& thumbPath);
    
    void setEditable(bool editable);
    void setDownloadProgress(qreal progress);
    
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void mediaClicked();
    void saveMediaClicked();
    void copyMediaClicked();
    void deleteMediaClicked();

public slots:
    void showFullScreen();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUi();
    void loadMediaThumbnail();
    void updateThumbnail();
    
    class Impl;
    struct ImplDeleter {
        void operator()(MediaPreview::Impl* p) const;
    };
    
    std::unique_ptr<Impl, ImplDeleter> d;
};

} // namespace BummerGram
