#include "audio_player.h"
#include <QDebug>

#ifdef _WIN32
    #include <windows.h>
    #pragma comment(lib, "winmm.lib")
#endif

namespace BummerGram {

class AudioPlayer::Impl {
public:
    qreal currentVolume = 0.7;
    bool playing = false;
    QString currentFile;
};

AudioPlayer::AudioPlayer(QObject* parent)
    : QObject(parent)
    , d(std::make_unique<Impl>())
{
}

AudioPlayer::~AudioPlayer() = default;

void AudioPlayer::play(const QString& filePath) {
#ifdef _WIN32
    PlaySound(
        reinterpret_cast<LPCWSTR>(filePath.utf16()),
        nullptr,
        SND_FILENAME | SND_ASYNC | SND_NODEFAULT
    );
    d->playing = true;
    d->currentFile = filePath;
    emit playbackStarted();
#else
    Q_UNUSED(filePath)
    qDebug() << "[AudioPlayer] Native playback not available on this platform";
#endif
}

void AudioPlayer::stop() {
#ifdef _WIN32
    PlaySound(nullptr, nullptr, 0);
    d->playing = false;
#endif
}

void AudioPlayer::setVolume(qreal volume) {
    d->currentVolume = qBound(0.0, volume, 1.0);
}

qreal AudioPlayer::volume() const {
    return d->currentVolume;
}

bool AudioPlayer::isPlaying() const {
    return d->playing;
}

} // namespace BummerGram
