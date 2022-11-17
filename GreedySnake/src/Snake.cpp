#include "Snake.h"

enum class MoveStatus : int
{
	LEFT = 75, UP = 72, RIGHT = 77, DOWN = 80
};
enum class BlockFilledWith : int 
{
	NOTHING = 0, BARREL = 1, FRUIT = 2, SNAKE_PART = 3
};

inline void Snake::Grow()
{
	int deltaX = end->data[0] - end->last->data[0];
	int deltaY = end->data[1] - end->last->data[1];

	SnakeNode* newNode = new SnakeNode(deltaX + end->data[0], deltaY + end->data[1]);
	newNode->last = end;
	end->next = newNode;
	end = newNode;

	block[newNode->data[0]][newNode->data[1]] = (int)BlockFilledWith::SNAKE_PART;
	FillWithSnake(newNode->data[0], newNode->data[1]);
	Sleep(300);
}

inline int Snake::GetDirection() const
{
	while (_getch() != 224)
	{
		continue;
	}
    return _getch();
}

void Snake::Update()
{
	auto current = end;
	while (current->last != nullptr)
	{
		current->data[0] = current->last->data[0];
		current->data[1] = current->last->data[1];
		current = current->last;
	}	
}

inline void Snake::DrawWall() const
{
	system("cls");
	for (int i = 0; i < WIDTH; ++i)
	{
		for (int j = 0; j < HEIGHT; ++j)
		{
			if (i * j == 0 || i == (WIDTH - 1) || j == (HEIGHT - 1))
			{
				block[i][j] = (int)BlockFilledWith::BARREL;
				MoveCursor(i, j);
				std::cout << "¡õ";
			}
		}
	}
}

inline void Snake::MoveCursor(int x, int y) const
{
	COORD coord;  
	coord.X = x * 2;            
	coord.Y = y;
	HANDLE a = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleCursorPosition(a, coord);     
}

void Snake::Move()
{
	Clean(end->data[0], end->data[1]);
	if (status == MoveStatus::DOWN && status != MoveStatus::UP)
	{
		Update();
		++first.data[1];
	}
	else if (status == MoveStatus::UP && status != MoveStatus::DOWN)
	{
		Update();
		--first.data[1];
	}
	else if (status == MoveStatus::LEFT && status != MoveStatus::RIGHT)
	{
		Update();
		--first.data[0];
	}
	else if (status == MoveStatus::RIGHT && status != MoveStatus::LEFT)
	{
		Update();
		++first.data[0];
	}	
	if (EatFruit())	GenerateFruit();
	if (!CollideCheck())
	{
		GAMING = false;
		return;
	}
	FillWithSnake(first.data[0], first.data[1]);
	Sleep(300);
	if (_kbhit())
	{
		_getch();
		int key = _getch();
		if (abs(key - (int)status) != 2 && abs(key - (int)status) != 8)
			status = (MoveStatus)key;
	}
}

inline bool Snake::CollideCheck() const
{
	if (block[first.data[0]][first.data[1]] == (int)BlockFilledWith::BARREL
		|| block[first.data[0]][first.data[1]] == (int)BlockFilledWith::SNAKE_PART
		|| block[end->data[0]][end->data[1]] == (int)BlockFilledWith::BARREL)
		return false;
	else return true;
}

inline void Snake::Clean(int x, int y) const
{
	MoveCursor(x, y);
	std::cout << "  ";
	block[x][y] = (int)BlockFilledWith::NOTHING;
}

inline void Snake::FillWithSnake(int x, int y) const
{
	MoveCursor(x, y);
	std::cout << "¡ö";
	block[x][y] = (int)BlockFilledWith::SNAKE_PART;
}

inline void Snake::DrawSnake()
{
	FillWithSnake(WIDTH / 2, HEIGHT / 2);

	SnakeNode* newNode1 = new SnakeNode(first.data[0] + 1, first.data[1]);
	first.next = newNode1;
	newNode1->last = &first;
	FillWithSnake(first.data[0] + 1, first.data[1]);

	SnakeNode* newNode2 = new SnakeNode(first.data[0] + 2, first.data[1]);
	newNode2->last = newNode1;
	newNode1->next = newNode2;
	FillWithSnake(first.data[0] + 2, first.data[1]);

	end = newNode2;
}

inline bool Snake::EatFruit()
{
	if (block[first.data[0]][first.data[1]] == (int)BlockFilledWith::FRUIT)
	{
		Grow();
		block[first.data[0]][first.data[1]] = (int)BlockFilledWith::NOTHING;
		return true;
	}
	else
		return false;
}

inline void Snake::GenerateFruit() const
{
	int x = 0, y = 0;
	while (block[x][y] != (int)BlockFilledWith::NOTHING)
	{
		unsigned seed = time(0);
		srand(seed);
		x = (rand() % (WIDTH - 1 - 1 + 1)) + 1;// generate a random coordinate between [1, 20]
		y = (rand() % (HEIGHT - 1 - 1 + 1)) + 1;
	}
	MoveCursor(x, y);
	std::cout << "¡ñ";
	block[x][y] = (int)BlockFilledWith::FRUIT;
}

inline void Snake::hideCursor() const
{
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

inline void SnakeNode::ClearBlock() const
{
	block[data[0]][data[1]] = (int)BlockFilledWith::NOTHING;
}

Snake::Snake()
	: first(WIDTH / 2, HEIGHT / 2)
	{
		hideCursor();
		DrawWall();
		DrawSnake();
		GenerateFruit();
		status = (MoveStatus)GetDirection();
	}