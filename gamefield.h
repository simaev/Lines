#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QVector>
#include <QPoint>
#include <QColor>
#include <QQueue>
#include <QRandomGenerator>

const QVector<QColor> BALL_COLORS = {
    QColor(220, 50,  50),   // красный
    QColor(50,  120, 220),  // синий
    QColor(50,  200, 80),   // зелёный
    QColor(230, 210, 50),   // жёлтый
    QColor(200, 60,  200),  // фиолетовый
    QColor(50,  210, 210),  // циан
    QColor(230, 140, 30)    // оранжевый
};

const int EMPTY            = -1;
const int NEW_BALLS        = 3;
const int LINE_LENGTH      = 5;

struct Cell {
    int color;
    Cell() : color(EMPTY) {}
};

class GameField {
public:
    int rows, cols, numColors, score;
    QVector<QVector<Cell>> grid;

    GameField(int r, int c, int colors);

    bool            isEmpty(int r, int c) const;
    bool            moveBall(QPoint from, QPoint to);
    QVector<QPoint> spawnBalls();
    int             removeLines();
    bool            hasPath(QPoint from, QPoint to);
    QVector<int>    getNextColors() const;
    bool            isGameOver() const;
    void            reset(int r, int c, int colors);

private:
    QVector<int> nextColors;

    void            generateNextColors();
    QVector<QPoint> checkLine(QPoint start, int dr, int dc);
};

#endif
