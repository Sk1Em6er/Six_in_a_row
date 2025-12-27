#include "history_widget.h"
#include <QFont>
#include <QDir>
#include <QFileInfoList>
#include <QScreen>
#include <audio_manager.h>

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent) {
    setWindowTitle("六子棋 - 游戏历史");
    setFixedSize(600, 400);
    move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());
    
    initHistoryLayout();

    onLoadHistory();
}

void HistoryWidget::initHistoryLayout() {

    QLabel *titleLabel = new QLabel("游戏历史记录", this);
    QFont titleFont("微软雅黑", 20, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);


    historyList = new QListWidget(this);
    historyList->setFont(QFont("微软雅黑", 12));
    historyList->setStyleSheet("QListWidget { border: 1px solid #ccc; border-radius: 5px; }");


    loadBtn = new QPushButton("刷新记录", this);
    deleteBtn = new QPushButton("删除选中", this);
    backBtn = new QPushButton("返回菜单", this);


    QString btnStyle = 
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }";
    loadBtn->setStyleSheet(btnStyle);
    deleteBtn->setStyleSheet(btnStyle);
    backBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 10px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }"
    );


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *btnLayout = new QHBoxLayout();

    btnLayout->addWidget(loadBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(backBtn);
    btnLayout->setSpacing(20);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(historyList);
    mainLayout->addLayout(btnLayout);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(20);
    setLayout(mainLayout);


    connect(loadBtn, &QPushButton::clicked, this, &HistoryWidget::onLoadHistory);
    connect(deleteBtn, &QPushButton::clicked, this, &HistoryWidget::onDeleteSelected);
    connect(backBtn, &QPushButton::clicked, this, &HistoryWidget::onBackMenu);
}


void HistoryWidget::onLoadHistory() {
    historyList->clear();

    QDir dir("game_records");
    if (!dir.exists()) {
        QMessageBox::information(this, "提示", "暂无游戏记录！");
        return;
    }

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &file : fileList) {
        if (file.suffix() == "txt") {

            GameRecord record;
            load_game_record(file.baseName().toUtf8().data(), &record);
            

            QString winner = (record.winner == WHITE) ? "玩家" : "AI";
            QString itemText = QString("游戏ID：%1 | 获胜方：%2 | 步数：%3 | 时间：%4")
                                 .arg(record.gameId)
                                 .arg(winner)
                                 .arg(record.stepCount)
                                 .arg(QDateTime::fromSecsSinceEpoch(record.createTime).toString("yyyy-MM-dd hh:mm:ss"));
            historyList->addItem(itemText);
        }
    }

    if (historyList->count() == 0) {
        QMessageBox::information(this, "提示", "暂无游戏记录！");
    }
}


void HistoryWidget::onDeleteSelected() {
    QListWidgetItem *selectedItem = historyList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "提示", "请选中要删除的记录！");
        return;
    }


    QString text = selectedItem->text();
    QString gameId = text.split("|").first().split("：").last().trimmed();
    

    if (delete_game_record(gameId.toUtf8().data())) {
        QMessageBox::information(this, "提示", "记录删除成功！");
        onLoadHistory();
    } else {
        QMessageBox::warning(this, "提示", "记录删除失败！");
    }
}


void HistoryWidget::onBackMenu() {
    AudioManager::instance()->playDropSound("click.wav");
    this->close();
}
