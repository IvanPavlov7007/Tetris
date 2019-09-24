#include <QPainter>
#include <QTimer>
#include <QTime>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QMediaPlaylist>
//#include <QLabel>
//#include <QHBoxLayout> 
#include "gamewidget.h"

GameWidget::GameWidget()
	: QWidget()
{
	setFixedSize(355, 705);
	/*QHBoxLayout *_layout = new QHBoxLayout(this);
	_layout->addStretch();
	_layout->setStretchFactor(_layout, 0);
	label = new QLabel("My implementation of the tetris idea", this);
	_layout->addWidget(label);*/
	srand(QTime::currentTime().msec());
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &GameWidget::timerTick);
	cells = new Cell**[width];
	for (int x = 0; x < width; x++)
	{
		cells[x] = new Cell*[height];
		for (int y = 0; y < height; y++)
		{
			cells[x][y] = new Cell(x, y);
		}
	}
	QMediaPlaylist *playList = new QMediaPlaylist(this);
	playList->addMedia(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/sound.mp3"));
	playList->setPlaybackMode(QMediaPlaylist::Loop);
	QMediaPlayer* player = new QMediaPlayer(this);
	player->setPlaylist(playList);
	player->setVolume(100);
	player->play();
	timer->start(currentSpan);
}

void GameWidget::timerTick()
{
	if (figure.empty())
	{
		switch (rand() % 4)
		{
		case 0:
			createBox();
			break;
		case 1:
			createBar();
			break;
		case 2:
			createZ();
			break;
		case 3:
			createL();
			break;
		}
		if (!tryMove(figure))
			QApplication::exit();
		else
			fillCells(figure);
	}
	else
	{
		fillCells(figure,false);
		QVector<Cell*> newFigure = nextStep(figure, 0, 1);
		if (!tryMove(newFigure) || newFigure == figure)
		{
			fillCells(figure, true);
			currentFigureCenter = nullptr;
			figure.clear();
			checkCompleteRows();
		}
		else
		{
			figure = newFigure;
			if (currentFigureCenter != nullptr)
				currentFigureCenter = cells[currentFigureCenter->_x][currentFigureCenter->_y + 1];
			fillCells(figure, true);
		}
	}
	if(deltaSpan < currentSpan)
		currentSpan -= deltaSpan;
	timer->setInterval(currentSpan);
	update();
}

void GameWidget::checkCompleteRows()
{
	QList<Cell*> cellsOnGround;
	QList<Cell*> filledCellsInRow;
	for (int y = 0; y < height; y++)
	{
		bool complete = true;
		for (int x = 0; x < width; x++)
		{
			if (!cells[x][y]->_filled)
			{
				complete = false;
			}
			else
				filledCellsInRow.append(cells[x][y]);

		}

		if (complete)
		{
			for (int x = 0; x < width; x++)
			{
				cells[x][y]->_filled = false;
			}
			fillCells(cellsOnGround.toVector(), false);
			cellsOnGround = moveCellsWithoutChecking(cellsOnGround, 0, 1);
			fillCells(cellsOnGround.toVector(), true);
		}
		else
			cellsOnGround.append(filledCellsInRow);
	}
}
QList<Cell*> GameWidget::moveCellsWithoutChecking(QList<Cell*> &figure, int deltaX, int deltaY)
{
	QList<Cell*> newFigure = QList<Cell*>();
	Cell* oldCell;
	for (int i = 0; i < figure.size(); i++)
	{
		oldCell = figure[i];
		newFigure.append( cells[oldCell->_x + deltaX][oldCell->_y + deltaY] );
	}
	return newFigure;
}

QVector<Cell*> GameWidget::nextStep(QVector<Cell*> &figure, int deltaX, int deltaY)
{
	QVector<Cell*> newFigure = QVector<Cell*>(figure.size());
	int newX, newY;
	for(int i = 0; i < figure.size(); i++)
	{
		newX = figure[i]->_x + deltaX;
		newY = figure[i]->_y + deltaY;
		if (newY >= height || newX >= width || newX < 0)
		{
			newFigure = figure;
			break;
		}
		newFigure[i] = cells[newX][newY];
	}
	return newFigure;
}

void GameWidget::fillCells(QVector<Cell*> &cells, bool value)
{
	for each(auto f in cells)
	{
		f->_filled = value;
	}
}

bool GameWidget::tryMove(QVector<Cell*> &figure)
{
	bool result = true;
	for each(auto f in figure)
	{
		if (f->_filled)
			result = false;
	}
	return result;
}

void GameWidget::createBox()
{
	figure.resize(4);
	figure[0] = cells[4][0];
	figure[1] = cells[5][0];
	figure[2] = cells[4][1];
	figure[3] = cells[5][1];
}

void GameWidget::createBar()
{
	figure.resize(4);
	figure[0] = cells[4][0];
	figure[1] = cells[4][1];
	figure[2] = cells[4][2];
	figure[3] = cells[4][3];
	currentFigureCenter = cells[4][2];
}

void GameWidget::createZ()
{
	figure.resize(4);
	figure[0] = cells[5][0];
	figure[1] = cells[4][0];
	figure[2] = cells[4][1];
	figure[3] = cells[3][1];
	currentFigureCenter = cells[4][0];
}

void GameWidget::createL()
{
	figure.resize(4);
	figure[0] = cells[4][0];
	figure[1] = cells[4][1];
	figure[2] = cells[4][2];
	figure[3] = cells[5][2];
	currentFigureCenter = cells[4][1];
}

void GameWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (cells[x][y]->_filled)
				painter.setBrush(Qt::red);
			else
				painter.setBrush(Qt::white);
			painter.drawRect(spacing + x * cellWidth + x * spacing, spacing + y* cellHeight + y * spacing, cellWidth, cellHeight);
		}
	}
	painter.drawText(QRect(380,3 * spacing,300,100), "The implementation of the Tetris idea by\n                                             Ivan Pavlov");
}

void GameWidget::rotateFigure(bool left)
{
	if (currentFigureCenter == nullptr)
		return;
	
	int deltaX, deltaY, newX, newY, c;
	bool possible = true;
	QVector<Cell*> newFigure(figure.size());
	for (int i = 0; i < figure.size(); i++)
	{
		deltaX = figure[i]->_x - currentFigureCenter->_x;
		deltaY = figure[i]->_y - currentFigureCenter->_y;
		if (left)
		{
			c = deltaX;
			deltaX = deltaY;
			deltaY = -c;
		}
		else
		{
			c = deltaY;
			deltaY = deltaX;
			deltaX = -c;
		}
		newX = currentFigureCenter->_x + deltaX;
		newY = currentFigureCenter->_y + deltaY;
		if (newY < 0 || newY >= height || newX >= width || newX < 0)
		{
			possible = false;
			break;
		}
		newFigure[i] = cells[newX][newY];
	}

	if (possible)
	{
		fillCells(figure, false);
		if (tryMove(newFigure))
			figure = newFigure;
		fillCells(figure, true);
		update();
	}
}

void GameWidget::keyPressEvent(QKeyEvent * event)
{
	switch (event->key())
	{
	case Qt::Key::Key_Left:
		moveFigure(-1, 0);
		break;
	case Qt::Key::Key_Right:
		moveFigure(1, 0);
		break;
	case Qt::Key::Key_Down:
		moveFigure(0, 1);
		break;
	case Qt::Key::Key_A:
		rotateFigure(true);
		break;
	case Qt::Key::Key_D:
		rotateFigure(false);
		break;
	}
	QWidget::keyPressEvent(event);
}

void GameWidget::mousePressEvent(QMouseEvent * event)
{
	if (expanded)
		setFixedSize(625, 705);
	else
		setFixedSize(355, 705);
	expanded = !expanded;
}

void GameWidget::moveFigure(int deltaX, int deltaY)
{
	fillCells(figure, false);
	QVector<Cell*> newFigure = nextStep(figure, deltaX, deltaY);
	if (tryMove(newFigure) && newFigure != figure)
	{
		figure = newFigure;
		if(currentFigureCenter != nullptr)
			currentFigureCenter = cells[currentFigureCenter->_x + deltaX][currentFigureCenter->_y + deltaY];
	}

	fillCells(figure, true);
	update();
}
