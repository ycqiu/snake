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
		H = 12,	
		W = 12
	};

	enum
	{
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3	
	};

	static const int direction[4][2]; 

public:
	static void printSnake(const std::string& s, const std::string& name = "");

private:
	int dir_;
	std::pair<int, int> appleLoc_;
	std::string snake_;   //snake_[0],snake_[1]表示头部坐标
	std::vector<std::string> plat_;  //横纵下标都从1开始

public:
	CSnake();

	int getDir() const  { return dir_; };
	std::pair<int, int> getAppleLoc() const  { return appleLoc_; };
	std::string getSnake() const { return snake_; };

	void resetPlat();
	int kbhit();
	int getkb();
	
	int draw();
	void fillPlat();
	int gameOver(int, int) const;
	int putApple(); 
	int changeDirection(int);
	int go();
	void printPlat(const std::string& name = "");
};

#endif 
