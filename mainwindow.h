#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QScrollBar>
#include <QTimer>
#include "gamefield.h"
#include "ballwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onCellClicked(QPoint cell);
    void onAnimationDone(QPoint from, QPoint to);
    void onNewGame();

private:
    Ui::MainWindow* ui;
    GameField*   game     = nullptr;
    BallWidget*  field    = nullptr;
    QPoint       selected = {-1,-1};

    QLabel*       scoreLabel;
    QLabel*       recordLabel;
    QTableWidget* nextTable;
    QSpinBox*     rowsSpin;
    QSpinBox*     colsSpin;
    QSpinBox*     colorsSpin;
    QComboBox*    diffCombo;
    QListWidget*  logList;
    QScrollBar*   speedBar;

    int bestScore = 0;

    void setupUI();
    void updateScore();
    void updateNextBalls();
    void addLog(const QString& msg);
    void showHint();
    void afterMove();
};

#endif
