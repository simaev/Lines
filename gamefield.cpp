#include "gamefield.h"

GameField::GameField(int r, int c, int colors)
    : rows(0), cols(0), numColors(0), score(0)
{
    reset(r, c, colors);
}

void GameField::reset(int r, int c, int colors)
{
    rows = r; cols = c; numColors = colors; score = 0;

    grid.clear();
    grid.resize(rows);
    for (int i = 0; i < rows; i++) {
        grid[i].clear();
        grid[i].resize(cols);
        for (int j = 0; j < cols; j++)
            grid[i][j].color = EMPTY;
    }

    nextColors.clear();
    generateNextColors();
    spawnBalls();
}

bool GameField::isEmpty(int r, int c) const
{
    return grid[r][c].color == EMPTY;
}

void GameField::generateNextColors()
{
    nextColors.clear();
    for (int i = 0; i < NEW_BALLS; i++)
        nextColors.append(QRandomGenerator::global()->bounded(numColors));
}

QVector<QPoint> GameField::spawnBalls()
{
    QVector<QPoint> empty;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (grid[r][c].color == EMPTY)
                empty.append(QPoint(r, c));

    for (int i = empty.size() - 1; i > 0; i--) {
        int j = QRandomGenerator::global()->bounded(i + 1);
        QPoint tmp = empty[i]; empty[i] = empty[j]; empty[j] = tmp;
    }

    QVector<QPoint> placed;
    int n = qMin(NEW_BALLS, empty.size());
    for (int i = 0; i < n; i++) {
        grid[empty[i].x()][empty[i].y()].color = nextColors[i];
        placed.append(empty[i]);
    }

    generateNextColors();
    return placed;
}

bool GameField::moveBall(QPoint from, QPoint to)
{
    if (grid[to.x()][to.y()].color != EMPTY) return false;
    if (!hasPath(from, to)) return false;
    grid[to.x()][to.y()].color = grid[from.x()][from.y()].color;
    grid[from.x()][from.y()].color = EMPTY;
    return true;
}

bool GameField::hasPath(QPoint from, QPoint to)
{
    if (grid[to.x()][to.y()].color != EMPTY) return false;

    QVector<QVector<bool>> visited(rows, QVector<bool>(cols, false));
    QQueue<QPoint> queue;
    queue.enqueue(from);
    visited[from.x()][from.y()] = true;

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    while (!queue.isEmpty()) {
        QPoint cur = queue.dequeue();
        if (cur == to) return true;
        for (int d = 0; d < 4; d++) {
            int nr = cur.x() + dr[d];
            int nc = cur.y() + dc[d];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (visited[nr][nc] || grid[nr][nc].color != EMPTY) continue;
            visited[nr][nc] = true;
            queue.enqueue(QPoint(nr, nc));
        }
    }
    return false;
}

QVector<QPoint> GameField::checkLine(QPoint start, int dr, int dc)
{
    int color = grid[start.x()][start.y()].color;
    if (color == EMPTY) return {};

    QVector<QPoint> line;
    line.append(start);

    int r = start.x() - dr, c = start.y() - dc;
    while (r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c].color == color) {
        line.prepend(QPoint(r, c)); r -= dr; c -= dc;
    }
    r = start.x() + dr; c = start.y() + dc;
    while (r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c].color == color) {
        line.append(QPoint(r, c)); r += dr; c += dc;
    }

    return line.size() >= LINE_LENGTH ? line : QVector<QPoint>();
}

int GameField::removeLines()
{
    QVector<QPoint> toRemove;
    int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};

    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            if (grid[r][c].color == EMPTY) continue;
            for (int d = 0; d < 4; d++) {
                auto line = checkLine(QPoint(r,c), dirs[d][0], dirs[d][1]);
                for (auto& p : line)
                    if (!toRemove.contains(p)) toRemove.append(p);
            }
        }

    for (auto& p : toRemove) grid[p.x()][p.y()].color = EMPTY;

    int gained = toRemove.size() * 2;
    score += gained;
    return gained;
}

bool GameField::isGameOver() const
{
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (grid[r][c].color == EMPTY) return false;
    return true;
}

QVector<int> GameField::getNextColors() const
{
    return nextColors;
}
