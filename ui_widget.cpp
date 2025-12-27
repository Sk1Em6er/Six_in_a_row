#include "ui_widget.h"
#include "audio_manager.h"
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDateTime>
#include <cstdlib>
#include <stack>
#include <QApplication>
#include <cstring>
#include "file_operation.h"
#include <ai.h>




SixInARowWidget::SixInARowWidget(QWidget *parent)
    : QWidget(parent)
    ,isGameRunning(false)
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
}

SixInARowWidget::~SixInARowWidget()
{
    if (backToMenuBtn) {
        backToMenuBtn->deleteLater();
        backToMenuBtn = nullptr;
    }
    if (undoBtn) {
        undoBtn->deleteLater();
        undoBtn = nullptr;
    }
    if (restartBtn) {
        restartBtn->deleteLater();
        restartBtn = nullptr;
    }

    this->setUpdatesEnabled(false);

    this->disconnect();
}


bool SixInARowWidget::isInUndoStack(int x, int y) {
    std::stack<std::pair<int, int>> tempStack = undoStack;
    while (!tempStack.empty()) {
        std::pair<int, int> pos = tempStack.top();
        tempStack.pop();
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}


void SixInARowWidget::initControlLayout() {

    QLabel *diffLabel = new QLabel("AI难度：", this);
    diffComboBox = new QComboBox(this);
    diffComboBox->addItem("简单", EASY);
    diffComboBox->addItem("中等", MEDIUM);
    diffComboBox->addItem("困难", HARD);
    diffComboBox->addItem("专家", EXPERT);
    diffComboBox->setCurrentIndex(0);


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

    // 4. 样式美化
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


    QHBoxLayout *diffLayout = new QHBoxLayout();
    diffLayout->addWidget(diffLabel);
    diffLayout->addWidget(diffComboBox);


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


    QVBoxLayout *controlLayout = new QVBoxLayout();
    controlLayout->addLayout(diffLayout);
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


    connect(diffComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SixInARowWidget::onDifficultyChanged);
    connect(undoBtn, &QPushButton::clicked, this, &SixInARowWidget::onUndoClicked);
    connect(restartBtn, &QPushButton::clicked, this, &SixInARowWidget::onRestartClicked);
    connect(pauseBtn, &QPushButton::clicked, this, &SixInARowWidget::onPauseClicked);
    connect(hintBtn, &QPushButton::clicked, this, &SixInARowWidget::onHintClicked);
    connect(musicBtn, &QPushButton::clicked, this, &SixInARowWidget::onMusicToggleClicked);
    connect(volumeSlider, &QSlider::valueChanged, this, &SixInARowWidget::onVolumeChanged);
    connect(quitBtn, &QPushButton::clicked, this, &SixInARowWidget::onQuitGameClicked);
    connect(backToMenuBtn, &QPushButton::clicked, this, &SixInARowWidget::onBackToMenuClicked);
}


void SixInARowWidget::saveStep(int x, int y) {
    undoStack.push(std::make_pair(x, y));
    lastPlayer = (lastPlayer == WHITE) ? BLACK : WHITE;
}


void SixInARowWidget::onUndoClicked() {
    if (isPaused || isGameOver || undoStack.empty()) {
        QMessageBox::warning(this, "提示", "无法悔棋！（游戏暂停/结束/无步骤）");
        return;
    }


    std::pair<int, int> lastStep = undoStack.top();
    undoStack.pop();
    board[lastStep.first][lastStep.second] = EMPTY;


    if (!undoStack.empty()) {
        std::pair<int, int> aiStep = undoStack.top();
        undoStack.pop();
        board[aiStep.first][aiStep.second] = EMPTY;
    }

    isGameOver = false;
    hintX = -1;
    hintY = -1;
    update();

}


void SixInARowWidget::onRestartClicked() {

    init_board();

    while (!undoStack.empty()) undoStack.pop();

    isGameOver = false;
    isPaused = false;
    hintX = -1;
    hintY = -1;
    pauseBtn->setText("暂停");

    update();
}


void SixInARowWidget::onPauseClicked() {
    isPaused = !isPaused;
    if (isPaused) {
        pauseBtn->setText("继续");
        QMessageBox::information(this, "提示", "游戏已暂停！");
    } else {
        pauseBtn->setText("暂停");
        QMessageBox::information(this, "提示", "游戏已继续！");
    }
}


void SixInARowWidget::onHintClicked() {
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


void SixInARowWidget::onMusicToggleClicked() {
    isMusicOn = !isMusicOn;
    if (isMusicOn) {
        musicBtn->setText("关闭音乐");
        volumeSlider->setEnabled(true);
        AudioManager::instance()->resumeBGM(); // 恢复全局音乐
        QMessageBox::information(this, "提示", "音乐已开启！");
    } else {
        musicBtn->setText("开启音乐");
        volumeSlider->setEnabled(false);
        AudioManager::instance()->pauseBGM(); // 暂停全局音乐
        QMessageBox::information(this, "提示", "音乐已关闭！");
    }
}


void SixInARowWidget::onVolumeChanged(int value) {
    double vol = value / 100.0;
    AudioManager::instance()->setVolume(vol);
    qDebug() << "[GameWidget] 全局音量调整为：" << value << "%";
}


void SixInARowWidget::onQuitGameClicked() {
    int ret = QMessageBox::question(this, "确认退出",
                                    "是否确定退出游戏？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        AudioManager::instance()->pauseBGM();
        QApplication::quit();
    }
}


void SixInARowWidget::onDifficultyChanged(int index) {
    AIDifficulty newDiff = static_cast<AIDifficulty>(index);
    if (newDiff < EASY || newDiff > EXPERT) return;

    aiDiff = newDiff;
    QString diffName = diffComboBox->currentText();
    qDebug() << "[GameWidget] AI难度切换为：" << diffName;


    if (!isGameOver && !isPaused) {
        if (QMessageBox::question(this, "提示",
                                  QString("难度已切换为%1，是否重新开始？").arg(diffName),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            onRestartClicked();
        }
    }
}


void SixInARowWidget::onBackToMenuClicked()
{
    if (m_isClosing) return;
    m_isClosing = true;

    int ret = QMessageBox::question(this, "返回确认", "确定返回主菜单吗？",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {

        emit backToMenu();
        this->close();
    } else {
        m_isClosing = false;
    }
}

void SixInARowWidget::mousePressEvent(QMouseEvent *event) {

    if (isGameOver || isPaused || event->button() != Qt::LeftButton) return;


    QPoint mousePos = event->position().toPoint();
    int mouseX = mousePos.x();
    int mouseY = mousePos.y();


    int boardLeft = BOARD_OFFSET;
    int boardTop = CONTROL_HEIGHT + BOARD_OFFSET;
    int boardRight = boardLeft + cellSize * (BOARD_SIZE - 1) + 2*BOARD_OFFSET;
    int boardBottom = boardTop + cellSize * (BOARD_SIZE - 1) + 2*BOARD_OFFSET;


    if (mouseX < boardLeft || mouseX > boardRight ||
        mouseY < boardTop || mouseY > boardBottom) {
        return;
    }


    int ix = (mouseX - boardLeft + cellSize/2) / cellSize;
    int iy = (mouseY - boardTop + cellSize/2) / cellSize;


    if (ix < 0 || ix >= BOARD_SIZE || iy < 0 || iy >= BOARD_SIZE) return;
    if (board[ix][iy] != EMPTY) return;


    hintX = -1;
    hintY = -1;


    board[ix][iy] = WHITE;

    AudioManager::instance()->playDropSound("drop_sound.wav");

    saveStep(ix, iy);

    update();


    if (check_win(ix, iy, WHITE,board)) {
        gameOver(WHITE);
        return;
    }


    int aiX = -1, aiY = -1;
    ai_play(aiDiff);


    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == BLACK && !isInUndoStack(i, j)) {
                aiX = i;
                aiY = j;
                break;
            }
        }
        if (aiX != -1) break;
    }


    if (aiX != -1 && aiY != -1) {

        saveStep(aiX, aiY);

        AudioManager::instance()->playDropSound("drop_sound.wav");

        update();


        if (check_win(aiX, aiY, BLACK,board)) {
            gameOver(BLACK);
            return;
        }
    }
}


void SixInARowWidget::paintEvent(QPaintEvent *event) {
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


void SixInARowWidget::posToIndex(int x, int y, int *ix, int *iy) {
    int boardLeft = BOARD_OFFSET;
    int boardTop = CONTROL_HEIGHT + BOARD_OFFSET;

    *ix = (x - boardLeft + cellSize/2) / cellSize;
    *iy = (y - boardTop + cellSize/2) / cellSize;


    *ix = qBound(0, *ix, BOARD_SIZE - 1);
    *iy = qBound(0, *iy, BOARD_SIZE - 1);
}


void SixInARowWidget::gameOver(ChessType winner) {
    isGameOver = true;
    QString msg = winner == WHITE ? "玩家胜利！" : "AI胜利！";
    QMessageBox::information(this, "游戏结束", msg);


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

void SixInARowWidget::clearGameResources()
{

    isGameRunning = false;
    this->disconnect();
}

void SixInARowWidget::closeEvent(QCloseEvent *event) {
    stopAllGameLogic();
    clearGameResources();

    backToMenuBtn = nullptr;
    undoBtn = nullptr;
    restartBtn = nullptr;

    m_isClosing = true;
    event->accept();


    this->deleteLater();
}

void SixInARowWidget::forceDestroyWindow()
{

    this->close();


    this->deleteLater();

}

void SixInARowWidget::stopAllGameLogic() {


    this->setUpdatesEnabled(false);

    QApplication::processEvents(QEventLoop::AllEvents, 200);

    this->disconnect();
}
