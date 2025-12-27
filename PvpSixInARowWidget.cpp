#include "PvpSixInARowWidget.h"
#include "ui_widget.h"
#include "audio_manager.h"
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDateTime>
#include <cstdlib>
#include <QStack> // 统一用QStack，删除<std::stack>
#include <QApplication>
#include <cstring>
#include "file_operation.h"
#include <ai.h>
#include "game_logic.h"


const int PvpSixInARowWidget::BOARDSIZE = 15;
const int PvpSixInARowWidget::BOARD_OFFSET = 20;
const int PvpSixInARowWidget::CONTROL_HEIGHT = 110;



PvpSixInARowWidget::PvpSixInARowWidget(QWidget *parent) : QWidget(parent),
    isGameOver(false), isPaused(false), isMusicOn(false),
    currentPlayer(BLACK), lastPlayer(EMPTY),
    hintX(-1), hintY(-1),
    m_labHint(nullptr), volumeLabel(nullptr), volumeSlider(nullptr),
    undoBtn(nullptr), restartBtn(nullptr), pauseBtn(nullptr), hintBtn(nullptr),
    musicBtn(nullptr), backToMenuBtn(nullptr), quitBtn(nullptr)
{
    setParent(parent);
    int boardPixelSize = cellSize * BOARD_SIZE + 40;
    setMinimumSize(boardPixelSize, boardPixelSize + 120);
    setFixedSize(boardPixelSize, boardPixelSize + 120);


    move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());


    init_board();

    initControlLayout();


    double globalVol = AudioManager::instance()->getVolume();
    volumeSlider->setValue(qRound(globalVol * 100));

    isMusicOn = AudioManager::instance()->isPlaying();
    musicBtn->setText(isMusicOn ? "关闭音乐" : "开启音乐");
    volumeSlider->setEnabled(isMusicOn);


    srand(time(NULL));
}


PvpSixInARowWidget::~PvpSixInARowWidget()
{

    QList<QWidget*> widgets = {backToMenuBtn, undoBtn, restartBtn, pauseBtn, hintBtn,
                                musicBtn, quitBtn, volumeLabel, volumeSlider, m_labHint};
    for (QWidget *w : widgets) {
        if (w) {
            w->deleteLater();
            w = nullptr;
        }
    }

    this->setUpdatesEnabled(false);
    this->disconnect();
}


void PvpSixInARowWidget::init_board()
{

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    undoStack.clear();
    isGameOver = false;
    isPaused = false;
    currentPlayer = BLACK;
    hintX = -1;
    hintY = -1;
}


bool PvpSixInARowWidget::isInUndoStack(int x, int y) {
    QStack<QPair<int, int>> tempStack = undoStack;
    while (!tempStack.isEmpty()) {
        QPair<int, int> pos = tempStack.top();
        tempStack.pop();
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}


void PvpSixInARowWidget::initControlLayout()
{

    m_labHint = new QLabel(this);
    m_labHint->setAlignment(Qt::AlignCenter);
    m_labHint->setStyleSheet("font-size: 14px; color: #333;");
    updateHintText();


    undoBtn = new QPushButton("悔棋", this);
    restartBtn = new QPushButton("重新开始", this);
    pauseBtn = new QPushButton("暂停", this);
    hintBtn = new QPushButton("AI提示", this);
    musicBtn = new QPushButton("关闭音乐", this);
    backToMenuBtn = new QPushButton("返回菜单", this);
    quitBtn = new QPushButton("退出游戏", this);


    volumeLabel = new QLabel("音量：", this);
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setEnabled(true);


    QString backBtnStyle =
        "QPushButton {"
        "   background-color: #ff9800;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover { background-color: #f57c00; }";
    backToMenuBtn->setStyleSheet(backBtnStyle);

    QString btnStyle =
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover { background-color: #1976D2; }";

    QString quitBtnStyle =
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 6px 12px;"
        "}"
        "QPushButton:hover { background-color: #d32f2f; }";


    undoBtn->setStyleSheet(btnStyle);
    restartBtn->setStyleSheet(btnStyle);
    pauseBtn->setStyleSheet(btnStyle);
    hintBtn->setStyleSheet(btnStyle);
    musicBtn->setStyleSheet(btnStyle);
    quitBtn->setStyleSheet(quitBtnStyle);


    QHBoxLayout *funcLayout = new QHBoxLayout();
    funcLayout->addWidget(undoBtn);
    funcLayout->addWidget(restartBtn);
    funcLayout->addWidget(pauseBtn);
    funcLayout->addWidget(hintBtn);
    funcLayout->addWidget(musicBtn);
    funcLayout->addWidget(backToMenuBtn);
    funcLayout->addWidget(quitBtn);
    funcLayout->setSpacing(8);


    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->setSpacing(5);


    QHBoxLayout *hintLayout = new QHBoxLayout();
    hintLayout->addWidget(m_labHint);


    QVBoxLayout *controlLayout = new QVBoxLayout();
    controlLayout->addLayout(hintLayout);
    controlLayout->addLayout(funcLayout);
    controlLayout->addLayout(volumeLayout);
    controlLayout->setSpacing(8);
    controlLayout->setContentsMargins(10, 10, 10, 10);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(controlLayout);


    QWidget *boardWidget = new QWidget(this);
    boardWidget->setFixedSize(cellSize * BOARD_SIZE + 40, cellSize * BOARD_SIZE + 20);
    mainLayout->addWidget(boardWidget);

    setLayout(mainLayout);


    connect(undoBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onUndoClicked);
    connect(restartBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onRestartClicked);
    connect(pauseBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onPauseClicked);
    connect(hintBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onHintClicked);
    connect(musicBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onMusicToggleClicked);
    connect(volumeSlider, &QSlider::valueChanged, this, &PvpSixInARowWidget::onVolumeChanged);
    connect(quitBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onQuitGameClicked);
    connect(backToMenuBtn, &QPushButton::clicked, this, &PvpSixInARowWidget::onBackToMenuClicked);
}


void PvpSixInARowWidget::onUndoClicked()
{
    if (undoStack.isEmpty() || isGameOver) {
        m_labHint->setText("无法撤销：无落子记录或游戏已结束！");
        return;
    }


    if (AudioManager::instance()) {
        AudioManager::instance()->playDropSound("undo.wav");
    }


    QPair<int, int> lastStep = undoStack.pop();
    board[lastStep.first][lastStep.second] = EMPTY;
    currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
    this->update();
    updateHintText();
    m_labHint->setText("已撤销上一步！");
}


void PvpSixInARowWidget::onRestartClicked()
{
    if (AudioManager::instance()) {
        AudioManager::instance()->playDropSound("restart.wav");
    }


    init_board();
    this->update();
    updateHintText();
    m_labHint->setText("游戏已重启！黑方先行");
}


void PvpSixInARowWidget::onBackToMenuClicked()
{
    int ret = QMessageBox::question(this, "返回确认", "确定返回主菜单吗？",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        emit backToMenu();
        this->close();
    }
}


void PvpSixInARowWidget::updateHintText()
{
    if (isGameOver) return;
    QString player = (currentPlayer == BLACK) ? "黑方" : "白方";
    m_labHint->setText(QString("当前回合：%1\n请点击棋盘落子").arg(player));
}


void PvpSixInARowWidget::posToIndex(int x, int y, int *ix, int *iy) {
    int boardLeft = BOARD_OFFSET;
    int boardTop = CONTROL_HEIGHT + BOARD_OFFSET;

    *ix = (x - boardLeft + cellSize/2) / cellSize;
    *iy = (y - boardTop + cellSize/2) / cellSize;


    *ix = qBound(0, *ix, BOARD_SIZE - 1);
    *iy = qBound(0, *iy, BOARD_SIZE - 1);
}


void PvpSixInARowWidget::saveStep(int x, int y) {
    undoStack.push(QPair<int, int>(x, y));
    lastPlayer = (lastPlayer == WHITE) ? BLACK : WHITE;
}


void PvpSixInARowWidget::onPauseClicked() {
    isPaused = !isPaused;
    if (isPaused) {
        pauseBtn->setText("继续");
        m_labHint->setText("游戏已暂停！点击“继续”恢复");
    } else {
        pauseBtn->setText("暂停");
        updateHintText();
    }
}


void PvpSixInARowWidget::onHintClicked() {
    if (isPaused || isGameOver) {
        QMessageBox::warning(this, "提示", "无法提示！（游戏暂停/结束）");
        return;
    }

    hintX = -1, hintY = -1;
    int bestScore = -1;


    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int score = score_position(i, j, WHITE);
                if (score > bestScore) {
                    bestScore = score;
                    hintX = i;
                    hintY = j;
                }
            }
        }
    }

    if (hintX == -1 || hintY == -1) {
        QMessageBox::warning(this, "提示", "无可用落子位置！");
        return;
    }


    update();

}


void PvpSixInARowWidget::onMusicToggleClicked() {
    isMusicOn = !isMusicOn;
    if (isMusicOn) {
        musicBtn->setText("关闭音乐");
        volumeSlider->setEnabled(true);
        if (AudioManager::instance()) {
            AudioManager::instance()->resumeBGM();
        }
        m_labHint->setText("音乐已开启！");
    } else {
        musicBtn->setText("开启音乐");
        volumeSlider->setEnabled(false);
        if (AudioManager::instance()) {
            AudioManager::instance()->pauseBGM();
        }
        m_labHint->setText("音乐已关闭！");
    }
}


void PvpSixInARowWidget::onVolumeChanged(int value) {
    double vol = value / 100.0;
    if (AudioManager::instance()) {
        AudioManager::instance()->setVolume(vol);
    }
    qDebug() << "[GameWidget] 全局音量调整为：" << value << "%";
}


void PvpSixInARowWidget::onQuitGameClicked() {
    int ret = QMessageBox::question(this, "确认退出",
                                    "是否确定退出游戏？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (AudioManager::instance()) {
            AudioManager::instance()->pauseBGM();
        }
        QApplication::quit();
    }
}


void PvpSixInARowWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor boardBgColor(205, 133, 63);
    painter.fillRect(this->rect(), boardBgColor);

    int boardLeft = BOARD_OFFSET;
    int boardTop = CONTROL_HEIGHT + BOARD_OFFSET;
    int boardSize = cellSize * (BOARD_SIZE - 1) + 2 * BOARD_OFFSET;


    painter.setPen(Qt::black);
    for (int i = 0; i < BOARD_SIZE; i++) {

        int y = boardTop + i * cellSize;
        painter.drawLine(boardLeft, y, boardLeft + boardSize - 2*BOARD_OFFSET, y);

        int x = boardLeft + i * cellSize;
        painter.drawLine(x, boardTop, x, boardTop + boardSize - 2*BOARD_OFFSET);

        for (int j = 0; j < BOARD_SIZE; j++) {
            int dotX = boardLeft + j * cellSize;
            int dotY = boardTop + i * cellSize;
            int dotRadius = 1;
            painter.setBrush(Qt::black);

            painter.drawEllipse(dotX - dotRadius, dotY - dotRadius,
                                2*dotRadius, 2*dotRadius);
        }
    }


    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY) continue;


            int x = boardLeft + i * cellSize;
            int y = boardTop + j * cellSize;
            int radius = cellSize / 2 - 2;


            if (board[i][j] == BLACK) {
                painter.setBrush(Qt::black);
                painter.drawEllipse(x - radius, y - radius, 2*radius, 2*radius);
            } else {
                painter.setBrush(Qt::white);
                painter.setPen(Qt::black);
                painter.drawEllipse(x - radius, y - radius, 2*radius, 2*radius);
            }
        }
    }


    if (hintX != -1 && hintY != -1) {
        int x = boardLeft + hintX * cellSize;
        int y = boardTop + hintY * cellSize;
        painter.setBrush(Qt::red);
        painter.drawEllipse(x - 5, y - 5, 10, 10);
    }
}


void PvpSixInARowWidget::mousePressEvent(QMouseEvent *event)
{

    if (isGameOver || isPaused || event->button() != Qt::LeftButton) {
        return;
    }


    QPoint mousePos = event->position().toPoint();
    int mouseX = mousePos.x();
    int mouseY = mousePos.y();


    int boardLeft = BOARD_OFFSET;
    int boardTop = CONTROL_HEIGHT + BOARD_OFFSET;
    int boardRight = boardLeft + cellSize * (BOARD_SIZE - 1)  +2*BOARD_OFFSET;
    int boardBottom = boardTop + cellSize * (BOARD_SIZE - 1)  +2*BOARD_OFFSET;

    if (mouseX < boardLeft || mouseX > boardRight ||
        mouseY < boardTop || mouseY > boardBottom) {
        return;
    }


    int ix = (mouseX - boardLeft + cellSize/2) / cellSize;
    int iy = (mouseY - boardTop + cellSize/2) / cellSize;


    if (ix < 0 || ix >= BOARD_SIZE || iy < 0 || iy >= BOARD_SIZE || board[ix][iy] != EMPTY) {
        return;
    }


    ChessType currentChess = currentPlayer;
    board[ix][iy] = currentChess;


    if (AudioManager::instance()) {
        AudioManager::instance()->playDropSound("drop_sound.wav");
    }


    saveStep(ix, iy);
    update();


    if (check_win(ix, iy, currentChess, board)) {
        gameOver(currentChess);
        return;
    }


    currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
    updateHintText();
}


void PvpSixInARowWidget::gameOver(ChessType winner) {
    isGameOver = true;
    QString msg = winner == WHITE ? "白方胜利！" : "黑方胜利！";
    m_labHint->setText(msg);
    QMessageBox::information(this, "游戏结束", msg, QMessageBox::Ok);


     GameRecord record;
     QString gameId = QString("%1_%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(rand() % 1000);
     strncpy(record.gameId, gameId.toUtf8().data(), sizeof(record.gameId)-1);
     record.gameId[sizeof(record.gameId)-1] = '\0';
     record.createTime = time(NULL);
     record.winner = winner;
     record.stepCount = 0;

     for (int i = 0; i < BOARD_SIZE; i++) {
         for (int j = 0; j < BOARD_SIZE; j++) {
             if (board[i][j] != EMPTY) record.stepCount++;
         }
     }

     save_game_record(&record);
}

