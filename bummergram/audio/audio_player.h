#pragma once
/**
 * @file audio_player.h
 * @brief Audio player for BummerGram
 */

#include <QObject>
#include <QString>
#include <memory>

namespace BummerGram {

class AudioPlayer : public QObject {
    Q_OBJECT

public:
    explicit AudioPlayer(QObject* parent = nullptr);
    ~AudioPlayer() override;
    
    void play(const QString& filePath);
    void stop();
    void setVolume(qreal volume);
    qreal volume() const;
    bool isPlaying() const;
    
signals:
    void playbackStarted();
    void playbackFinished();
    void error(const QString& message);

private:
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
