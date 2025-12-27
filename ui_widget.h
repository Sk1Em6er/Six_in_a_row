#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>
#include <cstdlib>
#include <stack>
#include <QSlider>
#include "game_logic.h"


const int CONTROL_HEIGHT = 100;
const int BOARD_OFFSET = 20;

class SixInARowWidget : public QWidget {
    Q_OBJECT
public:
    explicit SixInARowWidget(QWidget *parent = nullptr);
    ~SixInARowWidget() override;
    void setAIDifficulty(AIDifficulty diff) { aiDiff = diff; }

signals:
    void backToMenu();
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onDifficultyChanged(int index);
    void onUndoClicked();
    void onRestartClicked();
    void onPauseClicked();
    void onHintClicked();
    void onMusicToggleClicked();
    void onVolumeChanged(int value);
    void onBackToMenuClicked();
    void onQuitGameClicked();
    void forceDestroyWindow();

private:
    int cellSize = 40;
    AIDifficulty aiDiff = EASY;
    bool isGameOver = false;
    bool isPaused = false;
    ChessType lastPlayer = EMPTY;
    int hintX = -1, hintY = -1;
    bool isMusicOn = true;
    bool isGameRunning;
    bool m_isClosing = false;

    std::stack<std::pair<int, int>> undoStack;


    QComboBox *diffComboBox;
    QPushButton *undoBtn;
    QPushButton *restartBtn;
    QPushButton *pauseBtn;
    QPushButton *hintBtn;
    QPushButton *musicBtn;
    QPushButton *backToMenuBtn;
    QPushButton *quitBtn;
    QSlider *volumeSlider;
    QLabel *volumeLabel;

    SixInARowWidget *gameWidget = nullptr;

    void posToIndex(int x, int y, int *ix, int *iy);
    void gameOver(ChessType winner);
    void initControlLayout();
    void saveStep(int x, int y);
    bool isInUndoStack(int x, int y);
    void clearGameResources();
    void stopAllGameLogic();


};

#endif
