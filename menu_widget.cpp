#include "menu_widget.h"
#include "audio_manager.h"
#include "ui_widget.h"
#include "settings_dialog.h"
#include <QFont>
#include <QGuiApplication>
#include <QScreen>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>
#include <history_widget.h>
#include "PvpSixInARowWidget.h"
#include "QPixmap"
MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent) {
    setWindowTitle("六子棋");
    setFixedSize(400, 400);


    QScreen *screen = QGuiApplication::primaryScreen();
    move(screen->geometry().center() - rect().center());


    AudioManager::instance()->playBGM("bgm.mp3");
    AudioManager::instance()->setVolume(0.5);

    initMenuLayout();
}

MenuWidget::~MenuWidget() {
    delete settingsDialog;

}

void MenuWidget::initMenuLayout() {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(50, 50, 50, 50);


    QLabel *titleLabel = new QLabel("六子棋", this);
    QFont titleFont("SimHei", 15, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);


    QFont btnFont("SimHei", 14);
    const int btnHeight = 40;


    QPushButton *startBtn = new QPushButton("人机对战", this);
    startBtn->setFont(btnFont);
    startBtn->setFixedHeight(btnHeight);
    connect(startBtn, &QPushButton::clicked, this, &MenuWidget::onStartGameClicked);
    mainLayout->addWidget(startBtn);

    QPushButton *startpvpBtn = new QPushButton("双人对战", this);
    startpvpBtn->setFont(btnFont);
    startpvpBtn->setFixedHeight(btnHeight);
    connect(startpvpBtn, &QPushButton::clicked, this, &MenuWidget::onStartPvpClicked);
    mainLayout->addWidget(startpvpBtn);


    QPushButton *settingsBtn = new QPushButton("游戏设置", this);
    settingsBtn->setFont(btnFont);
    settingsBtn->setFixedHeight(btnHeight);
    connect(settingsBtn, &QPushButton::clicked, this, &MenuWidget::onSettingsClicked);
    mainLayout->addWidget(settingsBtn);


    QPushButton *introBtn = new QPushButton("游戏介绍", this);
    introBtn->setFont(btnFont);
    introBtn->setFixedHeight(btnHeight);
    connect(introBtn, &QPushButton::clicked, this, &MenuWidget::onIntroClicked);
    mainLayout->addWidget(introBtn);


    QPushButton *historyBtn = new QPushButton("历史记录", this);
    historyBtn->setFont(btnFont);
    historyBtn->setFixedHeight(btnHeight);
    connect(historyBtn, &QPushButton::clicked, this, &MenuWidget::onHistoryClicked);
    mainLayout->addWidget(historyBtn);


    QPushButton *exitBtn = new QPushButton("退出游戏", this);
    exitBtn->setFont(btnFont);
    exitBtn->setFixedHeight(btnHeight);
    connect(exitBtn, &QPushButton::clicked, this, &MenuWidget::onQuitClicked);
    mainLayout->addWidget(exitBtn);


    mainLayout->addStretch();

    setLayout(mainLayout);
}



void MenuWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);



    QPixmap bgPixmap("res/menu_bg.jpg");


    if (!bgPixmap.isNull()) {
        painter.drawPixmap(this->rect(), bgPixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {

        painter.fillRect(this->rect(), QColor(245, 245, 245));
        qDebug() << "菜单背景图片加载失败！";
    }
}

void MenuWidget::onStartGameClicked() {
    AudioManager::instance()->playDropSound("click.wav");



    SixInARowWidget *aiGameWidget = new SixInARowWidget(nullptr);

    gameWidget = aiGameWidget;

    gameWidget->show();
    gameWidget->raise();
    gameWidget->activateWindow();
    this->hide();


    connect(aiGameWidget, &SixInARowWidget::finished, this, [this]() {
        this->showNormal();
        this->raise();
        this->activateWindow();
    });

    connect(aiGameWidget, &SixInARowWidget::backToMenu,this ,[this](){
        this->show();
    });



}

void MenuWidget::onStartPvpClicked() {
    AudioManager::instance()->playDropSound("click.wav");


    PvpSixInARowWidget *pvpGameWidget = new PvpSixInARowWidget(nullptr);
    gameWidget = pvpGameWidget;

    gameWidget->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    gameWidget->show();
    gameWidget->raise();
    gameWidget->activateWindow();
    this->hide();

    connect(pvpGameWidget, &PvpSixInARowWidget::finished, this, [this]() {
        this->showNormal();
        this->raise();
        this->activateWindow();
        gameWidget = nullptr;
    }, Qt::QueuedConnection);


    connect(gameWidget, &QWidget::destroyed, this, [this]() {
        gameWidget = nullptr;
    }, Qt::DirectConnection);

    connect(pvpGameWidget, &PvpSixInARowWidget::backToMenu, this, [this]() {
        this->show();
    });
}

void MenuWidget::onSettingsClicked() {
    AudioManager::instance()->playDropSound("click.wav");
    if (!settingsDialog || settingsDialog->isHidden()) {
        settingsDialog = new SettingsDialog(this);
        connect(settingsDialog, &SettingsDialog::finished, this, [this]() {
            settingsDialog->deleteLater();
            settingsDialog = nullptr;
        });
    }
    settingsDialog->show();
}


void MenuWidget::onIntroClicked() {
    AudioManager::instance()->playDropSound("click.wav");
    QString introText = "六子棋游戏规则：\n"
                        "1. 玩家操控白子，AI操控黑子；\n"
                        "2. 率先将6颗同色棋子连成一线（横/竖/斜）者获胜；\n"
                        "3. 点击棋盘落子，AI自动回应；\n"
                        "4. 支持悔棋、暂停、AI提示、音乐控制等功能。";
    QMessageBox::information(this, "游戏介绍", introText);
}


void MenuWidget::onHistoryClicked() {
    AudioManager::instance()->playDropSound("click.wav");


    if (historyWidget) {
        historyWidget->close();
        historyWidget->deleteLater();
        historyWidget = nullptr;
    }


    historyWidget = new HistoryWidget(this);
    historyWidget->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    historyWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    historyWidget->setAttribute(Qt::WA_QuitOnClose, false);


    historyWidget->show();
    historyWidget->raise();
    historyWidget->activateWindow();
    this->hide();


    connect(historyWidget, &HistoryWidget::destroyed, this, [this]() {
        this->showNormal();
        this->raise();
        this->activateWindow();
        historyWidget = nullptr;
    }, Qt::DirectConnection);
}


void MenuWidget::onQuitClicked() {
    AudioManager::instance()->playDropSound("click.wav");
    int result = QMessageBox::question(this, "退出游戏",
                                       "确定要退出六子棋吗？",
                                       QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        AudioManager::instance()->pauseBGM();
        QApplication::quit();
    }
}
