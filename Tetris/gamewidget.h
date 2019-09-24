#pragma once
#include <QWidget>
#include "game.h"

class QTimer;
class QLabel;

class GameWidget : public QWidget
{
	Q_OBJECT
public:
	GameWidget();
private slots:
	void timerTick();
private:
	QVector<Cell*> nextStep(QVector<Cell*> &figure, int deltaX, int deltaY);
	QList<Cell*> moveCellsWithoutChecking(QList<Cell*> &figure, int deltaX, int deltaY);
	void fillCells(QVector<Cell*> &figure, bool value = true);
	bool tryMove(QVector<Cell*> &figure);
	void moveFigure(int deltaX, int deltaY);
	void rotateFigure(bool left);
	void checkCompleteRows();
	void createBox();
	void createBar();
	void createZ();
	void createL();

	void paintEvent(QPaintEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	const int width = 10;
	const int height = 20;
	const int cellWidth = 30;
	const int cellHeight = 30;
	const int spacing = 5;
	Cell ***cells;
	QVector<Cell*> figure;
	Cell* currentFigureCenter = nullptr;
	QLabel* label;

	const int initSpan = 1000;
	QTimer* timer;
	int currentSpan = initSpan;
	const int deltaSpan = 1;
	bool expanded = false;
};