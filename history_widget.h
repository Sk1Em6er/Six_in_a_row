#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <QGuiApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include "file_operation.h"

class HistoryWidget : public QWidget {
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    ~HistoryWidget() override = default;

signals:
    void backToMenu();

private slots:

    void onLoadHistory();
    void onDeleteSelected();
    void onBackMenu();

private:
    QListWidget *historyList;
    QPushButton *loadBtn;
    QPushButton *deleteBtn;
    QPushButton *backBtn;

    void initHistoryLayout();
};

#endif // HISTORY_WIDGET_H
