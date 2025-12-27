#ifndef MENU_WIDGET_H
#define MENU_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <history_widget.h>


class SettingsDialog;
class SixInARowWidget;

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

    void startGame();

    void startPvpGame();
    void showHistory();

private slots:
    void onStartGameClicked();
    void onStartPvpClicked();
    void onIntroClicked();
    void onSettingsClicked();
    void onHistoryClicked();
    void onQuitClicked();
    void initMenuLayout();


private:


    // 成员变量
    SettingsDialog *settingsDialog = nullptr;

    QWidget *gameWidget = nullptr;
    HistoryWidget *historyWidget = nullptr;
};

#endif // MENU_WIDGET_H
