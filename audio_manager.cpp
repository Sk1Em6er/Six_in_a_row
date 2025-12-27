#include "audio_manager.h"
#include <QDebug>
#include <QFile>

AudioManager* AudioManager::instance() {
    static AudioManager instance;
    return &instance;
}

AudioManager::AudioManager(QObject *parent) : QObject(parent) {

    m_audioOutput = new QAudioOutput(this);
    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmPlayer->setAudioOutput(m_audioOutput);
    m_bgmPlayer->setLoops(QMediaPlayer::Infinite);


    m_dropOutput = new QAudioOutput(this);
    m_dropPlayer = new QMediaPlayer(this);
    m_dropPlayer->setAudioOutput(m_dropOutput);
    m_dropOutput->setVolume(0.8);
}

AudioManager::~AudioManager() {
    m_bgmPlayer->stop();
    m_dropPlayer->stop();
    delete m_bgmPlayer;
    delete m_audioOutput;
    delete m_dropPlayer;
    delete m_dropOutput;
}

void AudioManager::playBGM(const QString& path) {

    QFile file(path);
    if (!file.exists()) {
        qWarning() << "[AudioManager] BGM文件不存在：" << path;
        return;
    }


    if (m_bgmPlayer->source().toLocalFile() != path) {
        m_bgmPlayer->setSource(QUrl::fromLocalFile(path));
    }
    m_bgmPlayer->play();
    qDebug() << "[AudioManager] 播放BGM：" << path;
}

void AudioManager::pauseBGM() {
    if (m_bgmPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_bgmPlayer->pause();
        qDebug() << "[AudioManager] 暂停BGM";
    }
}

void AudioManager::resumeBGM() {
    if (m_bgmPlayer->playbackState() == QMediaPlayer::PausedState) {
        m_bgmPlayer->play();
        qDebug() << "[AudioManager] 恢复BGM";
    }
}

void AudioManager::setVolume(double volume) {
    double finalVol = qBound(0.0, volume, 1.0);
    m_audioOutput->setVolume(finalVol);
    qDebug() << "[AudioManager] 全局音量设置为：" << finalVol;
}

double AudioManager::getVolume() const {
    return m_audioOutput->volume();
}

bool AudioManager::isPlaying() const {
    return m_bgmPlayer->playbackState() == QMediaPlayer::PlayingState;
}

void AudioManager::playDropSound(const QString& path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "[AudioManager] 音效文件不存在：" << path;
        return;
    }
    m_dropPlayer->setSource(QUrl::fromLocalFile(path));
    m_dropPlayer->play();
    qDebug() << "[AudioManager] 播放落子音效：" << path;
}
