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

	static const int direction_[4][2]; 

private:

	int dir_;
	std::pair<int, int> appleLoc_;
	std::string snake_;   //snake_[0],snake_[1]表示头部坐标
	std::vector<std::string> plat_;  //横纵下表都从1开始

public:
	CSnake();

	int getDir() const  { return dir_; };
	std::pair<int, int> getAppleLoc() const  { return appleLoc_; };
	std::string getSnake() const { return snake_; };

	void resetPlat();
	int kbhit();
	int getkb();
	
	int draw();
	int gameOver(int, int) const;
	int putApple(); 
	int changeDirection(int);
	int go();
};

#endif 
