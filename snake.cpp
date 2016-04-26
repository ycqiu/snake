#include "snake.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;


const int CSnake::direction_[4][2] = {
		{-1, 0},  
		{1, 0},
		{0, -1},
		{0, 1}
};

CSnake::CSnake() : dir_(RIGHT), appleLoc_(INVALID, INVALID), plat_(H + 1)
{ 
	snake_ += char(H / 2 + 1);
	snake_ += char(W / 2 + 1);
/*	snake_ += char(H / 2 + 1);
	snake_ += char(W / 2);
	snake_ += char(H / 2) ;
	snake_ += char(W / 2);
*/
	appleLoc_ = make_pair(3, 3);
	resetPlat();
}

void CSnake::resetPlat()
{
	for(std::vector<std::string>::size_type i = 0; i < plat_.size(); ++i)
	{
		plat_[i].assign(W + 1, ' ');	
	}
}

int CSnake::draw()
{
	int ret = 0;
	if(snake_.length() % 2)
	{
		return ret = -1;
	}

	system("clear");
	resetPlat();
	plat_[appleLoc_.first][appleLoc_.second] = 'O';
	for(std::string::size_type i = 0; i < snake_.length(); i += 2)
	{
		int r = snake_[i];
		int c = snake_[i + 1];
		char v = '*';
		//head
		if(i == 0)
		{
			static const char* pv = "^v<>"; 
			if(dir_ >= UP && dir_ <= RIGHT)
			{
				v = *(pv + dir_);
			}
			else
			{
				return ret = -2;
			}
		}
		plat_[r][c] = v;
	}
	
	const char block = '#';
	std::string line(2 * W + 2, block);
	//std::string line( W + 2, block);
	cout << line << endl;
	for(int i = 1; i <= H; ++i)
	{
		cout << block;
		for(int j = 1; j <= W; ++j)
		{
			//cout << plat_[i][j] ; 
			cout << plat_[i][j] << " ";
		}
		cout << block << endl;
	}
	cout << line << endl;
	return ret;
}

int CSnake::gameOver(int r, int c)
{
	int ret = 0;
	if( !(r >= 1 && c >= 1 && r <= H && c <= W) )
	{
		//cout << "撞墙" << endl;
		return ret = -1;
	}

	//检查当前蛇头位置是否与蛇身重合(不包括蛇尾,因为蛇已经移动了)	
	for(std::string::size_type i = 0; i < snake_.length() - 2; i += 2)
	{
		if(r == snake_[i] && c == snake_[i + 1])	
		{
		//	cout << "吃到自己" << endl;
			return ret = -2;
		}
	}

	return ret;
}

int CSnake::go()
{
	int ret = 0;
	int r = snake_[0] + direction_[dir_][0];
	int c = snake_[1] + direction_[dir_][1];
	ret = gameOver(r, c);
	if(ret)
	{
	//	cout << "gameOver: " << ret << endl;
		return ret = -1;	
	}

	std::string pos; 
	pos += char(r);
	pos += char(c);
	snake_.insert(0, pos);

	//能不能吃苹果
	if(r == appleLoc_.first && c == appleLoc_.second)
	{
		//重新放苹果		
		ret = putApple();
		if(ret)
		{
			return ret = -2;
		}
	}
	else
	{
		snake_.erase(snake_.length() - 2, 2);
	}
	return ret;
}

int CSnake::changeDirection(int d)
{
	int ret = 0;	
	if( !(d >= UP && d <= RIGHT) )
	{
		return ret = -1;
	}
	
	//上下不能转换  左右不能转换		
	const int v[] = {DOWN, UP, RIGHT, LEFT};	
	if(v[d] == dir_)
	{
		return ret;
	}

	dir_ = d;
	return ret;
}

int CSnake::putApple()
{
	int ret = 0;
	if(snake_.length() == W * H)
	{
		cout << "full" << endl;
		return ret = -1;
	}
	resetPlat();
	for(std::string::size_type i = 0; i < snake_.length(); i += 2)
	{
		int r = snake_[i];
		int c = snake_[i + 1];
		char v = '*';
		plat_[r][c] = v;
	}

	std::vector<std::pair<int, int> > nullPos; 
	for(int i = 1; i <= H; ++i)
	{
		for(int j = 1; j <= W; ++j)
		{
			if(plat_[i][j] == ' ')
			{
				nullPos.push_back(make_pair(i, j));	
			}
		}
	}

	srand(time(NULL));
	int index = rand() % nullPos.size();
	appleLoc_ = nullPos[index];
	return 0;
}


int CSnake::getkb()
{
	char key = getchar();
	switch(key)
	{
		case 'w':
			key = CSnake::UP;
			break;

		case 's':
			key = CSnake::DOWN;
			break;

		case 'a':
			key = CSnake::LEFT;
			break;

		case 'd':
			key = CSnake::RIGHT;
			break;

		case 'q':
			key = -1;
			break;
	}

	return key;
}
