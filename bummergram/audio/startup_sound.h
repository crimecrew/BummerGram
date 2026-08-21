#pragma once
/**
 * @file startup_sound.h
 * @brief Startup audio player for BummerGram
 * 
 * Handles asynchronous audio playback using QSoundEffect
 * with fallback to native Windows multimedia API.
 */

#include <QObject>
#include <QString>
#include <QUrl>
#include <QAudioOutput>
#include <QSoundEffect>
#include <memory>

namespace BummerGram {

/**
 * @class StartupSoundPlayer
 * @brief Manages startup sound playback
 * 
 * Singleton class that plays the startup sound when the
 * splash screen appears. Uses QSoundEffect with fallback
 * to native Windows PlaySound API.
 */
class StartupSoundPlayer : public QObject {
    Q_OBJECT

public:
    // ========================================================================
    // Singleton Access
    // ========================================================================
    
    /**
     * @brief Get the singleton instance
     */
    static StartupSoundPlayer& instance();
    
    // Prevent copying
    StartupSoundPlayer(const StartupSoundPlayer&) = delete;
    StartupSoundPlayer& operator=(const StartupSoundPlayer&) = delete;

    // ========================================================================
    // Playback Control
    // ========================================================================
    
    /**
     * @brief Play the default startup sound asynchronously
     */
    void playAsync();
    
    /**
     * @brief Play a sound from a local file
     * @param filePath Path to the audio file
     */
    void playFromFile(const QString& filePath);
    
    /**
     * @brief Fetch audio from URL and play
     * @param url URL to fetch audio from
     */
    void fetchAndPlay(const QUrl& url);
    
    /**
     * @brief Set playback volume (0.0 to 1.0)
     */
    void setVolume(qreal volume);
    
    /**
     * @brief Stop current playback
     */
    void stop();
    
    /**
     * @brief Check if currently playing
     */
    bool isPlaying() const;
    
    /**
     * @brief Get the volume level
     */
    qreal volume() const;

signals:
    /**
     * @brief Emitted when playback starts
     */
    void playbackStarted();
    
    /**
     * @brief Emitted when playback finishes
     */
    void playbackFinished();
    
    /**
     * @brief Emitted on playback error
     * @param error Error message
     */
    void errorOccurred(const QString& error);

public slots:
    /**
     * @brief Handle sound effect loading completed
     */
    void onSoundLoaded();
    
    /**
     * @brief Handle playback finished
     */
    void onPlaybackFinished();

private:
    /**
     * @brief Private constructor for singleton
     */
    StartupSoundPlayer();
    
    /**
     * @brief Destructor
     */
    ~StartupSoundPlayer();
    
    // ========================================================================
    // Platform-Specific
    // ========================================================================
    
    /**
     * @brief Play using native Windows multimedia API
     * @param filePath Path to audio file
     */
    void playUsingNativeAPI(const QString& filePath);
    
    /**
     * @brief Get the default startup sound path
     */
    QString getDefaultSoundPath() const;

    // ========================================================================
    // Member Variables
    // ========================================================================
    
    class Impl;
    std::unique_ptr<Impl> d;
};

} // namespace BummerGram
