#pragma once

#include <Windows.h>
#include <conio.h>
#include <time.h>
#include <iostream>

const int WIDTH = 15;
const int HEIGHT = 20;
inline bool GAMING = true;
extern enum class MoveStatus : int;
extern enum class BlockFilledWith : int;
inline int block[WIDTH][HEIGHT] = { 0 };

struct SnakeNode
{
	int data[2];
	SnakeNode* next = nullptr;
	SnakeNode* last = nullptr;
	SnakeNode(int x = 0, int y = 0)
	{
		data[0] = x;
		data[1] = y;
	}
	void ClearBlock() const;
};

class Snake
{
private:
	SnakeNode first;
	SnakeNode* end = nullptr;
public:
	MoveStatus status;
	Snake();
	~Snake()
	{
		auto current = first.next;
		while (current != nullptr)
		{
			auto temp = current->next;
			delete current;
			current = temp;
		}
	}
	void Grow();
	void DrawWall() const;
	void MoveCursor(int x, int y) const;
	void Move();
	bool CollideCheck() const;
	void Clean(int x, int y) const;
	void GenerateFruit() const;
	int GetDirection() const;
	void Update();
protected:
	void hideCursor() const;
	void FillWithSnake(int x, int y) const;
	void DrawSnake();
	bool EatFruit();
};
