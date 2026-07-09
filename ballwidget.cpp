#include "ballwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QRadialGradient>
#include <QQueue>

BallWidget::BallWidget(QWidget* parent)
    : QWidget(parent), game(nullptr), selected(-1,-1),
    cellSize(50), animStep(0), animating(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(450, 450);

    animTimer = new QTimer(this);
    animTimer->setInterval(40); // скорость анимации ~25 кадров/с
    connect(animTimer, &QTimer::timeout, this, &BallWidget::onAnimationTick);
}

void BallWidget::setField(GameField* field)
{
    game = field;
    cellSize = 50;
    update();
}

void BallWidget::setSelected(QPoint p)
{
    selected = p;
    update();
}

void BallWidget::clearSelected()
{
    selected = QPoint(-1, -1);
    update();
}

QPoint BallWidget::pixelToCell(QPoint px) const
{
    if (cellSize <= 0) return QPoint(-1,-1);
    return QPoint(px.y() / cellSize, px.x() / cellSize);
}

// Строим путь BFS для анимации (список клеток)
void BallWidget::buildAnimPath(QPoint from, QPoint to)
{
    animPath.clear();
    if (!game) return;

    QVector<QVector<QPoint>> prev(game->rows,
                                  QVector<QPoint>(game->cols, QPoint(-1,-1)));
    QVector<QVector<bool>> visited(game->rows,
                                   QVector<bool>(game->cols, false));
    QQueue<QPoint> queue;
    queue.enqueue(from);
    visited[from.x()][from.y()] = true;

    int dr[] = {-1,1,0,0};
    int dc[] = {0,0,-1,1};

    while (!queue.isEmpty()) {
        QPoint cur = queue.dequeue();
        if (cur == to) break;
        for (int d = 0; d < 4; d++) {
            int nr = cur.x()+dr[d], nc = cur.y()+dc[d];
            if (nr<0||nr>=game->rows||nc<0||nc>=game->cols) continue;
            if (visited[nr][nc]) continue;
            if (game->grid[nr][nc].color != EMPTY && QPoint(nr,nc) != to) continue;
            visited[nr][nc] = true;
            prev[nr][nc] = cur;
            queue.enqueue(QPoint(nr,nc));
        }
    }

    // Восстанавливаем путь
    QPoint cur = to;
    while (cur != from) {
        animPath.prepend(cur);
        QPoint p = prev[cur.x()][cur.y()];
        if (p == QPoint(-1,-1)) { animPath.clear(); return; }
        cur = p;
    }
    animPath.prepend(from);
}

void BallWidget::startMoveAnimation(QPoint from, QPoint to)
{
    animFrom = from;
    animTo   = to;
    animStep = 0;
    animating = true;
    buildAnimPath(from, to);
    animTimer->start();
}

void BallWidget::onAnimationTick()
{
    animStep++;
    if (animStep >= animPath.size()) {
        animTimer->stop();
        animating = false;
        emit animationDone(animFrom, animTo);
    }
    update();
}

void BallWidget::paintEvent(QPaintEvent*)
{
    if (!game) return;

    int sz1 = width()  / game->cols;
    int sz2 = height() / game->rows;
    cellSize = qMax(10, qMin(sz1, sz2));

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(18, 18, 28));

    // Текущая позиция анимируемого шарика
    QPoint animCurCell = animating && !animPath.isEmpty()
                             ? animPath[qMin(animStep, animPath.size()-1)]
                             : QPoint(-1,-1);

    for (int r = 0; r < game->rows; r++) {
        for (int c = 0; c < game->cols; c++) {
            QRect cellRect(c*cellSize, r*cellSize, cellSize, cellSize);

            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);

            // Фон клетки
            QColor bg = (r+c)%2==0 ? QColor(45,45,58) : QColor(35,35,48);
            p.fillRect(cellRect, bg);

            // Сетка
            p.setPen(QPen(QColor(60,60,80), 1));
            p.setBrush(Qt::NoBrush);
            p.drawRect(cellRect);

            // Подсветка выбранной
            if (!animating && selected == QPoint(r,c)) {
                p.fillRect(cellRect, QColor(255,255,255,25));
                p.setPen(QPen(QColor(180,220,255), 2));
                p.setBrush(Qt::NoBrush);
                p.drawRect(cellRect.adjusted(1,1,-1,-1));
            }

            // Шарик (пропускаем анимируемый на его старой позиции)
            bool isAnimBall = animating && (QPoint(r,c)==animFrom);
            if (!isAnimBall && game->grid[r][c].color != EMPTY) {
                bool isSel = (!animating && selected==QPoint(r,c));
                QColor color = BALL_COLORS[game->grid[r][c].color];
                drawBall(p,
                         c*cellSize + cellSize/2.0,
                         r*cellSize + cellSize/2.0,
                         cellSize * (isSel ? 0.44 : 0.38),
                         color, isSel);
            }

            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::NoPen);
        }
    }

    // Рисуем анимируемый шарик поверх всего
    if (animating && !animPath.isEmpty()) {
        int step = qMin(animStep, animPath.size()-1);
        QPoint cell = animPath[step];
        int color = game->grid[animFrom.x()][animFrom.y()].color;
        if (color == EMPTY && animStep > 0)
            color = game->grid[animTo.x()][animTo.y()].color;
        if (color != EMPTY) {
            drawBall(p,
                     cell.y()*cellSize + cellSize/2.0,
                     cell.x()*cellSize + cellSize/2.0,
                     cellSize * 0.42,
                     BALL_COLORS[color], false);
        }
    }
}

void BallWidget::drawBall(QPainter& p, double cx, double cy,
                          double radius, QColor color, bool highlight)
{
    QRectF ballRect(cx - radius, cy - radius, radius*2, radius*2);

    QRadialGradient grad(cx - radius*0.3, cy - radius*0.35, radius*1.1);
    grad.setColorAt(0.0, color.lighter(190));
    grad.setColorAt(0.4, color.lighter(110));
    grad.setColorAt(1.0, color.darker(170));

    p.setBrush(grad);
    if (highlight)
        p.setPen(QPen(QColor(220,240,255), 2.5));
    else
        p.setPen(QPen(color.darker(200), 1));

    p.drawEllipse(ballRect);

    // Блик
    QRadialGradient glare(cx - radius*0.28, cy - radius*0.32, radius*0.45);
    glare.setColorAt(0.0, QColor(255,255,255,140));
    glare.setColorAt(1.0, QColor(255,255,255,0));
    p.setBrush(glare);
    p.setPen(Qt::NoPen);
    p.drawEllipse(ballRect);
}

void BallWidget::mousePressEvent(QMouseEvent* e)
{
    if (!game || animating) return;
    if (e->button() != Qt::LeftButton) return;
    QPoint cell = pixelToCell(e->pos());
    if (cell.x()<0||cell.x()>=game->rows||cell.y()<0||cell.y()>=game->cols) return;
    emit cellClicked(cell);
}

void BallWidget::keyPressEvent(QKeyEvent* e)
{
    if (!game || animating || selected==QPoint(-1,-1)) {
        QWidget::keyPressEvent(e); return;
    }
    int r = selected.x(), c = selected.y();
    if      (e->key()==Qt::Key_Up)    r--;
    else if (e->key()==Qt::Key_Down)  r++;
    else if (e->key()==Qt::Key_Left)  c--;
    else if (e->key()==Qt::Key_Right) c++;
    else { QWidget::keyPressEvent(e); return; }

    if (r<0||r>=game->rows||c<0||c>=game->cols) return;
    emit cellClicked(QPoint(r,c));
}

// Всплывающее меню по правой кнопке мыши
void BallWidget::contextMenuEvent(QContextMenuEvent* e)
{
    if (!game || animating) return;

    QPoint cell = pixelToCell(e->pos());
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background:#2a2a3a; color:white; border:1px solid #555; }"
        "QMenu::item:selected { background:#4a4a6a; }"
        );

    QAction* actNew    = menu.addAction("  Новая игра");
    menu.addSeparator();
    QAction* actHint   = menu.addAction("  Подсказка (показать ход)");
    QAction* actCancel = menu.addAction("  Отменить выбор");

    // Если кликнули на шарик — добавляем "Выбрать"
    QAction* actSelect = nullptr;
    if (cell.x()>=0 && cell.x()<game->rows &&
        cell.y()>=0 && cell.y()<game->cols &&
        game->grid[cell.x()][cell.y()].color != EMPTY)
    {
        menu.addSeparator();
        actSelect = menu.addAction("✅  Выбрать шарик");
    }

    QAction* chosen = menu.exec(e->globalPos());

    if (!chosen) return;
    if (chosen == actNew)    emit cellClicked(QPoint(-2,-2)); // сигнал новой игры
    if (chosen == actCancel) clearSelected();
    if (chosen == actHint)   emit cellClicked(QPoint(-3,-3)); // сигнал подсказки
    if (actSelect && chosen == actSelect) emit cellClicked(cell);
}
