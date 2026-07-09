#ifndef BALLWIDGET_H
#define BALLWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QVector>
#include "gamefield.h"

class BallWidget : public QWidget {
    Q_OBJECT

public:
    explicit BallWidget(QWidget* parent = nullptr);
    void setField(GameField* field);
    void setSelected(QPoint p);
    void clearSelected();
    void startMoveAnimation(QPoint from, QPoint to);

signals:
    void cellClicked(QPoint cell);
    void animationDone(QPoint from, QPoint to);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;

private slots:
    void onAnimationTick();

private:
    GameField* game;
    QPoint     selected;
    mutable int cellSize;

    // Анимация
    QTimer*        animTimer;
    QPoint         animFrom, animTo;
    QVector<QPoint> animPath;
    int            animStep;
    bool           animating;

    QPoint pixelToCell(QPoint px) const;
    void   drawBall(QPainter& p, double x, double y, double size,
                  QColor color, bool highlight);
    void   buildAnimPath(QPoint from, QPoint to);
};

#endif
