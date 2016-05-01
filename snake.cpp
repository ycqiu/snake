#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "snake.h"
#include "log.h"

using namespace std;


const int CSnake::direction[4][2] = {
		{-1, 0},  
		{1, 0},
		{0, -1},
		{0, 1}
};

CSnake::CSnake() : dir_(RIGHT), appleLoc_(INVALID, INVALID), plat_(H + 1)
{ 
	snake_ += char(H / 2 + 1);
	snake_ += char(W / 2 + 1);
	putApple();

	resetPlat();
}

void CSnake::resetPlat()
{
	for(std::vector<std::string>::size_type i = 0; i < plat_.size(); ++i)
	{
		plat_[i].assign(W + 1, ' ');	
	}
}

void CSnake::fillPlat()
{
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
			v = *(pv + dir_);
		}

		if(i != 0 && i == snake_.length() - 2)
		{
			v = 't';
		}

		plat_[r][c] = v;
	}
}


int CSnake::draw()
{
	int ret = 0;
	if(snake_.length() % 2)
	{
		ERROR_LOG("snake length: %d", snake_.length());	
		return ret = -1;
	}

	system("clear");
	fillPlat();
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

int CSnake::gameOver(int r, int c) const
{
	int ret = 0;
	if( !(r >= 1 && c >= 1 && r <= H && c <= W) )
	{
		ERROR_LOG("hit wall");
		return ret = -1;
	}

	//检查当前蛇头位置是否与蛇身重合(不包括蛇尾,因为蛇已经移动了)	
	for(std::string::size_type i = 0; i < snake_.length() - 2; i += 2)
	{
		if(r == snake_[i] && c == snake_[i + 1])	
		{
			ERROR_LOG("eat self");
			return ret = -2;
		}
	}

	return ret;
}

int CSnake::go()
{
	int ret = 0;
	int r = snake_[0] + direction[dir_][0];
	int c = snake_[1] + direction[dir_][1];
	ret = gameOver(r, c);
	if(ret)
	{
		ERROR_LOG("gameOver ret: %d", ret);
		return ret = -1;	
	}

	std::string pos; 
	pos += char(r);
	pos += char(c);
	snake_.insert(0, pos);

	//能不能吃苹果
	if(r == appleLoc_.first && c == appleLoc_.second)
	{
		/*
		if(snake_.length() > 10)
		{
			snake_.erase(snake_.length() - 2, 2);
		}
		*/
		//重新放苹果		
		ret = putApple();
		printPlat();	
		//sleep(2);
		if(ret)
		{
			INFO_LOG("putApple ret: %d", ret);
			return ret = 1;
		}
		return ret = 2;
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
		ERROR_LOG("out range dir: %d", d);
		return ret = -1;
	}

	//蛇头和蛇的第二个节点不能相撞,如果撞上了表示该方向不能走
	if(snake_[0] + direction[d][0] == snake_[2] &&
		snake_[1] + direction[d][1] == snake_[3]) 
	{
		INFO_LOG("wrong dir, old: %d, new: %d", dir_, d);
		return ret = 1;
	}

	dir_ = d;
	return ret;
}

int CSnake::putApple()
{
	int ret = 0;
	if(snake_.length() / 2 == W * H)
	{
		INFO_LOG("plat full");
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
	INFO_LOG("new apple: (%d, %d)", appleLoc_.first, appleLoc_.second);
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

void CSnake::printSnake(const std::string& s, const std::string& name)
{
	DEBUG_LOG("%s: ", name.c_str());
	for(std::string::size_type i = 0; i < s.length(); i += 2)	
	{
		//cout << (int)s[i] << " " << (int)s[i + 1] << "| ";
		DEBUG_LOG("%d %d| ", (int)s[i], (int)s[i + 1]); 
	}
}

void CSnake::printPlat(const std::string& name)
{
	fillPlat();
	DEBUG_LOG("%s:", name.c_str());
	const char block = '#';
	std::string line(W + 2, block);
	DEBUG_LOG("%s", line.c_str());
	for(int i = 1; i <= H; ++i)
	{
		//DEBUG_LOG("#%s#", (plat_[i].c_str() + 1));
		DEBUG_LOG("#%s#", (plat_[i].c_str() + 1));
	}
	DEBUG_LOG("%s", line.c_str());
}
