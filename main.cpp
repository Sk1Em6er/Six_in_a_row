#include <QApplication>
#include "menu_widget.h"
#include "ui_widget.h"
#include "audio_manager.h"
#include <QMessageBox>
#include <PvpSixInARowWidget.h>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    AudioManager::instance();


    MenuWidget menu;
    menu.setWindowTitle("六子棋 - 主菜单");
    menu.show();



    QObject::connect(&menu, &MenuWidget::startGame, [&]() {


        SixInARowWidget *game = new SixInARowWidget();

        game->show();


        QObject::connect(game, &SixInARowWidget::destroyed, &menu, [&]() {
            menu.show();
        });
    });

    QObject::connect(&menu, &MenuWidget::startPvpGame, [&]() {
        qDebug() << "开始双人对战(PVP)模式";


        PvpSixInARowWidget *pvpGame = new PvpSixInARowWidget();
        pvpGame->show();


        QObject::connect(pvpGame, &PvpSixInARowWidget::destroyed, &menu, [&]() {
            menu.show();
        });
    });


    QObject::connect(&menu, &MenuWidget::showHistory, [&]() {
        QMessageBox::information(nullptr, "历史记录", "暂无游戏记录！");
        menu.show();
    });

    return a.exec();
}
