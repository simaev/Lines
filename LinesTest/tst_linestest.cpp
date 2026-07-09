#include <QtTest>
#include "../Lines/gamefield.h"

class LinesTest : public QObject {
    Q_OBJECT

private slots:
    void testEmptyField();
    void testSpawnBalls();
    void testMoveBall();
    void testNoPathThroughBalls();
    void testRemoveLines();
    void testGameOver();
};

void LinesTest::testEmptyField()
{
    GameField g(9, 9, 7);
    // После создания поле не пустое (уже заспавнились шарики)
    int count = 0;
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            if (!g.isEmpty(r, c)) count++;
    QVERIFY(count > 0);
}

void LinesTest::testSpawnBalls()
{
    GameField g(9, 9, 7);
    int before = 0;
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            if (!g.isEmpty(r, c)) before++;

    g.spawnBalls();

    int after = 0;
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            if (!g.isEmpty(r, c)) after++;

    QVERIFY(after >= before); // шариков стало больше или столько же
}

void LinesTest::testMoveBall()
{
    GameField g(9, 9, 7);
    // Очищаем поле
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            g.grid[r][c].color = EMPTY;

    g.grid[0][0].color = 0; // ставим один шарик
    bool ok = g.moveBall({0,0}, {0,5});
    QVERIFY(ok);
    QCOMPARE(g.grid[0][5].color, 0);
    QCOMPARE(g.grid[0][0].color, EMPTY);
}

void LinesTest::testNoPathThroughBalls()
{
    GameField g(9, 9, 7);
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            g.grid[r][c].color = EMPTY;

    g.grid[0][0].color = 0;
    // Стена из шариков
    for (int r = 0; r < g.rows; r++)
        g.grid[r][1].color = 1;

    bool ok = g.moveBall({0,0}, {0,8});
    QVERIFY(!ok); // пути нет
}

void LinesTest::testRemoveLines()
{
    GameField g(9, 9, 7);
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            g.grid[r][c].color = EMPTY;

    // Ставим 5 в ряд
    for (int c = 0; c < 5; c++)
        g.grid[0][c].color = 2;

    int gained = g.removeLines();
    QVERIFY(gained > 0);

    // Проверяем что удалились
    for (int c = 0; c < 5; c++)
        QCOMPARE(g.grid[0][c].color, EMPTY);
}

void LinesTest::testGameOver()
{
    GameField g(3, 3, 2);
    // Заполняем всё поле
    for (int r = 0; r < g.rows; r++)
        for (int c = 0; c < g.cols; c++)
            g.grid[r][c].color = 0;

    QVERIFY(g.isGameOver());
}

QTEST_MAIN(LinesTest)
#include "tst_linestest.cpp.moc"
// или: #include "tst_linestest.moc"  — зависит от версии Qt
