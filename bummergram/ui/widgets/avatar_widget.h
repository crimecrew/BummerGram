#pragma once
/**
 * @file avatar_widget.h
 * @brief Avatar display widget with context menu
 */

#include <QWidget>
#include <QString>
#include <QPixmap>

class QLabel;
class QMenu;

namespace BummerGram {

/**
 * @class AvatarWidget
 * @brief Avatar display widget with context menu actions
 */
class AvatarWidget : public QWidget {
    Q_OBJECT

public:
    explicit AvatarWidget(QWidget* parent = nullptr);
    ~AvatarWidget() override;
    
    void setAvatar(const QString& filePath);
    void setAvatar(const QPixmap& pixmap);
    void setUserId(const QString& userId);
    void setUserName(const QString& userName);
    
    QSize sizeHint() const override;

signals:
    void copyDirectLinkClicked();
    void saveAvatarClicked();
    void avatarClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void copyDirectMediaLink();
    void saveProfileAvatar();

private:
    void setupUi();
    void updateDisplay();
    void loadAvatarFromCache();
    
    class Impl;
    struct ImplDeleter {
        void operator()(AvatarWidget::Impl* p) const;
    };
    
    std::unique_ptr<Impl, ImplDeleter> d;
};

} // namespace BummerGram
