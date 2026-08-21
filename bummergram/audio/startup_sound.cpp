#include "startup_sound.h"
#include "../core/localstorage.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>

#ifdef _WIN32
    #include <windows.h>
    #pragma comment(lib, "winmm.lib")
#endif

namespace BummerGram {

// ============================================================================
// Private Implementation
// ============================================================================

class StartupSoundPlayer::Impl {
public:
    std::unique_ptr<QSoundEffect> soundEffect;
    qreal currentVolume = 0.7;
    bool isPlayingFlag = false;
    QString lastError;
    
    // Default startup sound URL (example placeholder)
    const QString STARTUP_SOUND_URL = 
        "qrc:/sounds/startup.wav";
    
    // Fallback to bundled sound if available
    const QString BUNDLED_SOUND = ":/sounds/startup.wav";
};

} // namespace

// ============================================================================
// Singleton
// ============================================================================

StartupSoundPlayer& StartupSoundPlayer::instance() {
    static StartupSoundPlayer player;
    return player;
}

// ============================================================================
// Construction
// ============================================================================

StartupSoundPlayer::StartupSoundPlayer()
    : QObject(nullptr)
    , d(std::make_unique<Impl>())
{
    // Create sound effect
    d->soundEffect = std::make_unique<QSoundEffect>();
    
    // Set volume
    d->soundEffect->setVolume(d->currentVolume);
    
    // Connect signals
    connect(d->soundEffect.get(), &QSoundEffect::playingChanged,
            this, [this]() {
                d->isPlayingFlag = d->soundEffect->isPlaying();
                if (!d->isPlayingFlag) {
                    emit playbackFinished();
                }
            });
    
    // Set default source
    QString defaultSound = getDefaultSoundPath();
    if (QFile::exists(defaultSound)) {
        d->soundEffect->setSource(QUrl::fromLocalFile(defaultSound));
    } else {
        // Try bundled resource
        d->soundEffect->setSource(QUrl(BUNDLED_SOUND));
    }
    
    qDebug() << "[StartupSound] Initialized with source:" 
             << d->soundEffect->source();
}

StartupSoundPlayer::~StartupSoundPlayer() = default;

// ============================================================================
// Public Methods
// ============================================================================

void StartupSoundPlayer::playAsync() {
    qDebug() << "[StartupSound] Playing startup sound...";
    
    // Check if sound is loaded
    if (!d->soundEffect->isLoaded()) {
        qDebug() << "[StartupSound] Sound not loaded, attempting reload...";
        
        // Try to reload
        QString defaultSound = getDefaultSoundPath();
        if (QFile::exists(defaultSound)) {
            d->soundEffect->setSource(QUrl::fromLocalFile(defaultSound));
        }
    }
    
    // Check again
    if (d->soundEffect->isLoaded()) {
        d->soundEffect->play();
        emit playbackStarted();
        qDebug() << "[StartupSound] Playback started";
    } else {
        // Sound not available - that's okay, continue without audio
        qDebug() << "[StartupSound] Sound file not available, skipping audio";
        emit errorOccurred("Sound file not available");
    }
}

void StartupSoundPlayer::playFromFile(const QString& filePath) {
    qDebug() << "[StartupSound] Playing from file:" << filePath;
    
    if (!QFile::exists(filePath)) {
        QString error = QString("File not found: %1").arg(filePath);
        qWarning() << "[StartupSound]" << error;
        emit errorOccurred(error);
        return;
    }
    
    // Set new source
    QUrl fileUrl = QUrl::fromLocalFile(filePath);
    d->soundEffect->setSource(fileUrl);
    
    // Wait for load then play
    QTimer::singleShot(100, this, [this]() {
        if (d->soundEffect->isLoaded()) {
            d->soundEffect->play();
            emit playbackStarted();
        } else {
            // Try native API as fallback
            QString currentSource = d->soundEffect->source().toLocalFile();
            if (!currentSource.isEmpty()) {
                playUsingNativeAPI(currentSource);
            }
        }
    });
}

void StartupSoundPlayer::fetchAndPlay(const QUrl& url) {
    qDebug() << "[StartupSound] Fetch and play from URL:" << url;
    
    // For URL-based audio, we need to download first
    // This is a simplified implementation
    if (url.isLocalFile()) {
        playFromFile(url.toLocalFile());
        return;
    }
    
    // Network fetch would require QNetworkAccessManager
    // For now, fall back to default sound
    qDebug() << "[StartupSound] Network audio not implemented, using default";
    playAsync();
}

void StartupSoundPlayer::setVolume(qreal volume) {
    // Clamp volume to valid range
    d->currentVolume = qBound(0.0, volume, 1.0);
    d->soundEffect->setVolume(d->currentVolume);
    
    qDebug() << "[StartupSound] Volume set to:" << d->currentVolume;
}

qreal StartupSoundPlayer::volume() const {
    return d->currentVolume;
}

void StartupSoundPlayer::stop() {
    d->soundEffect->stop();
    qDebug() << "[StartupSound] Stopped";
}

bool StartupSoundPlayer::isPlaying() const {
    return d->soundEffect->isPlaying();
}

// ============================================================================
// Private Methods
// ============================================================================

void StartupSoundPlayer::playUsingNativeAPI(const QString& filePath) {
#ifdef _WIN32
    qDebug() << "[StartupSound] Using Windows native PlaySound API";
    
    // Play using Windows multimedia API
    // SND_FILENAME = play from file
    // SND_ASYNC = asynchronous playback
    // SND_NODEFAULT = don't play default sound on error
    BOOL result = PlaySound(
        reinterpret_cast<LPCWSTR>(filePath.utf16()),
        nullptr,
        SND_FILENAME | SND_ASYNC | SND_NODEFAULT
    );
    
    if (result) {
        emit playbackStarted();
    } else {
        DWORD error = GetLastError();
        QString errorMsg = QString("PlaySound failed with error: %1").arg(error);
        qWarning() << "[StartupSound]" << errorMsg;
        emit errorOccurred(errorMsg);
    }
#else
    Q_UNUSED(filePath)
    qDebug() << "[StartupSound] Native API not available on this platform";
#endif
}

QString StartupSoundPlayer::getDefaultSoundPath() const {
    // Check for custom user sound
    QString userSound = LocalStorage::getSoundsPath() + "/startup.wav";
    if (QFile::exists(userSound)) {
        return userSound;
    }
    
    // Check in app directory
    QString appDirSound = QCoreApplication::applicationDirPath() + "/sounds/startup.wav";
    if (QFile::exists(appDirSound)) {
        return appDirSound;
    }
    
    // Check standard locations
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath(),
        LocalStorage::getSoundsPath(),
        "/usr/share/bummergram/sounds",
        "/opt/bummergram/sounds"
    };
    
    for (const QString& path : searchPaths) {
        QString fullPath = path + "/startup.wav";
        if (QFile::exists(fullPath)) {
            return fullPath;
        }
    }
    
    return QString();
}

// ============================================================================
// Slots
// ============================================================================

void StartupSoundPlayer::onSoundLoaded() {
    qDebug() << "[StartupSound] Sound effect loaded";
}

void StartupSoundPlayer::onPlaybackFinished() {
    qDebug() << "[StartupSound] Playback finished";
    emit playbackFinished();
}

} // namespace BummerGram
