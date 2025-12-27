#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QSlider>
#include <QLabel>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onMusicToggleClicked();
    void onVolumeChanged(int value);

private:
    void initUI();
    bool m_isMusicOn = true;

    QPushButton *m_musicBtn;
    QSlider *m_volumeSlider;
    QLabel *m_volumeLabel;
};

#endif // SETTINGSDIALOG_H
