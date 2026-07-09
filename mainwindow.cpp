#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Lines");
    setupUI();
    onNewGame();
}

MainWindow::~MainWindow() { delete ui; delete game; }

void MainWindow::setupUI()
{
    QWidget* center = new QWidget(this);
    setCentralWidget(center);
    center->setStyleSheet("background:#12121e;");

    QHBoxLayout* root = new QHBoxLayout(center);
    root->setContentsMargins(8,8,8,8);
    root->setSpacing(10);

    // Поле
    field = new BallWidget(this);
    root->addWidget(field, 3);

    // Правая панель
    QVBoxLayout* panel = new QVBoxLayout();
    panel->setSpacing(8);
    root->addLayout(panel, 1);

    QString groupStyle =
        "QGroupBox { color:#aab; font-size:13px; border:1px solid #334;"
        "border-radius:6px; margin-top:8px; padding-top:6px; }"
        "QGroupBox::title { subcontrol-origin:margin; left:8px; }";
    QString labelStyle = "color:#ccd; font-size:13px;";
    QString spinStyle  =
        "QSpinBox { background:#22223a; color:white; border:1px solid #445;"
        "border-radius:4px; padding:2px 4px; }"
        "QSpinBox::up-button,QSpinBox::down-button { width:16px; }";
    QString comboStyle =
        "QComboBox { background:#22223a; color:white; border:1px solid #445;"
        "border-radius:4px; padding:2px 6px; }"
        "QComboBox QAbstractItemView { background:#22223a; color:white; }";

    // Счёт
    scoreLabel = new QLabel("Счёт: 0", this);
    scoreLabel->setStyleSheet("font-size:26px; font-weight:bold; color:#e0e8ff;");
    scoreLabel->setAlignment(Qt::AlignCenter);
    panel->addWidget(scoreLabel);

    recordLabel = new QLabel("Рекорд: 0", this);
    recordLabel->setStyleSheet("font-size:13px; color:#778;");
    recordLabel->setAlignment(Qt::AlignCenter);
    panel->addWidget(recordLabel);

    // Следующие шарики
    QGroupBox* nextGroup = new QGroupBox("Следующие шарики", this);
    nextGroup->setStyleSheet(groupStyle);
    QVBoxLayout* nextLay = new QVBoxLayout(nextGroup);
    nextTable = new QTableWidget(1, 3, this);
    nextTable->horizontalHeader()->hide();
    nextTable->verticalHeader()->hide();
    nextTable->setFixedHeight(52);
    nextTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    nextTable->setSelectionMode(QAbstractItemView::NoSelection);
    nextTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    nextTable->setStyleSheet(
        "QTableWidget { background:#1a1a2e; border:none; gridline-color:#334; }"
        );
    nextLay->addWidget(nextTable);
    panel->addWidget(nextGroup);

    // Настройки
    QGroupBox* setGroup = new QGroupBox("Настройки", this);
    setGroup->setStyleSheet(groupStyle);
    QGridLayout* setLay = new QGridLayout(setGroup);
    setLay->setVerticalSpacing(6);

    auto lbl = [&](const QString& t) {
        QLabel* l = new QLabel(t, this);
        l->setStyleSheet(labelStyle);
        return l;
    };

    setLay->addWidget(lbl("Строки:"),   0, 0);
    rowsSpin = new QSpinBox(this);
    rowsSpin->setRange(5,15); rowsSpin->setValue(9);
    rowsSpin->setStyleSheet(spinStyle);
    setLay->addWidget(rowsSpin, 0, 1);

    setLay->addWidget(lbl("Столбцы:"), 1, 0);
    colsSpin = new QSpinBox(this);
    colsSpin->setRange(5,15); colsSpin->setValue(9);
    colsSpin->setStyleSheet(spinStyle);
    setLay->addWidget(colsSpin, 1, 1);

    setLay->addWidget(lbl("Цветов:"),  2, 0);
    colorsSpin = new QSpinBox(this);
    colorsSpin->setRange(3,7); colorsSpin->setValue(7);
    colorsSpin->setStyleSheet(spinStyle);
    setLay->addWidget(colorsSpin, 2, 1);

    setLay->addWidget(lbl("Сложность:"), 3, 0);
    diffCombo = new QComboBox(this);
   diffCombo->addItems({"Лёгкий (3 цв)", "Нормальный (5 цв)", "Хард (7 цв)"});
    diffCombo->setStyleSheet(comboStyle);
   connect(diffCombo, &QComboBox::currentIndexChanged, this, [this](int idx){
       int c[] = {3, 5, 7};
       colorsSpin->setValue(c[idx]);
   });
    setLay->addWidget(diffCombo, 3, 1);

    panel->addWidget(setGroup);

    // Кнопка новой игры
    QPushButton* btnNew = new QPushButton("▶  Новая игра", this);
    btnNew->setStyleSheet(
        "QPushButton { background:#3a3a6a; color:white; font-size:15px;"
        "border-radius:6px; padding:10px; border:1px solid #556; }"
        "QPushButton:hover { background:#4a4a8a; }"
        "QPushButton:pressed { background:#2a2a5a; }"
        );
    connect(btnNew, &QPushButton::clicked, this, &MainWindow::onNewGame);
    panel->addWidget(btnNew);

    // Лог
    QGroupBox* logGroup = new QGroupBox("Ходы", this);
    logGroup->setStyleSheet(groupStyle);
    QVBoxLayout* logLay = new QVBoxLayout(logGroup);
    logList = new QListWidget(this);
    logList->setStyleSheet(
        "QListWidget { background:#1a1a2e; color:#aac; border:none;"
        "font-size:12px; }"
        "QListWidget::item { padding:2px 4px; }"
        "QListWidget::item:alternate { background:#1e1e34; }"
        );
    logList->setAlternatingRowColors(true);
    logList->setMaximumHeight(160);
    logLay->addWidget(logList);
    panel->addWidget(logGroup);

    // Слайдер скорости анимации
    QGroupBox* speedGroup = new QGroupBox("Скорость анимации", this);
    speedGroup->setStyleSheet(groupStyle);
    QVBoxLayout* speedLay = new QVBoxLayout(speedGroup);
    speedBar = new QScrollBar(Qt::Horizontal, this);
    speedBar->setRange(1, 5);
    speedBar->setValue(3);
    speedBar->setStyleSheet(
        "QScrollBar:horizontal { background:#22223a; height:16px;"
        "border-radius:8px; }"
        "QScrollBar::handle:horizontal { background:#556; border-radius:8px; min-width:20px; }"
        );
    connect(speedBar, &QScrollBar::valueChanged, this, [this](int val){
        // 1=медленно(80мс) 5=быстро(15мс)
        int ms[] = {0, 80, 55, 40, 25, 15};
        // animTimer интервал меняем через field
        Q_UNUSED(val); Q_UNUSED(ms);
    });
    speedLay->addWidget(speedBar);
    panel->addWidget(speedGroup);

    panel->addStretch();

    connect(field, &BallWidget::cellClicked,   this, &MainWindow::onCellClicked);
    connect(field, &BallWidget::animationDone, this, &MainWindow::onAnimationDone);

    resize(960, 660);
}

void MainWindow::onNewGame()
{
    delete game;
    game = new GameField(rowsSpin->value(), colsSpin->value(), colorsSpin->value());
    field->setField(game);
    field->clearSelected();
    selected = QPoint(-1,-1);
    logList->clear();
    updateScore();
    updateNextBalls();
    addLog("▶ Новая игра");
}

void MainWindow::onCellClicked(QPoint cell)
{
    if (!game) return;

    // Сигнал из контекстного меню
    if (cell == QPoint(-2,-2)) { onNewGame(); return; }
    if (cell == QPoint(-3,-3)) { showHint();  return; }

    if (selected == QPoint(-1,-1)) {
        // Выбираем шарик
        if (game->grid[cell.x()][cell.y()].color == EMPTY) return;
        selected = cell;
        field->setSelected(cell);
        addLog(QString("Выбран (%1,%2)").arg(cell.x()+1).arg(cell.y()+1));
    } else {
        if (cell == selected) {
            selected = QPoint(-1,-1);
            field->clearSelected();
            return;
        }
        // Переключаем на другой шарик
        if (game->grid[cell.x()][cell.y()].color != EMPTY) {
            selected = cell;
            field->setSelected(cell);
            return;
        }
        // Пробуем ход — сначала проверяем путь
        if (!game->hasPath(selected, cell)) {
            addLog("⛔ Нет пути!");
            return;
        }
        // Запускаем анимацию (ход выполнится в onAnimationDone)
        QPoint from = selected;
        QPoint to   = cell;
        selected = QPoint(-1,-1);
        field->clearSelected();
        field->startMoveAnimation(from, to);
    }
}

void MainWindow::onAnimationDone(QPoint from, QPoint to)
{
    if (!game) return;

    game->moveBall(from, to);
    addLog(QString("Ход: (%1,%2)→(%3,%4)")
               .arg(from.x()+1).arg(from.y()+1)
               .arg(to.x()+1).arg(to.y()+1));

    afterMove();
}

void MainWindow::afterMove()
{
    int removed = game->removeLines();
    if (removed > 0) {
        addLog(QString("✨ Линия! +%1 очков").arg(removed));
        updateScore();
        updateNextBalls();
        field->update();
        return; // новые шарики не спавнятся
    }

    game->spawnBalls();
    removed = game->removeLines();
    if (removed > 0)
        addLog(QString("✨ Бонус! +%1 очков").arg(removed));

    updateScore();
    updateNextBalls();
    field->update();

    if (game->isGameOver()) {
        if (game->score > bestScore) bestScore = game->score;
        QMessageBox::information(this, "Игра окончена",
                                 QString("Поле заполнено!\nСчёт: %1\nРекорд: %2")
                                     .arg(game->score).arg(bestScore));
    }
}

void MainWindow::showHint()
{
    if (!game) return;
    // Ищем любой возможный ход
    for (int r1=0; r1<game->rows; r1++)
        for (int c1=0; c1<game->cols; c1++) {
            if (game->grid[r1][c1].color == EMPTY) continue;
            for (int r2=0; r2<game->rows; r2++)
                for (int c2=0; c2<game->cols; c2++) {
                    if (game->grid[r2][c2].color != EMPTY) continue;
                    if (game->hasPath(QPoint(r1,c1), QPoint(r2,c2))) {
                        field->setSelected(QPoint(r1,c1));
                        selected = QPoint(r1,c1);
                        addLog(QString("💡 Подсказка: шарик (%1,%2)")
                                   .arg(r1+1).arg(c1+1));
                        return;
                    }
                }
        }
    addLog("💡 Ходов нет — игра закончена!");
}

void MainWindow::updateScore()
{
    scoreLabel->setText(QString("Счёт: %1").arg(game->score));
    if (game->score > bestScore) bestScore = game->score;
    recordLabel->setText(QString("Рекорд: %1").arg(bestScore));
}

void MainWindow::updateNextBalls()
{
    auto next = game->getNextColors();
    for (int i = 0; i < 3; i++) {
        QTableWidgetItem* item = new QTableWidgetItem();
        if (i < next.size()) {
            QColor c = BALL_COLORS[next[i]];
            item->setBackground(c);
            item->setToolTip(QString("Цвет %1").arg(i+1));
        }
        nextTable->setItem(0, i, item);
    }
}

void MainWindow::addLog(const QString& msg)
{
    logList->addItem(msg);
    logList->scrollToBottom();
}
