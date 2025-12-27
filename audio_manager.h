#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

class AudioManager : public QObject {
    Q_OBJECT
public:
    static AudioManager* instance();
    ~AudioManager() override;

    // 背景音乐控制
    void playBGM(const QString& path);
    void pauseBGM();
    void resumeBGM();
    void setVolume(double volume);
    double getVolume() const;
    bool isPlaying() const;


    void playDropSound(const QString& path);

private:
    explicit AudioManager(QObject *parent = nullptr);
    AudioManager(const AudioManager&) = delete;
    void operator=(const AudioManager&) = delete;

    // 全局背景音乐
    QMediaPlayer *m_bgmPlayer;
    QAudioOutput *m_audioOutput;

    // 落子音效（独立）
    QMediaPlayer *m_dropPlayer;
    QAudioOutput *m_dropOutput;
};

#endif // AUDIOMANAGER_H
