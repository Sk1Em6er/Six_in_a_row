#include "settings_dialog.h"
#include "audio_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFont>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("游戏设置");
    setFixedSize(300, 150);
    setModal(true);
    move(parent->geometry().center() - rect().center());


    m_isMusicOn = AudioManager::instance()->isPlaying();

    initUI();


    double currentVol = AudioManager::instance()->getVolume();
    m_volumeSlider->setValue(qRound(currentVol * 100));
}

void SettingsDialog::initUI() {
    m_musicBtn = new QPushButton(this);
    m_musicBtn->setText(m_isMusicOn ? "关闭音乐" : "开启音乐");

    m_volumeLabel = new QLabel("音量调节：", this);
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setFixedWidth(200);

    QFont font("微软雅黑", 12);
    m_musicBtn->setFont(font);
    m_volumeLabel->setFont(font);
    m_volumeSlider->setFont(font);

    QString btnStyle =
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }";
    m_musicBtn->setStyleSheet(btnStyle);

    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(m_volumeLabel);
    volumeLayout->addWidget(m_volumeSlider);
    volumeLayout->setSpacing(10);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_musicBtn, 0, Qt::AlignCenter);
    mainLayout->addLayout(volumeLayout);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    setLayout(mainLayout);


    connect(m_musicBtn, &QPushButton::clicked, this, &SettingsDialog::onMusicToggleClicked);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &SettingsDialog::onVolumeChanged);
}

void SettingsDialog::onMusicToggleClicked() {
    m_isMusicOn = !m_isMusicOn;
    if (m_isMusicOn) {
        m_musicBtn->setText("关闭音乐");
        AudioManager::instance()->resumeBGM();
        QMessageBox::information(this, "提示", "背景音乐已开启！");
    } else {
        m_musicBtn->setText("开启音乐");
        AudioManager::instance()->pauseBGM();
        QMessageBox::information(this, "提示", "背景音乐已关闭！");
    }
}

void SettingsDialog::onVolumeChanged(int value) {
    double vol = value / 100.0;
    AudioManager::instance()->setVolume(vol);

    qDebug() << "[SettingsDialog] 全局音量调整为：" << value << "%";
}
