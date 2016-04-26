#ifndef SNAKE_H_
#define SNAKE_H_

#include <iostream>
#include <string>
#include <vector>


class CSnake
{
public:
	enum 
	{
		INVALID = -1,
		H = 10,	
		W = 10
	};

	enum
	{
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3	
	};

private:
	static const int direction_[4][2]; 

	int dir_;
	std::pair<int, int> appleLoc_;
	std::string snake_;   //snake_[0],snake_[1]表示头部坐标
	std::vector<std::string> plat_;  //横纵下表都从1开始

public:
	CSnake();

	void resetPlat();
	int kbhit();
	int getkb();
	
	int draw();
	int gameOver(int, int);
	int putApple(); 
	int changeDirection(int);
	int go();
};

#endif 
