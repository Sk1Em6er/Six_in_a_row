#ifndef PVPSIXINAROWWIDGET_H
#define PVPSIXINAROWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStack>
#include <QPair>
#include <QSlider>
#include <QString>
#include <ctime>
#include "game_logic.h"



class PvpSixInARowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PvpSixInARowWidget(QWidget *parent = nullptr);
    ~PvpSixInARowWidget() override;


    static const int BOARDSIZE;
    static const int BOARD_OFFSET;
    static const int CONTROL_HEIGHT;

signals:
    void backToMenu();
    void finished();

private slots:
    void onUndoClicked();
    void onRestartClicked();
    void onPauseClicked();
    void onHintClicked();
    void onMusicToggleClicked();
    void onVolumeChanged(int value);
    void onBackToMenuClicked();
    void onQuitGameClicked();
    void forceDestroyWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:

    int cellSize = 40;
    bool isGameOver = false;
    bool isPaused = false;
    bool isMusicOn = true;
    int hintX = -1, hintY = -1;
    ChessType currentPlayer = BLACK;
    ChessType lastPlayer = EMPTY;

    ChessType board[15][15] = {{EMPTY}};

    QPushButton *undoBtn = nullptr;
    QPushButton *restartBtn = nullptr;
    QPushButton *pauseBtn = nullptr;
    QPushButton *hintBtn = nullptr;
    QPushButton *musicBtn = nullptr;
    QPushButton *backToMenuBtn = nullptr;
    QPushButton *quitBtn = nullptr;
    QSlider *volumeSlider = nullptr;
    QLabel *volumeLabel = nullptr;
    QLabel *m_labHint = nullptr;
    QStack<QPair<int, int>> undoStack;


    void init_board();
    void posToIndex(int x, int y, int *ix, int *iy);
    void gameOver(ChessType winner);
    void initControlLayout();
    void updateHintText();
    void saveStep(int x, int y);
    bool isInUndoStack(int x, int y);
};

#endif
